// -*- c++ -*-

#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <map>
#include <libdwarf.h>

class Type
{
public:
  Type(Dwarf_Off id);
  virtual ~Type();
  Dwarf_Off id() const;
  virtual Dwarf_Unsigned size() const = 0;
  virtual int init(Dwarf_Debug dbg, std::map<Dwarf_Off, Type*>& types) = 0;
  virtual int dumpData(void* data, int indent, std::ostream& out) = 0;
  virtual bool isCharacter() const = 0;
private:
  Dwarf_Off id_;
  Dwarf_Unsigned size_;
};

#endif
