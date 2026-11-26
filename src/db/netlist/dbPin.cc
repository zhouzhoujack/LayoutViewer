#include "dbPin.h"
#include "tlString.h"

namespace db
{

// --------------------------------------------------------------------------------
//  Pin class implementation

Pin::Pin ()
  : db::NetlistObject (), m_id (0)
{
  //  .. nothing yet ..
}

Pin::Pin (const std::string &name)
  : db::NetlistObject (), m_name (name), m_id (0)
{
  //  .. nothing yet ..
}

std::string Pin::expanded_name () const
{
  if (name ().empty ()) {
    return "$" + tl::to_string (id ());
  } else {
    return name ();
  }
}

}
