// -*- c++ -*-

#ifndef DUMPER_H
#define DUMPER_H

#include <string>
#include <iostream>

class DumperImpl;

class Dumper
{
public:
  Dumper();
  ~Dumper();
  int init(const char* filename);
  int dumpData(void* p, const char* name, std::ostream& out);
  int dumpType(const char* name, std::ostream& out);
private:
  DumperImpl* impl_;
};


#endif
