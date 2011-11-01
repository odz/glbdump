#ifndef ARRAY_TYPE_H
#define ARRAY_TYPE_H

#include <vector>

#include "type.h"

class ArrayType : public Type
{
public:
  ArrayType(Dwarf_Off id);
  virtual int init(Dwarf_Debug dbg, std::map<Dwarf_Off, Type*>& types);
  virtual int dumpData(void* data, int indent, std::ostream& out);
  virtual Dwarf_Unsigned size() const;
  virtual bool isCharacter() const;

private:
  int dumpDataSub(void *data, Dwarf_Unsigned size, int indent, int level,
		  std::ostream& out);

private:
  Type* baseType_;
  std::vector<Dwarf_Unsigned> length_;
};

#endif

// Local Variables:
// mode: c++
// End:
