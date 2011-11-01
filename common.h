// -*- c++ -*-

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>

#include <libdwarf.h>

#define UNUSED __attribute__((unused))

void print_error(const char* name, int rc, Dwarf_Error err);
int  get_name(Dwarf_Die die, std::string& outstr);
int  get_attr_udata(Dwarf_Die die, Dwarf_Half an, Dwarf_Unsigned* ret);
int  get_size(Dwarf_Die die, Dwarf_Unsigned* ret);
int  get_type(Dwarf_Die die, Dwarf_Off* ret);
bool is_global_variable(Dwarf_Die die);
void print_char_repr(char ch, std::ostream& out);
void print_indent(int indent, std::ostream& out);

#endif
