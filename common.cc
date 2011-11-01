#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>
#include <cctype>

#include <dwarf.h>
#include <libdwarf.h>

#include "common.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ostream;
using std::setw;

namespace {
  const int BASIC_OFFSET = 4;
}

void
print_error(const char* msg, int dwarf_code, Dwarf_Error err)
{
  if (dwarf_code == DW_DLV_ERROR)
    {
      const char* errmsg = dwarf_errmsg(err);
      long long myerr = dwarf_errno(err);

      cerr << "ERROR:  " << msg << ":  " << errmsg << "(" << myerr << ")" << endl;
    }
  else if (dwarf_code == DW_DLV_NO_ENTRY)
    {
      cerr << "NO ENTRY:  " << msg << endl;
    }
  else if (dwarf_code == DW_DLV_OK)
    {
      cerr << msg << " " << endl;
    }
  else
    {
      cerr << "InternalError:  " << msg << ":  code " << dwarf_code << endl;
    }
}

int
get_name(Dwarf_Die die, string& outstr)
{
  int rc;
  Dwarf_Error err;
  Dwarf_Attribute attr;
  char* str;

  rc = dwarf_attr(die, DW_AT_name, &attr, &err);
  if (rc == DW_DLV_NO_ENTRY)
    {
      outstr = "<unnamed>";
      return 0;
    }
  else if (rc != DW_DLV_OK)
    {
      print_error("dwarf_attr DW_AT_name", rc, err);
      return rc;
    }

  rc = dwarf_formstring(attr, &str, &err);
  if (rc != DW_DLV_OK)
    {
      print_error("dwarf_formstring DW_AT_name", rc, err);
      return rc;
    }

  outstr = str;

  return 0;
}

int
get_attr_udata(Dwarf_Die die, Dwarf_Half an, Dwarf_Unsigned* ret)
{
  int rc;
  Dwarf_Error err;
  Dwarf_Attribute attr;

  rc = dwarf_attr(die, an, &attr, &err);
  if (rc != DW_DLV_OK)
    {
      print_error("dwarf_attr", rc, err);
      return rc;
    }

  rc = dwarf_formudata(attr, ret, &err);
  if (rc != DW_DLV_OK)
    {
      print_error("dwarf_formudata", rc, err);
      return rc;
    }

  return 0;
}

int
get_size(Dwarf_Die die, Dwarf_Unsigned* size)
{
  return get_attr_udata(die, DW_AT_byte_size, size);
}

int
get_type(Dwarf_Die die, Dwarf_Off* ret)
{
  int rc;
  Dwarf_Error err;
  Dwarf_Attribute attr;
  Dwarf_Off id, aoff, off;

  rc = dwarf_dieoffset(die, &aoff, &err);
  if (rc != DW_DLV_OK)
    {
      print_error("dwarf_dieoffset", rc, err);
      return rc;
    }
  rc = dwarf_die_CU_offset(die, &off, &err);
  if (rc != DW_DLV_OK)
    {
      print_error("dwarf_die_CU_offset", rc, err);
      return rc;
    }

  rc = dwarf_attr(die, DW_AT_type, &attr, &err);
  if (rc == DW_DLV_NO_ENTRY)
    {
      return 0;
    }
  else if (rc != DW_DLV_OK)
    {
      print_error("dwarf_attr", rc, err);
      return rc;
    }
  rc = dwarf_formref(attr, &id, &err);
  if (rc != DW_DLV_OK)
    {
      print_error("dwarf_formref", rc, err);
      return rc;
    }

  *ret = aoff-off+id;
  return 0;
}

bool is_global_variable(Dwarf_Die die)
{
  Dwarf_Error err;
  Dwarf_Attribute attr;
  Dwarf_Bool hasattr;
  Dwarf_Bool isExternal;
  int rc;

  rc = dwarf_hasattr(die, DW_AT_external, &hasattr, &err);
  assert(rc == DW_DLV_OK);

  if (!hasattr)
    {
      return false;
    }

  rc = dwarf_attr(die, DW_AT_external, &attr, &err);
  assert(rc == DW_DLV_OK);

  rc = dwarf_formflag(attr, &isExternal, &err);
  assert(rc == DW_DLV_OK);
    
  if (isExternal)
    {
      return true;
    }
  else
    {
      return false;
    }
}

void
print_char_repr(char ch, ostream& out)
{
  // FIXME: handling escape and octal representation
  switch (ch)
    {
    case '\\':
    case '\'':
    case '\"':
      out << '\\' << ch;
      break;

    case '\0':
      out << "\\0";
      break;

    case '\a':
      out << "\\a";
      break;

    case '\b':
      out << "\\b";
      break;

    case '\f':
      out << "\\f";
      break;

    case '\n':
      out << "\\n";
      break;

    case '\r':
      out << "\\r";
      break;

    case '\t':
      out << "\\t";
      break;

    case '\v':
      out << "\\v";
      break;

    default:
      out << ch;
      break;
    }
}

void
print_indent(int indent, ostream& out)
{
  for (int i = 0; i < indent; i++)
    {
      out << setw(BASIC_OFFSET) << ' ';
    }
}
