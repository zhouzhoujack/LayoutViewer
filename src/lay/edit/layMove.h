#ifndef HDR_layMove
#define HDR_layMove

#include "detail/geoManager.h"
#include "layViewObject.h"
#include "layViewService.h"

#include <QTimer>
#include <QObject>

#include <memory>

namespace lay {

class Editables;
class LayoutView;

class MoveService
  : public QObject,
    public lay::ViewService
{
Q_OBJECT 

public: 
  MoveService (lay::LayoutView *view);
  ~MoveService ();

  virtual bool configure (const std::string &name, const std::string &value);
  bool begin_move (geo::Transaction *transaction = 0, bool selected_after_move = true);

private:
  virtual bool mouse_press_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool mouse_click_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool mouse_move_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool mouse_double_click_event (const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool mouse_release_event (const geo::DPoint &p, unsigned int /*buttons*/, bool prio);
  virtual bool wheel_event (int delta, bool horizontal, const geo::DPoint &p, unsigned int buttons, bool prio);
  virtual bool key_event (unsigned int key, unsigned int buttons);
  virtual void drag_cancel ();
  virtual void deactivated ();

  bool handle_click (const geo::DPoint &p, unsigned int buttons, bool drag_transient, geo::Transaction *transaction);

  bool m_dragging;
  bool m_dragging_transient;
  lay::Editables *mp_editables;
  lay::LayoutView *mp_view;
  double m_global_grid;
  geo::DPoint m_shift;
  geo::DPoint m_mouse_pos;
  std::unique_ptr<geo::Transaction> mp_transaction;
};

}

#endif

