#include <cassert>

#include <dwarf.h>
#include <libdwarf.h>

#include "common.h"
#include "type.h"

Type::Type(Dwarf_Off id)
  : id_(id)
{
}

Type::~Type()
{
}

Dwarf_Off
Type::id() const
{
  return id_;
}
