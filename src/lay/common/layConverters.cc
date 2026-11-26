#include "layConverters.h"
#include "detail/utilsString.h"
#include "tlInternational.h"
#include "tlString.h"

namespace lay
{

// ----------------------------------------------------------------
//  ColorConverter implementation

std::string 
ColorConverter::to_string (const QColor &c) const
{
  if (! c.isValid ()) {
    return "auto";
  } else {
    return geo::to_string (c.name ());
  }
}

void 
ColorConverter::from_string (const std::string &s, QColor &c) const
{
  std::string t (tl::trim (s));
  if (t == "auto") {
    c = QColor ();
  } else {
    c = QColor (t.c_str ());
  } 
}

}

