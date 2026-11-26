#include "dbCommon.h"

#include "dbBoxConvert.h"
#include "dbCell.h"

namespace db
{
// -----------------------------------------------------------------------------
//  BoxConvert implementation


DB_PUBLIC db::Box cellinst_box_convert_impl (const db::CellInst &inst, const db::Layout *layout, int layer, bool allow_empty)
{
  if (layer >= 0) {
    return inst.bbox (*layout, layer);
  } else if (allow_empty) {
    return inst.bbox (*layout);
  } else {
    db::Box box = inst.bbox (*layout);
    if (box.empty ()) {
      return db::Box (db::Point (0, 0), db::Point (0, 0));
    } else {
      return box;
    }
  }
}

DB_PUBLIC db::Box cell_box_convert_impl (const db::Cell &c, int layer, bool allow_empty)
{
  if (layer >= 0) {
    return c.bbox (layer);
  } else if (allow_empty) {
    return c.bbox ();
  } else {
    if (c.bbox ().empty ()) {
      return db::Box (db::Point (0, 0), db::Point (0, 0));
    } else {
      return c.bbox ();
    }
  }
}

template struct box_convert <db::CellInst, true>;
template struct box_convert <db::CellInst, false>;
template struct box_convert <db::Cell, true>;
template struct box_convert <db::Cell, false>;

}

