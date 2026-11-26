#include "layImageCacheEntry.h"
#include "detail/utilsString.h"

#include <algorithm>
#include <string>

#include "tlString.h"

namespace lay
{

// ----------------------------------------------------------------------------
//  free function

/**
 *  @brief Returns a value indicating whether the needed entry can make use of the one in the cache
 */
static bool applies (const lay::RedrawLayerInfo &in_cache, const lay::RedrawLayerInfo &needed) 
{
  if (needed.visible && !in_cache.visible) {
    return false;
  }

  if (needed.cell_frame != in_cache.cell_frame ||
      needed.xfill != in_cache.xfill ||
      needed.layer_index != in_cache.layer_index ||
      needed.cellview_index != in_cache.cellview_index ||
      needed.hier_levels != in_cache.hier_levels ||
      needed.prop_sel != in_cache.prop_sel ||
      needed.inverse_prop_sel != in_cache.inverse_prop_sel) {
    return false;
  }

  if (needed.trans.size () != in_cache.trans.size ()) {
    return false;
  }

  for (size_t i = 0; i < needed.trans.size (); ++i) {
    if (! needed.trans[i].equal (in_cache.trans[i])) {
      return false;
    }
  }

  return true;
}


// ----------------------------------------------------------------------------
//  ImageCacheEntry implementation

ImageCacheEntry::ImageCacheEntry (const lay::Viewport &vp, const std::vector<lay::RedrawLayerInfo> &layers, bool precious)
  : m_opened (true), m_trans (vp.trans ()), m_layers (layers), m_width (vp.width ()), m_height (vp.height ()), m_precious (precious)
{ 
  //  .. nothing yet .. 
}

bool ImageCacheEntry::equals (const lay::Viewport &vp, const std::vector<lay::RedrawLayerInfo> &layers) const
{
  if (!m_trans.equal (vp.trans ()) || m_width != vp.width () || m_height != vp.height ()) {
    return false;
  }

  if (m_layers.size () != layers.size ()) {
    return false;
  }

  for (size_t i = 0; i < m_layers.size (); ++i) {
    if (! applies (m_layers [i], layers [i])) {
      return false;
    }
  }

  return true;
}

void ImageCacheEntry::close (const BitmapCanvasData &data)
{
  m_data = data;
  m_opened = false;
}

void ImageCacheEntry::swap (ImageCacheEntry &other)
{
  std::swap (m_opened, other.m_opened);
  std::swap (m_layers, other.m_layers);
  std::swap (m_trans, other.m_trans);
  std::swap (m_width, other.m_width);
  std::swap (m_height, other.m_height);
  std::swap (m_precious, other.m_precious);
  m_data.swap (other.m_data);
}

std::string ImageCacheEntry::to_string () const
{
  return std::string (m_opened ? "(" : "") + std::string (m_precious ? "*" : " ") + 
         geo::to_string (m_width) + "x" + geo::to_string (m_height) + " " +
         m_trans.to_string () + std::string (m_opened ? ")" : "");
}

} // namespace lay
