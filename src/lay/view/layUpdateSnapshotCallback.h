#ifndef HDR_layUpdateSnapshotCallback
#define HDR_layUpdateSnapshotCallback

#include <cstddef>

#include "detail/geoTrans.h"
#include "layBitmap.h"

namespace lay
{

/**
 *  @brief A callback class which is triggered when a snapshot is taken
 */
class UpdateSnapshotCallback
{
public:
  UpdateSnapshotCallback () { }
  virtual ~UpdateSnapshotCallback () { }
  virtual void trigger () const { };
};

/**
 *  @brief An value in the drawing cache
 */
struct CellCacheInfo
{
public:
  CellCacheInfo ()
    : hits (0), fill (0), frame (0), vertex (0), text (0)
  { }

  ~CellCacheInfo ()
  {
    delete fill;
    fill = 0;
    delete frame;
    frame = 0;
    delete vertex;
    vertex = 0;
    delete text;
    text = 0;
  }

  size_t hits;
  geo::DPoint offset;
  lay::Bitmap *fill, *frame, *vertex, *text;
};

/**
 *  @brief A helper to merge a bitmap into a target bitmap at an offset
 */
inline void copy_bitmap (const lay::Bitmap *from, lay::Bitmap *to, int dx, int dy)
{
  if (to) {
    to->merge (from, dx, dy);
  }
}

/**
 *  @brief An UpdateSnapshotCallback which copies the cached cell's bitmaps
 *  onto the drawing planes when a snapshot is taken
 */
class UpdateSnapshotWithCache
  : public UpdateSnapshotCallback
{
public:
  UpdateSnapshotWithCache (const UpdateSnapshotCallback *parent, const geo::CplxTrans *trans, const CellCacheInfo *info, lay::CanvasPlane *fill, lay::CanvasPlane *frame, lay::CanvasPlane *vertex, lay::CanvasPlane *text)
    : mp_parent (parent), mp_trans (trans), mp_info (info), mp_fill (fill), mp_frame (frame), mp_vertex (vertex), mp_text (text)
  {
    //  .. nothing yet ..
  }

  void trigger () const
  {
    if (mp_parent) {
      mp_parent->trigger ();
    }

    geo::Point t = geo::Point (mp_info->offset + mp_trans->disp ());

    copy_bitmap (mp_info->fill,   dynamic_cast<lay::Bitmap *> (mp_fill),   t.x (), t.y ());
    copy_bitmap (mp_info->frame,  dynamic_cast<lay::Bitmap *> (mp_frame),  t.x (), t.y ());
    copy_bitmap (mp_info->vertex, dynamic_cast<lay::Bitmap *> (mp_vertex), t.x (), t.y ());
    copy_bitmap (mp_info->text,   dynamic_cast<lay::Bitmap *> (mp_text),   t.x (), t.y ());
  }

private:
  const UpdateSnapshotCallback *mp_parent;
  const geo::CplxTrans *mp_trans;
  const CellCacheInfo *mp_info;
  lay::CanvasPlane *mp_fill;
  lay::CanvasPlane *mp_frame;
  lay::CanvasPlane *mp_vertex;
  lay::CanvasPlane *mp_text;
};

} // namespace lay

#endif
