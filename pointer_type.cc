#include <cassert>

#include <dwarf.h>
#include <libdwarf.h>

#include "common.h"
#include "pointer_type.h"

using std::map;
using std::ostream;
using std::endl;

PointerType::PointerType(Dwarf_Off id)
  : Type(id)
{
}

int
PointerType::init(Dwarf_Debug dbg, map<Dwarf_Off, Type*>& types)
{
  Dwarf_Die die;
  Dwarf_Error err;
  Dwarf_Attribute attr;
  Dwarf_Off baseTypeId;
  int rc;
  
  rc = dwarf_offdie(dbg, this->id(), &die, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_attr(die, DW_AT_type, &attr, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_global_formref(attr, &baseTypeId, &err);
  assert(rc == DW_DLV_OK);

  map<Dwarf_Off, Type*>::iterator it = types.find(baseTypeId);
  assert(it != types.end());

  baseType_ = it->second;

  rc = get_attr_udata(die, DW_AT_byte_size, &size_);
  assert(rc == 0);

  return 0;
}

int
PointerType::dumpData(void* data, int indent, ostream& out)
{
  (void) indent;

  out << data;
  return 0;
}

Dwarf_Unsigned
PointerType::size() const
{
  return size_;
}

bool
PointerType::isCharacter() const
{
  return false;
}
