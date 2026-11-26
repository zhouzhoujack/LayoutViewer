#ifndef HDR_layCanvasPlane
#define HDR_layCanvasPlane

namespace lay {

const double render_epsilon = 1e-6;

/**
 *  @brief A generic canvas plane
 *
 *  This is the base class for lay::Bitmap for example.
 *  It is used by the renderer to identify the rendering target.
 */
class CanvasPlane 
{
public:
  /**
   *  @brief Destructor
   */
  virtual ~CanvasPlane ();

  /**
   *  @brief Clear the plane
   */
  virtual void clear () = 0;

  /**
   *  @brief Fill method 
   *
   *  This method is supposed to draw a small rectangle of 1 pixel
   *
   *  @param x,y The coordinate where to put the pixel (lower left corner)
   */
  virtual void pixel (unsigned int y, unsigned int x) = 0;
};

} // namespace lay

#endif


