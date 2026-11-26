#include "dbText.h"

namespace db
{

// -----------------------------------------------------------------------------
//  Text implementation

StringRef::~StringRef ()
{
  if (mp_rep) {
    mp_rep->unregister_ref (this);
  }
}

}

namespace tl
{

template<> void extractor_impl (tl::Extractor &ex, db::Text &p)
{
  if (! test_extractor_impl (ex, p)) {
    ex.error (tl::to_string (tr ("Expected a text specification")));
  }
}

template<> void extractor_impl (tl::Extractor &ex, db::DText &p)
{
  if (! test_extractor_impl (ex, p)) {
    ex.error (tl::to_string (tr ("Expected a text specification")));
  }
}


template<class C> bool _test_extractor_impl (tl::Extractor &ex, db::text<C> &t)
{
  if (ex.test ("(")) {

    std::string s;
    ex.read_word_or_quoted (s);
    t.string (s);

    ex.expect (",");

    typename db::text<C>::trans_type tt;
    ex.read (tt);
    t.trans (tt);

    ex.expect (")");

    return true;

  } else {
    return false;
  }
}

template<> bool test_extractor_impl (tl::Extractor &ex, db::Text &p)
{
  return _test_extractor_impl (ex, p);
}

template<> bool test_extractor_impl (tl::Extractor &ex, db::DText &p)
{
  return _test_extractor_impl (ex, p);
}

}

