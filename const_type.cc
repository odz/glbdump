#include <cassert>

#include <dwarf.h>
#include <libdwarf.h>

#include "common.h"
#include "const_type.h"

using std::map;
using std::ostream;

ConstType::ConstType(Dwarf_Off id)
  : Type(id)
{
}

int
ConstType::init(Dwarf_Debug dbg, map<Dwarf_Off, Type*>& types)
{
  Dwarf_Error err;
  Dwarf_Die die;
  Dwarf_Off baseTypeOff;
  Dwarf_Attribute attr;
  int rc;


  rc = dwarf_offdie(dbg, id(), &die, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_attr(die, DW_AT_type, &attr, &err);
  assert(rc == DW_DLV_OK);
  
  rc = dwarf_global_formref(attr, &baseTypeOff, &err);
  assert(rc == DW_DLV_OK);

  baseType_ = types[baseTypeOff];

  return 0;
}

int
ConstType::dumpData(void* data UNUSED, int indent UNUSED, ostream& out UNUSED)
{
  return this->baseType_->dumpData(data, indent, out);
}

Dwarf_Unsigned
ConstType::size() const
{
  return baseType_->size();
}

bool
ConstType::isCharacter() const
{
  return baseType_->isCharacter();
}
