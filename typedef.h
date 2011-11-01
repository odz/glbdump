#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "type.h"

class Typedef : public Type
{
public:
  Typedef(Dwarf_Off id);
  virtual int init(Dwarf_Debug dbg, std::map<Dwarf_Off, Type*> &types);
  virtual int dumpData(void* data, int indent, std::ostream& out);
  virtual Dwarf_Unsigned size() const;
  virtual bool isCharacter() const;
private:
  Type* originalType_;
};

#endif

// Local Variables:
// mode: c++
// End:
