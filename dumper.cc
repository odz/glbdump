#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cassert>

#include <unistd.h>
#include <fcntl.h>

#include <libelf.h>
#include <dwarf.h>
#include <libdwarf.h>

#include "dumper.h"
#include "common.h"
#include "type.h"
#include "base_type.h"
#include "const_type.h"
#include "pointer_type.h"
#include "structure_type.h"
#include "array_type.h"
#include "typedef.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ostringstream;
using std::map;
using std::vector;
using std::ostream;

class DumperImpl
{
public:
  int init(const char* filename);
  int processFile(Elf* elf);
  int processDie(Dwarf_Debug dbg, Dwarf_Die die, int depth);
  int dumpData(void* p, const char* name, ostream& out) const;
  int dumpType(const char* name, ostream& out) const;
  int getTypeInfo(Dwarf_Off id, Type** type) const;
  int addVariable(Dwarf_Die die);
private:
  map<Dwarf_Off, Type*> types_;
  map<string, Dwarf_Off> variables_;
};

#if 0
class TypeInfo
{
public:
public:
  virtual int init(Dwarf_Die die)               = 0;
  virtual Dwarf_Off id()                        = 0;
  virtual string name(const DumperImpl& dumper) = 0;
  virtual
  int
  dump(void* data, int indent,
       const DumperImpl& dumper, ostream& out)
  {
    (void) data;
    (void) indent;
    (void) dumper;
    (void) out;

    return 0;
  }
};

class PrimitiveType : public TypeInfo
{
public:
  virtual int
  init(Dwarf_Die die)
  {
    int rc;
    Dwarf_Error err;
    char* str;

    rc = dwarf_diename(die, &str, &err);
    assert(rc == DW_DLV_OK);

    rc = get_size(die, &size_);
    assert(rc == 0);

    rc = dwarf_dieoffset(die, &id_, &err);
    assert(rc == DW_DLV_OK);

    name_ = str;

    return 0;
  }

  virtual Dwarf_Off
  id()
  {
    return id_;
  }

  virtual string
  name(const DumperImpl& dumper)
  {
    (void) dumper;
    return name_;
  }

  virtual int
  dump(void* data, int indent,
       const DumperImpl& dumper, ostream& out)
  {
    (void) indent;
    (void) dumper;

    if (size_ == 1)
      {
	if (name_ == "bool")
	  {
	    bool val = *reinterpret_cast<bool *>(data);
	    if (val)
	      {
		out << "true";
	      }
	    else
	      {
		out << "false";
	      }
	  }
      }
    else if (size_ == 4)
      {
	out << *reinterpret_cast<int *>(data);
      }

    return 0;
  }
private:
  string         name_;
  Dwarf_Unsigned size_;
  Dwarf_Off      id_;
};

class StructType : public TypeInfo
{
public:
  virtual int
  init(Dwarf_Die die)
  {
    int rc;
    char* str;
    Dwarf_Error err;

    rc = dwarf_diename(die, &str, &err);
    assert(rc == DW_DLV_OK);
    name_ = str;

    rc = dwarf_dieoffset(die, &id_, &err);
    if (rc != 0)
      {
        print_error("dwarf_dieoffset", rc, err);
        return rc;
      }

    return 0;
  }

  virtual Dwarf_Off
  id()
  {
    return id_;
  }

  virtual string
  name(const DumperImpl& dumper)
  {
    (void) dumper;
    return "struct " + name_;
  }
private:
  struct Member {
    string name;
    Dwarf_Addr offset;
    Dwarf_Off type;
    Dwarf_Unsigned bit_size;
    Dwarf_Unsigned bit_offset;
    Dwarf_Unsigned byte_size;
  };

  int
  addMember(Dwarf_Die die)
  {
    Dwarf_Error err;
    char* str;
    int rc;

    rc = dwarf_diename(die, &str, &err);
    assert(rc == DW_DLV_OK);
    
    return 0;
  }

  Dwarf_Off
  getOffset(Dwarf_Die die)
  {
    Dwarf_Error err;
    Dwarf_Attribute attr;
    Dwarf_Locdesc** loc;
    Dwarf_Signed size;
    Dwarf_Off off;
    int rc;

    rc = dwarf_attr(die, DW_AT_data_member_location, &attr, &err);
    assert(rc == DW_DLV_OK);

    rc = dwarf_loclist_n(attr, &loc, &size, &err);
    assert(rc);

    off = loc[0]->ld_s[0].lr_number;

    for (int i = 0; i < size; i++)
      {
      }
    return off;
  }

  vector<Member> member_;
  string name_;
  Dwarf_Off id_;
};

class ArrayType : public TypeInfo
{
public:
  virtual int
  init(Dwarf_Die die)
  {
    int rc;
    Dwarf_Error err;
    Dwarf_Die child;
    Dwarf_Unsigned upperBound;

    rc = get_type(die, &type_);
    if (rc != 0)
      {
        return rc;
      }

    rc = dwarf_child(die, &child, &err);
    if (rc != DW_DLV_OK)
      {
        print_error("dwarf_child", rc, err);
        return rc;
      }

    rc = get_attr_udata(child, DW_AT_upper_bound, &upperBound);
    if (rc != 0)
      {
        return rc;
      }

    size_ = upperBound + 1;

    rc = dwarf_dieoffset(die, &id_, &err);
    assert(rc == DW_DLV_OK);

    return 0;
  }

  virtual Dwarf_Off
  id()
  {
    return id_;
  }

  virtual string
  name(const DumperImpl& dumper)
  {
    int rc;
    TypeInfo* child;
    string childName;

    rc = dumper.getTypeInfo(type_, &child);
    assert(rc == 0);

    childName = child->name(dumper);
    ostringstream out;
    out << childName;
    if (childName[childName.length() - 1] != ']')
      {
        out << ' ';
      }
    out << "[" << size_ << "]";

    return out.str();
  }
private:
  Dwarf_Unsigned size_;
  Dwarf_Off      type_;
  Dwarf_Off      id_;
};

class PointerType : public TypeInfo
{
public:
  virtual int
  init(Dwarf_Die die)
  {
    Dwarf_Error err;
    int rc;
    
    rc = dwarf_dieoffset(die, &id_, &err);
    assert(rc == DW_DLV_OK);

    rc = dwarf_tag(die, &tag_, &err);
    assert(rc == DW_DLV_OK);

    rc = get_type(die, &type_);
    assert(rc == 0);

    return 0;
  }

  virtual Dwarf_Off
  id()
  {
    return id_;
  }

  virtual string
  name(const DumperImpl& dumper)
  {
    int rc;
    TypeInfo* child;
    ostringstream outstr;

    rc = dumper.getTypeInfo(type_, &child);
    assert(rc == 0);

    outstr << child->name(dumper);

    if (tag_ == DW_TAG_pointer_type)
      {
	outstr << "*";
      }
    else if (tag_ == DW_TAG_reference_type)
      {
	outstr << "&";
      }
    else
      {
	// unreachable
	assert(0);
      }

    return outstr.str();
  }
private:
  Dwarf_Off type_;
  Dwarf_Off id_;
  Dwarf_Half tag_;
};

class VolatileType : public TypeInfo
{
public:
  virtual int
  init(Dwarf_Die die)
  {
    Dwarf_Error err;
    int rc;

    rc = dwarf_dieoffset(die, &id_, &err);
    assert(rc == DW_DLV_OK);

    rc = get_type(die, &type_);
    assert(rc == 0);

    return 0;
  }

  virtual Dwarf_Off
  id()
  {
    return id_;
  }

  virtual string
  name(const DumperImpl& dumper)
  {
    TypeInfo* child;
    int rc;

    rc = dumper.getTypeInfo(type_, &child);
    assert(rc == 0);

    return "volatile " + child->name(dumper);
  }
private:
  Dwarf_Off type_;
  Dwarf_Off id_;
};

class ConstType : public TypeInfo
{
  virtual int
  init(Dwarf_Die die)
  {
    Dwarf_Error err;
    int rc;

    rc = dwarf_dieoffset(die, &id_, &err);
    assert(rc == DW_DLV_OK);

    rc = get_type(die, &type_);
    assert(rc == 0);

    return 0;
  }

  virtual Dwarf_Off
  id()
  {
    return id_;
  }

  virtual string
  name(const DumperImpl& dumper)
  {
    TypeInfo* child;
    int rc;

    rc = dumper.getTypeInfo(type_, &child);
    assert(rc == 0);

    return "const " + child->name(dumper);
  }

private:
  Dwarf_Off id_;
  Dwarf_Off type_;
};

class EnumerationType : public TypeInfo
{
public:
  virtual int
  init(Dwarf_Die die)
  {
    Dwarf_Error err;
    char* str;
    int rc;

    rc = dwarf_dieoffset(die, &id_, &err);
    assert(rc == DW_DLV_OK);

    rc = dwarf_diename(die, &str, &err);
    assert(rc == DW_DLV_OK);

    name_ = str;

    return 0;
  }

  virtual Dwarf_Off
  id()
  {						
    return id_;
  }

  virtual string
  name(const DumperImpl& dumper)
  {
    (void) dumper;
    return "enum " + name_;
  }
private:
  Dwarf_Off id_;
  string name_;
};

#endif
Dumper::Dumper()
  : impl_(new DumperImpl)
{
}

Dumper::~Dumper()
{
  delete impl_;
}

int
Dumper::init(const char* object_file)
{
  return this->impl_->init(object_file);
}

int
Dumper::dumpData(void* p, const char* name, ostream& out)
{
  int rc;

  rc = impl_->dumpData(p, name, out);

  return rc;
}

int
DumperImpl::init(const char* object_file)
{
  elf_version(EV_NONE);
  if (elf_version(EV_CURRENT) == EV_NONE)
    {
      cerr << "libelf.a is out of date." << endl;
    }

  int f;

  f = open(object_file, O_RDONLY);
  if (f < 0)
    {
      cerr << "can't open " << object_file << endl;
      return -1;
    }

  Elf_Cmd cmd;
  Elf* arf;
  Elf* elf;
  int rc = 0;

  cmd = ELF_C_READ;
  arf = elf_begin(f, cmd, static_cast<Elf *>(0));

  while ((elf = elf_begin(f, cmd, arf)) != 0)
    {
      Elf32_Ehdr* eh32;
      (void) eh32;		// for ignoring warning
      eh32 = elf32_getehdr(elf);
      cmd = elf_next(elf);
      rc |= processFile(elf);
      elf_end(elf);
    }

  elf_end(arf);
  close(f);

#if 0
  for (map<Dwarf_Off, Type*>::iterator it = types_.begin();
       it != types_.end();
       ++it)
    {
      //cout << it->second->name(*this) << endl;
    }
#endif

  return rc;
}

int
DumperImpl::processFile(Elf* elf)
{
  int rc;
  Dwarf_Error err;
  Dwarf_Debug dbg;
  int dres;

  dres = dwarf_elf_init(elf, DW_DLC_READ, 0, 0, &dbg, &err);
  if (dres == DW_DLV_NO_ENTRY)
    {
      return 0;
    }
  else if (dres != DW_DLV_OK)
    {
      print_error("dwarf_elf_init", dres, err);
    }

  Dwarf_Die die;
  Dwarf_Unsigned cu_header_length = 0;
  Dwarf_Unsigned abbrev_offset = 0;
  Dwarf_Half version_stamp = 0;
  Dwarf_Half address_size = 0;
  Dwarf_Unsigned next_cu_offset = 0;

  while (true)
    {
      rc = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,
                                &abbrev_offset, &address_size,
                                &next_cu_offset, &err);
      if (rc != DW_DLV_OK)
        {
          break;
        }

      rc = dwarf_siblingof(dbg, 0, &die, &err);
      if (rc == DW_DLV_OK)
        {
          rc = processDie(dbg, die, 0);
          if (rc)
            {
              return rc;
            }
        }
    }

  assert(rc != DW_DLV_ERROR);

  for (map<Dwarf_Off, Type*>::iterator it = types_.begin();
       it != types_.end();
       ++it)
    {
      rc = it->second->init(dbg, types_);
      assert(rc == 0);
    }

  rc = dwarf_finish(dbg, &err);
  if (rc != DW_DLV_OK)
    {
      print_error("dwarf_finish", rc, err);
    }

  return 0;
}

