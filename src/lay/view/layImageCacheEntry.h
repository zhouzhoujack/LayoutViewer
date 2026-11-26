#ifndef HDR_layImageCacheEntry
#define HDR_layImageCacheEntry

#include <string>
#include <vector>

#include "detail/geoTrans.h"
#include "layViewport.h"
#include "layRedrawLayerInfo.h"
#include "layRedrawThreadCanvas.h"

namespace lay
{

/**
 *  @brief A class representing one entry in the image cache
 */
class ImageCacheEntry
{
public:
  /**
   *  @brief Constructor
   *  On creation, the entry is in "open" state and will be closed once the data has been 
   *  committed with "close".
   *  "Precious" entries will be preserved as far as possible.
   */
  ImageCacheEntry (const lay::Viewport &vp, const std::vector<lay::RedrawLayerInfo> &layers, bool precious);

  /**
   *  @brief Returns a value indicating whether the entry complies with the given viewport
   */
  bool equals (const lay::Viewport &vp, const std::vector<lay::RedrawLayerInfo> &layers) const;

  /**
   *  @brief Closes the entry and stores the given data
   */
  void close (const BitmapCanvasData &data);

  /**
   *  @brief Returns a value indicating whether the entry is opened
   */
  bool opened () const
  {
    return m_opened;
  }

  /**
   *  @brief Returns a value indicating whether the entry is precious
   */
  bool precious () const
  {
    return m_precious;
  }

  /**
   *  @brief Sets a value indicating whether the entry is precious
   */
  void set_precious (bool p)
  {
    m_precious = p;
  }

  /**
   *  @brief Gets the stored bitmap data
   */
  const BitmapCanvasData &data () const
  {
    return m_data;
  }

  /**
   *  @brief Swaps this entry with another one
   */
  void swap (ImageCacheEntry &other);

  /**
   *  @brief Returns a string representing this entry
   */
  std::string to_string () const;

private:
  bool m_opened;
  geo::DCplxTrans m_trans;
  std::vector<lay::RedrawLayerInfo> m_layers;
  unsigned int m_width, m_height;
  bool m_precious;
  BitmapCanvasData m_data;
};

} // namespace lay

#endif
