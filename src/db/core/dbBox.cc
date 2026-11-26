#include "dbBox.h"
#include "tlString.h"
#include "tlVariant.h"

namespace tl
{
// -----------------------------------------------------------------------------
//  Box implementation


template<> void extractor_impl (tl::Extractor &ex, db::Box &b)
{
  if (! test_extractor_impl (ex, b)) {
    ex.error (tl::to_string (tr ("Expected an box specification")));
  }
}

template<> void extractor_impl (tl::Extractor &ex, db::DBox &b)
{
  if (! test_extractor_impl (ex, b)) {
    ex.error (tl::to_string (tr ("Expected an box specification")));
  }
}

template<class C> bool _test_extractor_impl (tl::Extractor &ex, db::box<C> &b)
{
  typedef db::point<C> point_type;

  if (ex.test ("(")) {

    if (ex.test (")")) {
      b = db::box<C> ();
    } else {

      point_type p1, p2;
      ex.read (p1);
      ex.expect (";");
      ex.read (p2);

      b = db::box<C> (p1, p2);

      ex.expect (")");

    }

    return true;

  } else {
    return false;
  }
}

template<> bool test_extractor_impl (tl::Extractor &ex, db::Box &b)
{
  return _test_extractor_impl (ex, b);
}

template<> bool test_extractor_impl (tl::Extractor &ex, db::DBox &b)
{
  return _test_extractor_impl (ex, b);
}

}


