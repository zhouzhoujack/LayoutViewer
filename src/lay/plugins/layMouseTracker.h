#ifndef HDR_layMouseTracker
#define HDR_layMouseTracker

#include "layViewObject.h"
#include "layPlugin.h"
#include "layViewService.h"

class QMouseEvent;

namespace lay {

class LayoutCanvas;
class LayoutView;

class MouseTracker
  : public lay::ViewService,
    public lay::Plugin
{
public: 
  MouseTracker (lay::LayoutView *view);
  virtual bool mouse_move_event (const geo::DPoint &p, unsigned int buttons, bool prio);

private:
  lay::LayoutView *mp_view;
};

}

#endif

