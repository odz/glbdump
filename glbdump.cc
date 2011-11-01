#include <cassert>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <climits>

#include <ltdl.h>

#include "dumper.h"

using std::cout;
using std::cerr;
using std::string;
using std::endl;
using std::map;
using std::vector;
using std::ostringstream;

namespace {
  void print_usage_message(const char* progname);
}

int
main(int argc, char** argv)
{
  const char* object_file;
  const char* symbol_name;
  char object_file_path[PATH_MAX];
  int rc;
  char* p;

  if (argc != 3)
    {
      print_usage_message(argv[0]);
      exit(1);
    }

  rc = lt_dlinit();
  assert(rc == 0);

  object_file = argv[1];
  symbol_name = argv[2];
  p = realpath(object_file, object_file_path);
  assert(p);

  Dumper dumper;
  rc = dumper.init(object_file);
  if (rc != 0)
    {
      cerr << "can't read " << object_file << endl;
    }

  lt_dlhandle dl;
  void* data;
  dl = lt_dlopen(object_file_path);
  if (!dl)
    {
      cerr << lt_dlerror() << endl;
      return -1;
    }

  data = lt_dlsym(dl, symbol_name);
  if (!data)
    {
      cerr << lt_dlerror() << endl;
      return -1;
    }

  dumper.dumpData(data, symbol_name, std::cout);

  rc = lt_dlclose(dl);
  assert(rc == 0);

  lt_dlexit();
  
  return 0;
}

namespace {
  void
  print_usage_message(const char* progname)
  {
    cerr << "Usage: " << progname << " <object file> <symbol name>" << endl;
  }
}
