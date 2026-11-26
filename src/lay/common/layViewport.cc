#include "layViewport.h"

#include <algorithm>

namespace lay
{

// -----------------------------------------------------------------------------
//  Viewport implementation

Viewport::Viewport ()
  : m_width (0), m_height (0)
{
  //  .. nothing yet .. 
}

Viewport::Viewport (unsigned int width, unsigned int height, const geo::DBox &target_box)
  : m_width (width), m_height (height)
{
  set_box (target_box);
}

void 
Viewport::set_size (unsigned int w, unsigned int h)
{
  m_width = w;
  m_height = h;
  set_box (m_target_box);
}

void 
Viewport::set_global_trans (const geo::DCplxTrans &trans)
{
  if (! trans.equal (m_global_trans)) {
    geo::DBox b = box ();
    m_global_trans = trans;
    set_box (b);
  }
}

void 
Viewport::set_trans (const geo::DCplxTrans &trans)
{
  m_trans = trans;
  m_target_box = trans * geo::DBox (0.0, 0.0, double (width ()), double (height ()));
}

void 
Viewport::set_box (const geo::DBox &in_box)
{
  m_target_box = in_box;

  geo::DBox box = m_global_trans * in_box;

  //  use double arithmetics to avoid overflows
  double w = box.right () - box.left ();
  double h = box.top () - box.bottom ();
  double fx = w / double (std::max (width (), (unsigned int) 1));
  double fy = h / double (std::max (height (), (unsigned int) 1));
  double f = std::max (fx, fy);

  //  as a safety measure we treat the zero factor case somewhat more graceful
  if (f < 1e-13) {
    f = 0.001; // default mag. factor
  }

  double mx = double (box.right ()) + double (box.left ());
  double my = double (box.top ()) + double (box.bottom ());

  //  use only integer shift vectors. That enables a partial update of the image.
  double dx = floor (0.5 + (mx / f - double (width ())) * 0.5);
  double dy = floor (0.5 + (my / f - double (height ())) * 0.5);

  //  preserve the angle and mirror properties of the transformation
  m_trans = geo::DCplxTrans (1.0 / f /*mag*/, 0.0 /*angle*/, false, geo::DVector (-dx, -dy)) * m_global_trans;
}

geo::DBox 
Viewport::box () const
{
  geo::DPoint p1 = trans ().inverted () * geo::DPoint (0, 0);
  geo::DPoint p2 = trans ().inverted () * geo::DPoint (width (), height ());
  return geo::DBox (p1, p2);
}

}