int
DumperImpl::processDie(Dwarf_Debug dbg, Dwarf_Die die, int depth)
{
  Dwarf_Error err;
  Dwarf_Half tag;
  Dwarf_Off id;
  int rc;

  while (true)
    {
      rc = dwarf_tag(die, &tag, &err);
      assert(rc == DW_DLV_OK);
      rc = dwarf_dieoffset(die, &id, &err);
      assert(rc == DW_DLV_OK);

      Type* type = 0;

      switch (tag)
        {
        case DW_TAG_base_type:
          type = new BaseType(id);
          break;

        case DW_TAG_structure_type:
        case DW_TAG_union_type:
	  type = new StructureType(id);
          break;

        case DW_TAG_array_type:
	  type = new ArrayType(id);
          break;

        case DW_TAG_reference_type:
        case DW_TAG_pointer_type:
	  type = new PointerType(id);
          break;

        case DW_TAG_const_type:
	  type = new ConstType(id);
          break;

        case DW_TAG_enumeration_type:
          break;

	case DW_TAG_volatile_type:
	  break;

	case DW_TAG_typedef:
	  type = new Typedef(id);
	  break;

        case DW_TAG_variable:
	  if (is_global_variable(die))
	    {
	      addVariable(die);
	    }
          break;
        }

      if (type)
        {
          types_[type->id()] = type;
        }

      Dwarf_Die child;
      rc = dwarf_child(die, &child, &err);
      if (rc == DW_DLV_OK)
        {
          rc = processDie(dbg, child, depth + 1);
          if (rc)
            {
              return rc;
            }
        }
      else if (rc == DW_DLV_ERROR)
        {
          print_error("dwarf_child", rc, err);
          return rc;
        }

      rc = dwarf_siblingof(dbg, die, &die, &err);
      if (rc == DW_DLV_NO_ENTRY)
        {
          break;
        }
      else if (rc != DW_DLV_OK)
        {
          print_error("dwarf_siblingof", rc, err);
          return rc;
        }
    }

  return 0;
}

int
DumperImpl::dumpData(void* p, const char* name, ostream& out) const
{
  int rc;

  map<string, Dwarf_Off>::const_iterator varIt = variables_.find(string(name));

  if (varIt == variables_.end())
    {
      return -1;
    }

  Type* type;
  rc = getTypeInfo(varIt->second, &type);
  assert(rc == 0);

  out << name << " = ";
  type->dumpData(p, 0, out);
  out << ";" << endl;
  
  return 0;
}

int
DumperImpl::getTypeInfo(Dwarf_Off id, Type** type) const
{
  map<Dwarf_Off, Type*>::const_iterator it;

  it = types_.find(id);
  if (it != types_.end())
    {
      *type = it->second;
      return 0;
    }
  else
    {
      return -1;
    }
}

int
DumperImpl::addVariable(Dwarf_Die die)
{
  string name;
  Dwarf_Off type;
  int rc;

  rc = get_name(die, name);
  if (rc != 0)
    {
      return rc;
    }

  rc = get_type(die, &type);
  if (rc != 0)
    {
      return rc;
    }

  variables_[name] = type;
  return 0;
}
