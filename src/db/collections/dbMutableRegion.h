#ifndef HDR_dbMutableRegion
#define HDR_dbMutableRegion

#include "dbCommon.h"

#include "dbAsIfFlatRegion.h"

#include <set>

namespace db {

/**
 *  @brief An interface representing mutable regions
 *
 *  Mutable regions offer insert, transform, flatten and other manipulation functions.
 */
class DB_PUBLIC MutableRegion
  : public AsIfFlatRegion
{
public:
  MutableRegion ();
  MutableRegion (const MutableRegion &other);
  virtual ~MutableRegion ();

  virtual void do_insert (const db::Polygon &polygon) = 0;

  void transform (const db::UnitTrans &) { }
  void transform (const db::Disp &t) { do_transform (db::Trans (t)); }
  void transform (const db::Trans &t) { do_transform (t); }
  void transform (const db::ICplxTrans &t) { do_transform (t); }
  void transform (const db::IMatrix2d &t) { do_transform (t); }
  void transform (const db::IMatrix3d &t) { do_transform (t); }

  virtual void do_transform (const db::Trans &t) = 0;
  virtual void do_transform (const db::ICplxTrans &t) = 0;
  virtual void do_transform (const db::IMatrix2d &t) = 0;
  virtual void do_transform (const db::IMatrix3d &t) = 0;

  virtual void flatten () = 0;

  virtual void reserve (size_t n) = 0;

  void insert (const db::Polygon &polygon) { do_insert (polygon); }
  void insert (const db::Box &box);
  void insert (const db::Path &path);
  void insert (const db::SimplePolygon &polygon);

  void insert (const db::Shape &shape);

  template <class T>
  void insert (const db::Shape &shape, const T &trans)
  {
    if (shape.is_polygon () || shape.is_path () || shape.is_box ()) {
      db::Polygon poly;
      shape.polygon (poly);
      poly.transform (trans);
      insert (poly);
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

