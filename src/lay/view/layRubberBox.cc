#include "layRubberBox.h"

#include "layViewport.h"
#include "layRenderer.h"

namespace lay
{

// -------------------------------------------------------------
//  RubberBox implementation

RubberBox::RubberBox (lay::ViewObjectWidget *widget, unsigned int color, const geo::DPoint &p1, const geo::DPoint &p2)
  : lay::ViewObject (widget, false /*not static*/), 
    m_p1 (p1), m_p2 (p2), m_color (color), m_stipple (0)
{ }

void 
RubberBox::set_color (unsigned int color)
{
  if (m_color != color) {
    m_color = color;
    redraw ();
  }
}

void 
RubberBox::set_stipple (unsigned int s)
{
  if (m_stipple != s) {
    m_stipple = s;
    redraw ();
  }
}

void 
RubberBox::render (const Viewport &vp, ViewObjectCanvas &canvas)
{ 
  lay::Renderer &r = canvas.renderer ();
  int lw = int (0.5 + 1.0 / r.resolution ());
  lay::CanvasPlane *plane = canvas.plane (lay::ViewOp (m_color, lay::ViewOp::Copy, 0, m_stipple, 0, lay::ViewOp::Rect, lw));
  if (plane) {
    r.draw (vp.trans () * geo::DBox (m_p1, m_p2), 0, plane, 0, 0);
  }
}

void
RubberBox::set_points (const geo::DPoint &p1, const geo::DPoint &p2)
{ 
  if (m_p1 != p1 || m_p2 != p2) {
    m_p1 = p1;
    m_p2 = p2;
    redraw ();
  }
}

}

