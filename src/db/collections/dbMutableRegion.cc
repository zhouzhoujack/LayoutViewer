#include "dbMutableRegion.h"

namespace db
{

// -------------------------------------------------------------------------------------------------------------
//  FlatRegion implementation

MutableRegion::MutableRegion ()
  : AsIfFlatRegion ()
{
  //  .. nothing yet ..
}

MutableRegion::MutableRegion (const MutableRegion &other)
  : AsIfFlatRegion (other)
{
  //  .. nothing yet ..
}

MutableRegion::~MutableRegion ()
{
  //  .. nothing yet ..
}

void
MutableRegion::insert (const db::Box &box)
{
  if (! box.empty () && box.width () > 0 && box.height () > 0) {
    do_insert (db::Polygon (box));
  }
}

void
MutableRegion::insert (const db::Path &path)
{
  if (path.points () > 0) {
    do_insert (path.polygon ());
  }
}

void
MutableRegion::insert (const db::SimplePolygon &polygon)
{
  if (polygon.vertices () > 0) {
    db::Polygon poly;
    poly.assign_hull (polygon.begin_hull (), polygon.end_hull ());
    do_insert (poly);
  }
}

void
MutableRegion::insert (const db::Shape &shape)
{
  if (shape.is_polygon () || shape.is_path () || shape.is_box ()) {
    db::Polygon poly;
    shape.polygon (poly);
    insert (poly);
  } else if (shape.is_path ()) {
    insert (shape.path ());
  } else if (shape.is_box ()) {
    insert (shape.box ());
  }
}

}

