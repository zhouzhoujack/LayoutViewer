#include "layMove.h"
#include "detail/utilsString.h"
#include "layEditable.h"
#include "layLayoutView.h"
#include "laySelectPlugin.h"
#include "laybasicConfig.h"

namespace lay
{

// -------------------------------------------------------------
//  MoveService implementation

MoveService::MoveService (lay::LayoutView *view)
  : QObject (),
    lay::ViewService (view->view_object_widget ()), 
    m_dragging (false),
    m_dragging_transient (false),
    mp_editables (view),
    mp_view (view),
    m_global_grid (0.001)
{ 
  //  .. nothing yet ..
}

MoveService::~MoveService ()
{
  drag_cancel ();
}

void  
MoveService::deactivated ()
{
  m_shift = geo::DPoint ();
  mp_view->clear_transient_selection ();
  drag_cancel ();
}

lay::angle_constraint_type 
ac_from_buttons (unsigned int buttons)
{
  if ((buttons & lay::ShiftButton) != 0) {
    if ((buttons & lay::ControlButton) != 0) {
      return lay::AC_Any;
    } else {
      return lay::AC_Ortho;
    }
  } else {
    if ((buttons & lay::ControlButton) != 0) {
      return lay::AC_Diagonal;
    } else {
      return lay::AC_Global;
    }
  }
}

bool
MoveService::configure (const std::string &name, const std::string &value)
{
  if (name == cfg_grid) {
    geo::from_string (value, m_global_grid);
  }
  return false;  //  not taken
}

bool
MoveService::key_event (unsigned int key, unsigned int /*buttons*/)
{
  double dx = 0.0, dy = 0.0;
  if (int (key) == Qt::Key_Down) {
    dy = -1.0;
  } else if (int (key) == Qt::Key_Up) {
    dy = 1.0;
  } else if (int (key) == Qt::Key_Left) {
    dx = -1.0;
  } else if (int (key) == Qt::Key_Right) {
    dx = 1.0;
  }

  if (! m_dragging && fabs (dx + dy) > 0.0 && mp_editables->has_selection ()) {

    //  determine a shift distance which is 2, 5 or 10 times the grid and is more than 5 pixels
    double dmin = double (5 /*pixels min shift*/) / widget ()->mouse_event_trans ().mag ();
    double d = m_global_grid;
    while (d < dmin) {
      d *= 2.0;
      if (d < dmin) {
        d *= 2.5;
        if (d < dmin) {
          d *= 2.0;
        }
      } 
    }

    geo::DVector s = geo::DVector (dx * d, dy * d);
    m_shift += s;

    std::string pos = std::string ("dx: ") + geo::micron_to_string (m_shift.x ()) + "  dy: " + geo::micron_to_string (m_shift.y ());
    mp_view->message (pos);

    mp_editables->transform (geo::DCplxTrans (s));

    return true;

  } else {
    return false;
  }
}

bool 
MoveService::mouse_move_event (const geo::DPoint &p, unsigned int buttons, bool prio) 
{
  bool ret = false;

  if (m_dragging) {

    set_cursor (lay::Cursor::size_all);
    mp_editables->move (p, ac_from_buttons (buttons));

  } else if (prio) {

    lay::SelectionService *selector = mp_view->selection_service ();
    if (selector) {
      ret = selector->mouse_move_event (p, buttons, prio);
    }

  }

  //  track mouse position for the infix move initiation
  m_mouse_pos = p;

  return ret;  // not taken to allow the mouse tracker to receive events as well
}

bool 
MoveService::mouse_click_event (const geo::DPoint &p, unsigned int buttons, bool prio)
{
  if (prio && (buttons & lay::RightButton) != 0 && m_dragging) {
    if ((buttons & lay::ShiftButton) != 0) {
      mp_editables->move_transform (p, geo::DFTrans (geo::DFTrans::m90), ac_from_buttons (buttons));
    } else {
      mp_editables->move_transform (p, geo::DFTrans (geo::DFTrans::r90), ac_from_buttons (buttons));
    }
    return true;
  } 
  if (prio && (buttons & lay::LeftButton) != 0) {
    if (handle_click (p, buttons, false, 0)) {
      return true;
    }
  } 
  if (prio) {
    lay::SelectionService *selector = mp_view->selection_service ();
    if (selector) {
      return selector->mouse_click_event (p, buttons, prio);
    }
  }
  return false;
}

bool 
MoveService::mouse_double_click_event (const geo::DPoint &p, unsigned int buttons, bool prio)
{
  if (prio) {

    //  stop dragging if required
    if (m_dragging) {
      handle_click (p, buttons, false, 0);
    }

    lay::SelectionService *selector = mp_view->selection_service ();
    if (selector) {
      return selector->mouse_double_click_event (p, buttons, prio);
    }

  }
  return false;
}

bool 
MoveService::mouse_release_event (const geo::DPoint &p, unsigned int buttons, bool prio) 
{ 
  if (prio) {
    lay::SelectionService *selector = mp_view->selection_service ();
    if (selector) {
      return selector->mouse_release_event (p, buttons, prio);
    }
  }
  return false;
}

bool 
MoveService::wheel_event (int delta, bool horizontal, const geo::DPoint &p, unsigned int buttons, bool prio)
{
  if (prio) {
    lay::SelectionService *selector = mp_view->selection_service ();
    if (selector) {
      return selector->wheel_event (delta, horizontal, p, buttons, prio);
    }
  }
  return false;
}

bool 
MoveService::mouse_press_event (const geo::DPoint &p, unsigned int buttons, bool prio)
{
  if (prio && (buttons & lay::LeftButton) != 0) {
    if (handle_click (p, buttons, false, 0)) {
      return true;
    }
  } 
  if (prio) {
    lay::SelectionService *selector = mp_view->selection_service ();
    if (selector) {
      return selector->mouse_press_event (p, buttons, prio);
    }
  }
  return false;
}

bool
MoveService::begin_move (geo::Transaction *transaction, bool selected_after_move)
{
  if (m_dragging) {
    return false;
  }

  std::unique_ptr<geo::Transaction> trans_holder (transaction);

  bool drag_transient = ! selected_after_move;
  if (! mp_editables->has_selection ()) {
    //  try to use the transient selection for the real one
    mp_editables->transient_to_selection ();
    drag_transient = true;
  }

  if (! mp_editables->has_selection ()) {
    //  still nothing selected
    return false;
  }

  geo::DBox bbox = mp_editables->selection_bbox ();
  if (bbox.empty ()) {
    //  nothing (useful) selected
    return false;
  }

  set_cursor (lay::Cursor::size_all);

  //  emulate a "begin move" at the current mouse position if inside the box or the closest point
  //  of the box.

  geo::DPoint pstart = m_mouse_pos;
  if (! bbox.contains (pstart)) {
    pstart.set_x (std::max (pstart.x (), bbox.p1 ().x ()));
    pstart.set_x (std::min (pstart.x (), bbox.p2 ().x ()));
    pstart.set_y (std::max (pstart.y (), bbox.p1 ().y ()));
    pstart.set_y (std::min (pstart.y (), bbox.p2 ().y ()));
  }

  return handle_click (pstart, 0, drag_transient, trans_holder.release ());
}

bool 
MoveService::handle_click (const geo::DPoint &p, unsigned int buttons, bool drag_transient, geo::Transaction *transaction)
{
  std::unique_ptr<geo::Transaction> trans_holder (transaction);

  if (! m_dragging) {

    mp_transaction.reset (trans_holder.release ());

    if (mp_editables->begin_move (p, ac_from_buttons (buttons))) {

      lay::SelectionService *selector = mp_view->selection_service ();
      if (selector) {
        selector->hover_reset ();
      }
        
      mp_view->clear_transient_selection ();

      m_dragging = true;
      m_dragging_transient = drag_transient;
      widget ()->grab_mouse (this, false);

      m_shift = geo::DPoint ();

      return true;

    }

  } else {

    m_dragging = false;

    widget ()->ungrab_mouse (this);
    mp_editables->end_move (p, ac_from_buttons (buttons), mp_transaction.release ());

    if (m_dragging_transient) {
      mp_editables->clear_selection ();
    }

    return true;

  }
  return false;
}

void
MoveService::drag_cancel () 
{ 
  m_shift = geo::DPoint ();
  if (m_dragging) {

    mp_editables->edit_cancel ();
    widget ()->ungrab_mouse (this);

    m_dragging = false;

    if (mp_transaction.get ()) {
      mp_transaction->cancel ();
    }
    mp_transaction.reset (0);

  }
}

}

