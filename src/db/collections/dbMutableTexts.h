#ifndef HDR_dbMutableTexts
#define HDR_dbMutableTexts

#include "dbCommon.h"

#include "dbAsIfFlatTexts.h"

#include <set>

namespace db {

/**
 *  @brief An interface representing mutable text collections 
 *
 *  Mutable text collections offer insert, transform, flatten and other manipulation functions.
 */
class DB_PUBLIC MutableTexts
  : public AsIfFlatTexts
{
public:
  MutableTexts ();
  MutableTexts (const MutableTexts &other);
  virtual ~MutableTexts ();

  virtual void do_insert (const db::Text &text) = 0;

  virtual void do_transform (const db::Trans &t) = 0;
  virtual void do_transform (const db::ICplxTrans &t) = 0;
  virtual void do_transform (const db::IMatrix3d &t) = 0;
  virtual void do_transform (const db::IMatrix2d &t) = 0;

  virtual void flatten () = 0;

  virtual void reserve (size_t n) = 0;

  void transform (const db::UnitTrans &) { }
  void transform (const db::Disp &t) { do_transform (db::Trans (t)); }
  void transform (const db::Trans &t) { do_transform (t); }
  void transform (const db::ICplxTrans &t) { do_transform (t); }
  void transform (const db::IMatrix3d &t) { do_transform (t); }
  void transform (const db::IMatrix2d &t) { do_transform (t); }

  void insert (const db::Text &text) { do_insert (text); }
  void insert (const db::Shape &shape);

  template <class T>
  void insert (const db::Shape &shape, const T &trans)
  {
    if (shape.is_text ()) {
      db::Text text = shape.text ();
      text.transform (trans);
      insert (text);
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

