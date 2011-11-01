// -*- c++ -*-

#include "type.h"

class BaseType : public Type
{
public:
  BaseType(Dwarf_Off id);
  virtual int init(Dwarf_Debug dbg, std::map<Dwarf_Off, Type*>& types);
  virtual int dumpData(void* data, int indent, std::ostream& out);
  virtual Dwarf_Unsigned size() const;
  virtual bool isCharacter() const;
private:
  std::string name_;
  Dwarf_Unsigned size_;
  Dwarf_Unsigned encoding_;
};
