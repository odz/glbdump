#include "type.h"

class PointerType : public Type
{
public:
  PointerType(Dwarf_Off id);
  virtual int init(Dwarf_Debug dbg, std::map<Dwarf_Off, Type*>& types);
  virtual int dumpData(void *data, int indent, std::ostream& out);
  virtual Dwarf_Unsigned size() const;
  virtual bool isCharacter() const;

private:
  Type*	baseType_;
  Dwarf_Unsigned size_;
};

// Local Variables:
// mode: c++
// End:
