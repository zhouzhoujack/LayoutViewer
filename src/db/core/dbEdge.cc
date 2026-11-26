#include "dbEdge.h"
#include "tlLongInt.h"

namespace db
{
// -----------------------------------------------------------------------------
//  Edge implementation


/**
 *  @brief Computes the gcd of two numbers
 */
template <class C>
inline C gcd (C a, C b)
{
  while (b != 0) {
    a %= b;
    std::swap (a, b);
  }
  return a;
}

// #if defined(__SIZEOF_INT128__)
// typedef __int128 a2_type;
// #else
// //  fallback to long_int in case the __int128 isn't defined
typedef tl::long_int<4, uint32_t, uint64_t> a2_type;
// #endif

db::Coord div_exact (db::Coord a, db::coord_traits<db::Coord>::area_type b, db::coord_traits<db::Coord>::area_type d)
{
  if (a < 0) {
    return -db::Coord ((a2_type (-a) * a2_type (b) + a2_type (d / 2)) / a2_type (d));
  } else {
    return db::Coord ((a2_type (a) * a2_type (b) + a2_type ((d - 1) / 2)) / a2_type (d));
  }
}

}

namespace tl
{

template<> void extractor_impl (tl::Extractor &ex, db::Edge &e)
{
  if (! test_extractor_impl (ex, e)) {
    ex.error (tl::to_string (tr ("Expected an edge specification")));
  }
}

template<> void extractor_impl (tl::Extractor &ex, db::DEdge &e)
{
  if (! test_extractor_impl (ex, e)) {
    ex.error (tl::to_string (tr ("Expected an edge specification")));
  }
}

template<class C> bool _test_extractor_impl (tl::Extractor &ex, db::edge<C> &e)
{
  typedef db::point<C> point_type;

  if (ex.test ("(")) {

    point_type p1, p2;
    ex.read (p1);
    ex.expect (";");
    ex.read (p2);

    e = db::edge<C> (p1, p2);

    ex.expect (")");

    return true;

  } else {
    return false;
  }
}

template<> bool test_extractor_impl (tl::Extractor &ex, db::Edge &e)
{
  return _test_extractor_impl (ex, e);
}

template<> bool test_extractor_impl (tl::Extractor &ex, db::DEdge &e)
{
  return _test_extractor_impl (ex, e);
}

} // namespace tl

