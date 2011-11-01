#include <climits>
#include <cassert>
#include <dwarf.h>
#include <libdwarf.h>

#include "common.h"
#include "structure_type.h"

using std::map;
using std::vector;
using std::ostream;
using std::endl;

namespace {
  int getMemberOffset(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Unsigned* offset);
  void printBitField(void* data, Dwarf_Unsigned byteSize,
		     Dwarf_Unsigned bitOffset, Dwarf_Unsigned bitSize, ostream& out);

  template<typename T>
  T getBitFieldValue(T value, Dwarf_Unsigned bitOffset, Dwarf_Unsigned bitSize);
}

StructureType::StructureType(Dwarf_Off id)
  : Type(id)
{
}

int
StructureType::init(Dwarf_Debug dbg, map<Dwarf_Off, Type*>& types)
{
  Dwarf_Error err;
  Dwarf_Die die;
  Dwarf_Die child;
  int rc;

  rc = dwarf_offdie(dbg, this->id(), &die, &err);
  assert(rc == DW_DLV_OK);

  rc = get_attr_udata(die, DW_AT_byte_size, &size_);
  assert(rc == 0);

  rc = dwarf_child(die, &child, &err);
  assert(rc == DW_DLV_OK);

  for (;;)
    {
      rc = this->addMember(dbg, child, types);
      assert(rc == 0);

      rc = dwarf_siblingof(dbg, child, &child, &err);
      assert(rc == DW_DLV_OK || rc == DW_DLV_NO_ENTRY);
      if (rc == DW_DLV_NO_ENTRY)
	{
	  break;
	}
    }

  return 0;
}

int
StructureType::dumpData(void* data, int indent, ostream& out)
{
  out << '{' << endl;
  for (vector<Member>::iterator it = members_.begin();
       it != members_.end();
       ++it)
    {
      print_indent(indent + 1, out);
      out << "." << it->name << " = ";
      if (it->bitSize > 0)
	{
	  printBitField(static_cast<char *>(data) + it->offset, it->byteSize, it->bitOffset, it->bitSize, out);
	}
      else
	{
	  it->type->dumpData(static_cast<char *>(data) + it->offset, indent + 1, out);
	}
      out << "," << endl;
    }
  print_indent(indent, out);
  out << '}';

  return 0;
}

int
StructureType::addMember(Dwarf_Debug dbg, Dwarf_Die die,
			 map<Dwarf_Off, Type*>& types)
{
  Dwarf_Error err;
  Dwarf_Attribute attr;
  Dwarf_Off typeId;
  Dwarf_Bool hasBitSize;
  char* str;
  Member member;
  int rc;

  rc = dwarf_diename(die, &str, &err);
  assert(rc == DW_DLV_OK);
  member.name = str;

  rc = getMemberOffset(dbg, die, &member.offset);
  assert(rc == 0);

  rc = dwarf_attr(die, DW_AT_type, &attr, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_global_formref(attr, &typeId, &err);
  assert(rc == DW_DLV_OK);

  map<Dwarf_Off, Type*>::iterator it
    = types.find(typeId);
  assert(it != types.end());
  member.type = it->second;

  rc = dwarf_hasattr(die, DW_AT_bit_size, &hasBitSize, &err);
  assert(rc == DW_DLV_OK);

  member.byteSize = 0;
  member.bitSize = 0;
  member.bitOffset = 0;

  if (hasBitSize)
    {
      rc = get_attr_udata(die, DW_AT_byte_size, &member.byteSize);
      assert(rc == DW_DLV_OK);
      
      rc = get_attr_udata(die, DW_AT_bit_size, &member.bitSize);
      assert(rc == DW_DLV_OK);

      rc = get_attr_udata(die, DW_AT_bit_offset, &member.bitOffset);
      assert(rc == DW_DLV_OK);
    }

  members_.push_back(member);

  return 0;
}

Dwarf_Unsigned
StructureType::size() const
{
  return size_;
}

bool
StructureType::isCharacter() const
{
  return false;
}

namespace {
  int
  getMemberOffset(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Unsigned* offset)
  {
    Dwarf_Error err;
    Dwarf_Attribute attr;
    Dwarf_Locdesc** locList;
    Dwarf_Signed numberOfLocList;
    int rc;

    rc = dwarf_attr(die, DW_AT_data_member_location, &attr, &err);
    assert(rc == DW_DLV_OK);

    rc = dwarf_loclist_n(attr, &locList, &numberOfLocList, &err);
    assert(rc == DW_DLV_OK);

    *offset = locList[0]->ld_s->lr_number;

    for (Dwarf_Signed i = 0; i < numberOfLocList; i++)
      {
	dwarf_dealloc(dbg, locList[i]->ld_s, DW_DLA_LOC_BLOCK);
	dwarf_dealloc(dbg, locList[i], DW_DLA_LOCDESC);
      }

    dwarf_dealloc(dbg, locList, DW_DLA_LIST);

    return 0;
  }

  void
  printBitField(void* data, Dwarf_Unsigned byteSize,
		Dwarf_Unsigned bitOffset, Dwarf_Unsigned bitSize, ostream& out)
  {
    if (byteSize == 1)
      {
	out << static_cast<int>(getBitFieldValue(*reinterpret_cast<unsigned char *>(data), bitOffset, bitSize));
      }
    else if (byteSize == 2)
      {
	out << getBitFieldValue(*reinterpret_cast<unsigned short *>(data), bitOffset, bitSize);
      }
    else if (byteSize == 4)
      {
	out << getBitFieldValue(*reinterpret_cast<unsigned int *>(data), bitOffset, bitSize);
      }
    else if (byteSize == 8)
      {
	out << getBitFieldValue(*reinterpret_cast<unsigned long long *>(data), bitOffset, bitSize);
      }
  }

  template<typename T>
  T
  getBitFieldValue(T val, Dwarf_Unsigned bitOffset, Dwarf_Unsigned bitSize)
  {
    Dwarf_Unsigned wholeSize = CHAR_BIT * sizeof(T);
    Dwarf_Unsigned shift = wholeSize - (bitOffset + bitSize);
    T mask = (static_cast<T>(1) << bitSize) - 1;

    return (val >> shift) & mask;
  }
}
