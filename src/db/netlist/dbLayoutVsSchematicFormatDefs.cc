#include "dbLayoutVsSchematicFormatDefs.h"

namespace db
{
// -----------------------------------------------------------------------------
//  LayoutVsSchematicFormatDefs implementation


namespace lvs_std_format
{
  const char *lvs_magic_string_cstr = "#%lvsdb-klayout";
  DB_PUBLIC std::string ShortKeys::lvs_magic_string (lvs_magic_string_cstr);
  DB_PUBLIC std::string LongKeys::lvs_magic_string (lvs_magic_string_cstr);

  DB_PUBLIC std::string LongKeys::reference_key ("reference");
  DB_PUBLIC std::string LongKeys::layout_key ("layout");
  DB_PUBLIC std::string LongKeys::xref_key ("xref");

  DB_PUBLIC std::string LongKeys::mismatch_key ("mismatch");
  DB_PUBLIC std::string LongKeys::match_key ("match");
  DB_PUBLIC std::string LongKeys::nomatch_key ("nomatch");
  DB_PUBLIC std::string LongKeys::warning_key ("warning");
  DB_PUBLIC std::string LongKeys::skipped_key ("skipped");

  DB_PUBLIC std::string ShortKeys::reference_key ("H");
  DB_PUBLIC std::string ShortKeys::layout_key ("J");
  DB_PUBLIC std::string ShortKeys::xref_key ("Z");

  DB_PUBLIC std::string ShortKeys::mismatch_key ("0");
  DB_PUBLIC std::string ShortKeys::match_key ("1");
  DB_PUBLIC std::string ShortKeys::nomatch_key ("X");
  DB_PUBLIC std::string ShortKeys::warning_key ("W");
  DB_PUBLIC std::string ShortKeys::skipped_key ("S");
}

}
