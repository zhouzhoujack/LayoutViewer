#ifndef HDR_dbMutableEdgePairs
#define HDR_dbMutableEdgePairs

#include "dbCommon.h"

#include "dbAsIfFlatEdgePairs.h"

#include <set>

namespace db {

/**
 *  @brief An interface representing mutable edge pair collections
 *
 *  Mutable edge pair collections offer insert, transform, flatten and other manipulation functions.
 */
class DB_PUBLIC MutableEdgePairs
  : public AsIfFlatEdgePairs
{
public:
  MutableEdgePairs ();
  MutableEdgePairs (const MutableEdgePairs &other);
  virtual ~MutableEdgePairs ();

  virtual void do_insert (const db::EdgePair &edge_pair) = 0;

  virtual void do_transform (const db::Trans &t) = 0;
  virtual void do_transform (const db::ICplxTrans &t) = 0;
  virtual void do_transform (const db::IMatrix2d &t) = 0;
  virtual void do_transform (const db::IMatrix3d &t) = 0;

  virtual void flatten () = 0;

  virtual void reserve (size_t n) = 0;

  void transform (const db::UnitTrans &) { }
  void transform (const db::Disp &t) { do_transform (db::Trans (t)); }
  void transform (const db::Trans &t) { do_transform (t); }
  void transform (const db::ICplxTrans &t) { do_transform (t); }
  void transform (const db::IMatrix2d &t) { do_transform (t); }
  void transform (const db::IMatrix3d &t) { do_transform (t); }

  void insert (const db::EdgePair &edge_pair) { do_insert (edge_pair); }
  void insert (const db::Shape &shape);

  template <class T>
  void insert (const db::Shape &shape, const T &trans)
  {
    if (shape.is_edge_pair ()) {
      db::EdgePair ep = shape.edge_pair ();
      ep.transform (trans);
      insert (ep);
    }
  }

  template <class Iter>
  void insert (const Iter &b, const Iter &e)
  {
    reserve (count () + (e - b));
    for (Iter i = b; i != e; ++i) {
      insert (*i);
    }
  }

  template <class Iter>
  void insert_seq (const Iter &seq)
  {
    for (Iter i = seq; ! i.at_end (); ++i) {
      insert (*i);
    }
  }
};

}

#endif

