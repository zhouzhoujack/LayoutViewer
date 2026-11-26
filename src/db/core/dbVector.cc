#include "dbVector.h"

// ----------------------------------------------------------------
//  Implementation of the custom extractors

namespace {

  template <class C>
  bool _test_extractor_impl (tl::Extractor &ex, db::vector<C> &p)
  {
    //  TODO: this is not really a "test": we should move back if we did not
    //  receive a ",".
    C x = 0;
    if (ex.try_read (x)) {
      ex.expect (",");
      C y = 0;
      ex.read (y);
      p = db::vector<C> (x, y);
      return true;
    } else {
      return false;
    }
  }

  template <class C>
  void _extractor_impl (tl::Extractor &ex, db::vector<C> &p)
  {
    if (! _test_extractor_impl (ex, p)) {
      ex.error (tl::to_string (tr ("Expected a vector specification")));
    }
  }

}

namespace tl 
{
  
template <> 
void 
extractor_impl (tl::Extractor &ex, db::Vector &p)
{
  _extractor_impl (ex, p);
}

template <> 
void 
extractor_impl (tl::Extractor &ex, db::DVector &p)
{
  _extractor_impl (ex, p);
}

template <> 
bool 
test_extractor_impl (tl::Extractor &ex, db::Vector &p)
{
  return _test_extractor_impl (ex, p);
}

template <> 
bool 
test_extractor_impl (tl::Extractor &ex, db::DVector &p)
{
  return _test_extractor_impl (ex, p);
}

} // namespace tl

