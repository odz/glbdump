#include <vector>

#include "type.h"

class StructureType : public Type
{
private:
  struct Member
  {
    std::string name;
    Dwarf_Unsigned offset;
    Type* type;
    Dwarf_Unsigned byteSize;
    Dwarf_Unsigned bitSize;
    Dwarf_Unsigned bitOffset;
  };

public:
  StructureType(Dwarf_Off id);
  virtual int init(Dwarf_Debug dbg, std::map<Dwarf_Off, Type*>& types);
  virtual int dumpData(void* data, int indent, std::ostream& out);
  virtual Dwarf_Unsigned size() const;
  virtual bool isCharacter() const;

private:
  int addMember(Dwarf_Debug dbg, Dwarf_Die die,
		std::map<Dwarf_Off, Type*>& types);

private:
  std::vector<Member> members_;
  Dwarf_Unsigned size_;
};

// Local Variables:
// mode: c++
// End:
