#ifndef HDR_layRubberBox
#define HDR_layRubberBox

#include "layViewObject.h"

namespace lay
{

class RubberBox
  : public lay::ViewObject
{
public: 
  RubberBox (lay::ViewObjectWidget *canvas, unsigned int color, const geo::DPoint &p1, const geo::DPoint &p2);

  void set_color (unsigned int color);
  void set_stipple (unsigned int s);
  void set_points (const geo::DPoint &begin, const geo::DPoint &end);

private:
  virtual void render (const Viewport &vp, ViewObjectCanvas &canvas);

  geo::DPoint m_p1, m_p2;
  unsigned int m_color;
  unsigned int m_stipple;
};

}

#endif


