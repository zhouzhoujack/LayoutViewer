#include "rdb.h"
#include "rdbReader.h"

#include <string.h>

namespace rdb
{

bool 
match_filename_to_format (const std::string &fn, const std::string &fmt)
{
  const char *fp = fmt.c_str ();
  while (*fp && *fp != '(') {
    ++fp;
  }
  while (*fp && *fp != ')') {
    if (*++fp == '*') {
      ++fp;
    }
    const char *fpp = fp;
    while (*fpp && *fpp != ' ' && *fpp != ')') {
      ++fpp;
    }
    if (fn.size () > (unsigned int) (fpp - fp) && strncmp (fn.c_str () + fn.size () - (fpp - fp), fp, fpp - fp) == 0) {
      return true;
    }
    fp = fpp;
    while (*fp == ' ') {
      ++fp;
    }
  }
  return false;
}

// ---------------------------------------------------------------
//  Reader implementation

Reader::Reader (tl::InputStream &stream)
  : mp_actual_reader (0)
{
  for (tl::Registrar<rdb::FormatDeclaration>::iterator rdr = tl::Registrar<rdb::FormatDeclaration>::begin (); rdr != tl::Registrar<rdb::FormatDeclaration>::end () && ! mp_actual_reader; ++rdr) {
    stream.reset ();
    if (rdr->detect (stream)) {
      stream.reset ();
      mp_actual_reader = rdr->create_reader (stream);
    }
  }

  if (! mp_actual_reader) {
    throw rdb::ReaderException (tl::to_string (tr ("Marker database has unknown format")));
  }
}

Reader::~Reader ()
{
  if (mp_actual_reader) {
    delete mp_actual_reader;
    mp_actual_reader = 0;
  }
}

}

