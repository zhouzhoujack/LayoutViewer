#ifndef HDR_layDrawing
#define HDR_layDrawing


#include <vector>
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"

#include "tlObject.h"
#include "tlObjectCollection.h"
#include "layViewOp.h"
#include "detail/geoTrans.h"

#include <QColor>

namespace lay
{

class CellView;
class Drawings;
class CanvasPlane;
class Renderer;
class RedrawThreadCanvas;

/**
 *  @brief The drawing interface
 *
 *  A "drawing" object implements functionality to draw objects
 *  from a cellview onto a set of planes. 
 *  The object can control the appearance of the planes.
 *  Basically, the object must implement these features:
 *
 *  1.) Allocate a number of planes in the constructor using 
 *  the num_planes parameter of the "Drawing" constructor.
 *
 *  2.) Implement a drawing function ("paint_cv_on_planes")
 *  that draws the given cellview onto the planes provided.
 *  This method is called from the drawing thread and must not
 *  make use of members of the object or protect them with a 
 *  mutex. It is also possible to implement "paint_on_planes" 
 *  to draw without a cellview.
 *
 *  3.) Control the appearance of the planes planes by implementing
 *  the get_view_ops method.
 */

class Drawing
  : virtual public geo::RefCounted
{
public:
  /**
   *  @brief The constructor
   *  
   *  See above for a explanation of the parent-child relationship.
   */
  Drawing (unsigned int num_planes, Drawings *drawings);

  /**
   *  @brief The destructor
   */
  virtual ~Drawing ();

  /**
   *  @brief Paint on the planes provided (called by the drawing thread)
   *
   *  @param cellview The cellview to paint
   *  @param trans The transformation to use when painting
   *  @param planes The planes to paint on. The number is the same than
   *                passed in the constructor.
   */
  virtual void paint_cv_on_planes (const lay::CellView & /*cellview*/, 
                                   const geo::CplxTrans & /*trans*/,
                                   const std::vector <lay::CanvasPlane *> & /*planes*/)
  {
    // .. nothing yet ..
  }

  /**
   *  @brief Paint annotation shapes on the planes provided (called by the drawing thread)
   *
   *  @param trans The transformation to use when painting
   *  @param planes The planes to paint on. The number is the same than
   *                 passed in the constructor.
   */
  virtual void paint_on_planes (const geo::DCplxTrans & /*trans*/,
                                const std::vector <lay::CanvasPlane *> & /*planes*/,
                                lay::Renderer & /*renderer*/)
  {
    // .. nothing yet ..
  }

  /**
   *  @brief Obtain the number of planes allocated
   */
  unsigned int num_planes () const
  {
    return m_num_planes;
  }

  /**
   *  @brief Get the current appearance
   */
  virtual std::vector <lay::ViewOp> get_view_ops (lay::RedrawThreadCanvas &canvas, QColor background, QColor foreground, QColor active) const = 0;

private:
  unsigned int m_num_planes;
};

/**
 *  @brief The collection of drawing objects
 */
class Drawings
  : public geo::weak_collection<lay::Drawing>
{
public:
  /**
   *  @brief The constructor
   */
  Drawings ()
    : geo::weak_collection<lay::Drawing> ()
  {
    // .. nothing yet ..
  }

  /**
   *  @brief The destructor
   */
  virtual ~Drawings ()
  {
    // .. nothing yet ..
  }

  /**
   *  @brief Update the display establishing the appearance
   */
  virtual void update_drawings () = 0;
};

}

#endif


