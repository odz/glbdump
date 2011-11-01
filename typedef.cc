#include <cassert>
#include <dwarf.h>

#include "typedef.h"

using std::map;
using std::ostream;

Typedef::Typedef(Dwarf_Unsigned id)
  : Type(id)
{
}

int
Typedef::init(Dwarf_Debug dbg, map<Dwarf_Unsigned, Type*>& types)
{
  Dwarf_Error err;
  Dwarf_Die die;
  Dwarf_Attribute attr;
  Dwarf_Off typeId;
  int rc;

  rc = dwarf_offdie(dbg, this->id(), &die, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_attr(die, DW_AT_type, &attr, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_global_formref(attr, &typeId, &err);
  assert(rc == DW_DLV_OK);

  map<Dwarf_Unsigned, Type*>::iterator it
    = types.find(typeId);
  assert(it != types.end());
  this->originalType_ = it->second;

  return 0;
}

int
Typedef::dumpData(void* data, int indent, ostream& out)
{
  return this->originalType_->dumpData(data, indent, out);
}

Dwarf_Unsigned
Typedef::size() const
{
  return this->originalType_->size();
}

bool
Typedef::isCharacter() const
{
  return this->originalType_->isCharacter();
}
