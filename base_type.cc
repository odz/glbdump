#include <cassert>

#include <dwarf.h>
#include <libdwarf.h>

#include "base_type.h"
#include "common.h"

using std::map;
using std::ostream;

BaseType::BaseType(Dwarf_Off id)
  : Type(id)
{
}

int
BaseType::init(Dwarf_Debug dbg, map<Dwarf_Off, Type*>& types)
{
  Dwarf_Error err;
  Dwarf_Die die;
  int rc;
  char* str;

  (void) types;

  rc = dwarf_offdie(dbg, id(), &die, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_diename(die, &str, &err);
  assert(rc == DW_DLV_OK);
  name_ = str;

  dwarf_dealloc(dbg, str, DW_DLA_STRING);

  rc = get_size(die, &size_);
  assert(rc == 0);

  rc = get_attr_udata(die, DW_AT_encoding, &encoding_);
  assert(rc == 0);

  return 0;
}

int
BaseType::dumpData(void* data, int indent UNUSED, ostream& out)
{
  if (size_ == 1)
    {
      if (encoding_ == DW_ATE_boolean)
	{
	  bool val = *reinterpret_cast<bool *>(data);
	  out << (val ? "true" : "false");
	}
      else if (encoding_ == DW_ATE_signed_char ||
	       encoding_ == DW_ATE_unsigned_char)
	{
	  char ch = *reinterpret_cast<char *>(data);
	  out << '\'';
	  print_char_repr(ch, out);
	  out << '\'';
	}
    }
  else if (size_ == 2)
    {
      if (encoding_ == DW_ATE_signed)
	{
	  out << *reinterpret_cast<short *>(data);
	}
      else
	{
	  out << *reinterpret_cast<unsigned short *>(data);
	}
    }
  else if (size_ == 4)
    {
      if (encoding_ == DW_ATE_signed)
	{
	  out << *reinterpret_cast<int *>(data);
	}
      else if (encoding_ == DW_ATE_unsigned)
	{
	  out << *reinterpret_cast<unsigned int *>(data);
	}
      else
	{
	  assert(0);
	}
    }
  else if (size_ == 8)
    {
      if (encoding_ == DW_ATE_signed)
	{
	  out << *reinterpret_cast<long long *>(data);
	}
      else if (encoding_ == DW_ATE_unsigned)
	{
	  out << *reinterpret_cast<unsigned long long *>(data);
	}
      else
	{
	  assert(0);
	}
    }
  return 0;
}

Dwarf_Unsigned
BaseType::size() const
{
  return size_;
}

bool
BaseType::isCharacter() const
{
  bool ret;

  switch (encoding_)
    {
    case DW_ATE_signed_char:
    case DW_ATE_unsigned_char:
      ret = true;
      break;

    default:
      ret = false;
      break;
    }

  return ret;
}
