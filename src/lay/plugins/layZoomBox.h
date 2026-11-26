#ifndef HDR_layZoomBox
#define HDR_layZoomBox

#include "layViewObject.h"
#include "layViewService.h"
#include "layPlugin.h"

namespace lay
{

class LayoutView;
class LayoutCanvas;
class RubberBox;

class ZoomService
  : public lay::ViewService,
    public lay::Plugin
{
public: 
  ZoomService (lay::LayoutView *view);
  ~ZoomService ();

  void set_colors (QColor background, QColor text);
  void begin (const geo::DPoint &pos);
  void begin_pan (const geo::DPoint &pos);

private:
  virtual bool mouse_move_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool mouse_release_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool mouse_press_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool mouse_click_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool wheel_event (int delta, bool horizontal, const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual void drag_cancel ();

  geo::DPoint m_p1, m_p2;
  geo::DBox m_vp;
  lay::LayoutView *mp_view;
  lay::RubberBox *mp_box;
  unsigned int m_color;
};

}

#endif


