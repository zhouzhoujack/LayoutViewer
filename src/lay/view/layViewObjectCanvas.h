#ifndef HDR_layViewObjectCanvas
#define HDR_layViewObjectCanvas

#include <cstddef>
#include <map>
#include <vector>

#include <QColor>

#include "layViewOp.h"
#include "layBitmapRenderer.h"

class QImage;

namespace lay
{

/**
 *  @brief The canvas interface
 *
 *  The canvas provides ways and attributes that allow the view objects to render
 *  themselves. The basic functionality of this interface is to provide planes
 *  for painting. 
 */
class ViewObjectCanvas
{
public:
  /**
   *  The destructor 
   */
  virtual ~ViewObjectCanvas () 
  { }

  /**
   *  @brief Background color property: background color of the canvas
   */
  virtual QColor background_color () const = 0;

  /**
   *  @brief Foreground color property: foreground color of the canvas (some "contrast" color to background)
   */
  virtual QColor foreground_color () const = 0;

  /**
   *  @brief Active color property: color of active elements on the canvas (some "contrast" color to background and different from foreground)
   */
  virtual QColor active_color () const = 0;

  /**
   *  @brief Get the resolution
   *
   *  The resolution describes the size of one pixel in relation to "one line width". "One line width" is the unit
   *  of the drawing and describes the width of a "one unit" wide line. In that sense, the resolution is the reciprocal 
   *  of a "one unit" line's width in pixels.
   */
  virtual double resolution () const = 0;

  /**
   *  @brief CanvasPlane provider
   *
   *  This method is used by the view objects to obtain the planes
   *  they should paint themselves on. 
   *  A style can be specified that determines how the plane will
   *  be displayed. CanvasPlanes may be shared if the style is identical.
   *  The plane index of the ViewOp is not used. 
   */
  virtual lay::CanvasPlane *plane (const lay::ViewOp &style) = 0;

  /**
   *  @brief CanvasPlane provider for a sequence of operations on the same plane
   *
   *  This method is used by the view objects to obtain the planes
   *  they should paint themselves on. 
   *  A operator vector can be specified that determines how the plane will
   *  be displayed. The operators are executed in the order given on the same plane.
   *  CanvasPlanes may be shared if the styles are identical.
   *  The plane index of the ViewOp is not used. 
   */
  virtual lay::CanvasPlane *plane (const std::vector<lay::ViewOp> &style) = 0;

  /**
   *  @brief Provide the renderer
   */
  virtual lay::Renderer &renderer () = 0;

  /**
   *  @brief Sort the planes in the painting order
   */
  virtual void sort_planes () = 0;
};

/**
 *  @brief The canvas interface
 *
 *  The canvas provides ways and attributes that allow the view objects to render
 *  themselves. The basic functionality of this interface is to provide planes
 *  for painting. 
 */
class BitmapViewObjectCanvas
  : public ViewObjectCanvas
{
public:
  /**
   *  @brief Constructor
   */
  BitmapViewObjectCanvas ();

  /**
   *  @brief Constructor
   */
  BitmapViewObjectCanvas (unsigned int width, unsigned int height, double resolution);

  /**
   *  @brief The destructor 
   */
  virtual ~BitmapViewObjectCanvas ();

  /**
   *  @brief CanvasPlane provider
   *
   *  This method is used by the view objects to obtain the planes
   *  they should paint themselves on. 
   *  A style can be specified that determines how the plane will
   *  be displayed. CanvasPlanes may be shared if the style is identical.
   *  The plane index of the ViewOp is not used. 
   */
  virtual lay::CanvasPlane *plane (const lay::ViewOp &style);

  /**
   *  @brief CanvasPlane provider for a sequence of operations on the same plane
   *
   *  This method is used by the view objects to obtain the planes
   *  they should paint themselves on. 
   *  A operator vector can be specified that determines how the plane will
   *  be displayed. The operators are executed in the order given on the same plane.
   *  CanvasPlanes may be shared if the styles are identical.
   *  The plane index of the ViewOp is not used. 
   */
  virtual lay::CanvasPlane *plane (const std::vector<lay::ViewOp> &style);

  /**
   *  @brief Sort the planes in the painting order
   */
  virtual void sort_planes ();

  /**
   *  @brief Provide the renderer
   */
  virtual lay::Renderer &renderer () 
  { 
    return m_renderer; 
  }

  /**
   *  @brief Get the resolution
   */
  virtual double resolution () const 
  {
    return m_resolution;
  }

  /**
   *  @brief Return the number of bitmaps stored so far
   */
  size_t fg_bitmaps () const
  {
    return mp_fg_bitmaps.size ();
  }

  /**
   *  @brief Return the foreground bitmap with the given index
   *
   *  The returned pointer may be 0. The index must be less than the one returned
   *  by fg_bitmaps ().
   */
  const lay::Bitmap *fg_bitmap (unsigned int i) const
  {
    return mp_fg_bitmaps [i];
  }

  /**
   *  @brief Direct access to the bitmap pointers
   */

  const std::vector <lay::Bitmap *> &fg_bitmap_vector () const
  {
    return mp_fg_bitmaps;
  }

  /**
   *  @brief Return the foreground plane style with the given index
   *
   *  The index must be less than the one returned by fg_planes ().
   */
  const lay::ViewOp &fg_style (unsigned int i) const
  {
    return m_fg_view_ops [i];
  }

  /**
   *  @brief Direct access to the styles vector
   */
  const std::vector <lay::ViewOp> &fg_view_op_vector () const
  {
    return m_fg_view_ops;
  }

  /**
   *  @brief Clear the foreground bitmaps and all associated information
   */
  void clear_fg_bitmaps ();

  /**
   *  @brief Return the background image
   */
  virtual QImage &bg_image () = 0;

  /**
   *  @brief Set the width and height and resolution
   */
  void set_size (unsigned int width, unsigned int height, double resolution);

  /**
   *  @brief Set the width and height
   */
  void set_size (unsigned int width, unsigned int height);

  /**
   *  @brief Set the resolution
   */
  void set_size (double resolution);

  /**
   *  @brief Get the width
   */
  unsigned int canvas_width () const 
  {
    return m_width;
  }

  /**
   *  @brief Get the height
   */
  unsigned int canvas_height () const 
  {
    return m_height;
  }

private:
  std::map <lay::ViewOp, unsigned int> m_fg_bitmap_table;
  std::map <std::vector <lay::ViewOp>, unsigned int> m_fgv_bitmap_table;
  std::vector <lay::Bitmap *> mp_fg_bitmaps;
  std::vector <lay::Bitmap *> mp_alloc_bitmaps;
  std::vector <lay::ViewOp> m_fg_view_ops;
  lay::BitmapRenderer m_renderer;
  unsigned int m_width, m_height;
  double m_resolution;
};

} // namespace lay

#endif
