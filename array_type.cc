#include <cassert>
#include <numeric>
#include <algorithm>
#include <functional>

#include <dwarf.h>
#include <libdwarf.h>

#include "common.h"
#include "array_type.h"

using std::vector;
using std::map;
using std::ostream;
using std::endl;

ArrayType::ArrayType(Dwarf_Off id)
  : Type(id)
{
}

int
ArrayType::init(Dwarf_Debug dbg, map<Dwarf_Off, Type*>& types)
{
  (void) dbg;
  (void) types;

  Dwarf_Error err;
  Dwarf_Die die;
  Dwarf_Attribute typeAttr;
  Dwarf_Off typeId;
  int rc;

  rc = dwarf_offdie(dbg, this->id(), &die, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_attr(die, DW_AT_type, &typeAttr, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_global_formref(typeAttr, &typeId, &err);
  assert(rc == DW_DLV_OK);

  map<Dwarf_Off, Type*>::iterator it
    = types.find(typeId);
  assert(it != types.end());
  baseType_ = it->second;

  Dwarf_Die subrange;
  Dwarf_Attribute uboundAttr;
  Dwarf_Unsigned ubound;

  rc = dwarf_child(die, &subrange, &err);
  assert(rc == DW_DLV_OK);

  length_.clear();

  for (;;)
    {
      rc = dwarf_attr(subrange, DW_AT_upper_bound, &uboundAttr, &err);
      assert(rc == DW_DLV_OK);
      rc = dwarf_formudata(uboundAttr, &ubound, &err);
      assert(rc == DW_DLV_OK);
      length_.push_back(ubound + 1);

      rc = dwarf_siblingof(dbg, subrange, &subrange, &err);
      assert(rc == DW_DLV_OK || rc == DW_DLV_NO_ENTRY);
      if (rc == DW_DLV_NO_ENTRY)
	{
	  break;
	}
    }

  std::reverse(length_.begin(), length_.end());

  return 0;
}

int
ArrayType::dumpData(void* data, int indent, ostream& out)
{
  Dwarf_Unsigned size = this->size();
  return dumpDataSub(data, size / length_[length_.size() - 1],
		     length_.size() - 1, indent, out);
}

int
ArrayType::dumpDataSub(void* data, Dwarf_Unsigned size, int level, 
		       int indent, ostream& out)
{
  if (level == 0 && baseType_->isCharacter())
    {
      out << '"';
      for (Dwarf_Unsigned i = 0; i < length_[0]; i++)
	{
	  print_char_repr(static_cast<char *>(data)[i], out);
	}
      out << '"';
    }
  else
    {
      out << '{' << endl;
      for (Dwarf_Unsigned i = 0; i < length_[level]; i++)
	{
	  print_indent(indent + 1, out);
	  out << '[' << i << "] = ";
	  void* p
	    = static_cast<char *>(data) + size * i;

	  if (level == 0)
	    {
	      baseType_->dumpData(p, indent, out);
	    }
	  else
	    {
	      dumpDataSub(p, size / length_[level - 1], level - 1, indent, out);
	    }
	  out << "," << endl;
	}
      print_indent(indent, out);
      out << "}";
    }
  return 0;
}

Dwarf_Unsigned
ArrayType::size() const
{
  
  return std::accumulate(length_.begin(), length_.end(), baseType_->size(),
			 std::multiplies<Dwarf_Unsigned>());
}

bool
ArrayType::isCharacter() const
{
  return false;
}
