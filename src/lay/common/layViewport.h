#ifndef HDR_layViewport
#define HDR_layViewport


#include "detail/geoTrans.h"
#include "geoBox.h"

namespace lay
{

/**
 *  @brief A "viewport" descriptor
 *
 *  A "viewport" is basically a rectangular region, given both in pixels
 *  and in micron. The basic specification consists of a transformation and
 *  a pixel width and height. This implies an identical aspect ratio of both
 *  pixel and micron dimensions.
 *  The transformation converts a micron point into a pixel point. The pixel
 *  coordinate in the mathematical sense, i.e. the lowest y value is at the bottom.
 *  Internally, there exist two boxes: the actual viewport box which has the
 *  same aspect ratio than the pixel box, and the target box which is the one
 *  that is set on set_box and the constructor. The target box is kept to allow
 *  a recomputation of the transformation if the size changes.
 */

class Viewport
{
public:
  /**
   *  @brief The default constructor providing an empty viewport
   */
  Viewport ();

  /**
   *  @brief The standard constructor
   *
   *  For a description of the parameters set set_size for width and height and
   *  set_box for target_box.
   */
  Viewport (unsigned int width, unsigned int height, const geo::DBox &target_box);

  /**
   *  @brief Set width and height
   */
  void set_size (unsigned int w, unsigned int h);

  /**
   *  @brief Specify a given micron box and determine the transformation accordingly.
   *
   *  Since the aspect ratio of micron and pixel box must be identical, the resulting
   *  micron box may not be identical to the given one. It is guaranteed however that
   *  the given box is contained in the resulting box. Internally however, the target
   *  box passed to this method is kept to allow a recomputation of the transformation
   *  if the size changes.
   *  If the current transformation includes rotation and mirror components, these are
   *  preserved.
   */
  void set_box (const geo::DBox &target_box);

  /**
   *  @brief Specify a given transformation directly
   */
  void set_trans (const geo::DCplxTrans &trans);

  /**
   *  @brief Specify the global transformation 
   */
  void set_global_trans (const geo::DCplxTrans &trans);

  /**
   *  @brief Get the current width
   */
  unsigned int width () const
  {
    return m_width;
  }

  /**
   *  @brief Get the current height
   */
  unsigned int height () const
  {
    return m_height;
  }

  /**
   *  @brief Get the current transformation from micron into pixel space
   */
  const geo::DCplxTrans &trans () const
  {
    return m_trans;
  }

  /**
   *  @brief Get the global transformation
   */
  const geo::DCplxTrans &global_trans () const
  {
    return m_global_trans;
  }

  /**
   *  @brief Get the current micron box
   */
  geo::DBox box () const;

  /**
   *  @brief Get the target micron box
   *
   *  The target box may have a different aspect ratio than the viewport box and is 
   *  not directly reflected by the transformation.
   */
  geo::DBox target_box () const
  {
    return m_target_box;
  }

private:
  unsigned int m_width, m_height;
  geo::DCplxTrans m_trans;
  geo::DBox m_target_box;
  geo::DCplxTrans m_global_trans;
};

}

#endif



