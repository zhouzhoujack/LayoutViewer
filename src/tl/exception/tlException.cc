#include "tlException.h"
#include "tlString.h"

namespace tl
{

// -----------------------------------------------------------------------------
//  Exception implementation

void
Exception::init (const std::string &fmt, const std::vector<tl::Variant> &a)
{ 
  m_msg = tl::sprintf (fmt, a);
}

}

