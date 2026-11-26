#include "layZoomBox.h"
#include "detail/utilsString.h"
#include "layRubberBox.h"
#include "layLayoutView.h"

namespace lay
{

// -------------------------------------------------------------
//  ZoomService implementation

ZoomService::ZoomService (lay::LayoutView *view)
  : lay::ViewService (view->view_object_widget ()),
    lay::Plugin(0),
    mp_view (view),
    mp_box (0),
    m_color (0)
{ }

ZoomService::~ZoomService ()
{
  drag_cancel ();
}

void
ZoomService::drag_cancel ()
{
  if (mp_box) {
    delete mp_box;
    mp_box = 0;
  }
  widget ()->ungrab_mouse (this);
}

void 
ZoomService::set_colors (QColor /*background*/, QColor color)
{
  m_color = color.rgb ();
  if (mp_box) {
    mp_box->set_color (m_color);
  }
}

bool 
ZoomService::mouse_move_event (const geo::DPoint &p, unsigned int /*buttons*/, bool prio) 
{
  if (prio) {

    if (mp_box) {

      m_p2 = p;
      mp_box->set_points (m_p1, m_p2);

      mp_view->message ("w: " + geo::micron_to_string (fabs (m_p2.x () - m_p1.x ())) + "  h: " + geo::micron_to_string (fabs (m_p2.y () - m_p1.y ())));

    } else if (mp_view) {

      m_vp.move (m_p1 - p);
      mp_view->pop_state ();  //  we will overwrite the previous state so we don't collect tiny move events
      mp_view->zoom_box (m_vp);

    }

    return true;

  } else {
    return false;
  }
}

bool 
ZoomService::mouse_press_event (const geo::DPoint &p, unsigned int buttons, bool prio)
{
  if (! prio && (buttons & lay::RightButton) != 0) {

    mp_view->stop_redraw (); // TODO: how to restart if zoom is aborted?
    if ((buttons & lay::ShiftButton) != 0) {
      begin_pan (p);
    } else {
      begin (p);
    }

    return true;

  } else if (! prio && (buttons & lay::MidButton) != 0) {

    mp_view->stop_redraw (); // TODO: how to restart if zoom is aborted?
    begin_pan (p);
    return true;

  }

  return false;
}

bool 
ZoomService::mouse_click_event (const geo::DPoint &p, unsigned int buttons, bool prio) 
{ 
  if (! prio && (buttons & lay::RightButton) != 0) {
    geo::DBox vp = widget ()->mouse_event_viewport ();
    if (mp_view && vp.contains (p)) {
      geo::DVector d = (vp.p2 () - vp.p1 ()) * 0.5;
      mp_view->zoom_box (geo::DBox (p - d, p + d));
    }
  }
  return false;
}

bool 
ZoomService::mouse_release_event (const geo::DPoint & /*p*/, unsigned int /*buttons*/, bool prio) 
{ 
  if (prio) {

    widget ()->ungrab_mouse (this);

    if (mp_box) {

      delete mp_box;
      mp_box = 0;

      geo::DBox vp = widget ()->mouse_event_viewport ();
      geo::DVector d = (vp.p2 () - vp.p1 ()) * 0.5;

      if (mp_view) {

        //  we need to use the original screen coordinate to find the move direction
        geo::DPoint p1s = widget ()->mouse_event_trans ().trans (m_p1);
        geo::DPoint p2s = widget ()->mouse_event_trans ().trans (m_p2);

        if (p2s.x () > p1s.x () && p1s.y () < p2s.y ()) {

          //  upward right: zoom fit
          mp_view->zoom_fit ();

        } else {

          double fx = fabs (m_p2.x () - m_p1.x ()) / vp.width ();
          double fy = fabs (m_p2.y () - m_p1.y ()) / vp.height ();
          double f = std::max (0.001, std::max (fx, fy));

          if (p1s.x () > p2s.x () || p1s.y () < p2s.y ()) {
            // zoom out
            f = 1.0 / f;
          }

          geo::DPoint c = m_p1 + (m_p2 - m_p1) * 0.5;
          geo::DBox b (c - d * f, c + d * f);
          mp_view->zoom_box (b);

        }

      }

    }

  }

  return false;
}

bool 
ZoomService::wheel_event (int delta, bool /*horizontal*/, const geo::DPoint &p, unsigned int buttons, bool prio)
{
  //  Only act without the mouse being grabbed.
  if (! prio) {

    geo::DBox vp = widget ()->mouse_event_viewport ();
    if (mp_view && vp.contains (p) && vp.width () > 0 && vp.height () > 0) {

      enum { horizontal, vertical, zoom } direction = zoom;
      if (mp_view->mouse_wheel_mode () == 0) {

        if ((buttons & lay::ShiftButton) != 0) {
          direction = vertical;
        } else if ((buttons & lay::ControlButton) != 0) {
          direction = horizontal;
        } else {
          direction = zoom;
        }

      } else {

        if ((buttons & lay::ShiftButton) != 0) {
          direction = horizontal;
        } else if ((buttons & lay::ControlButton) != 0) {
          direction = zoom;
        } else {
          direction = vertical;
        }

      }

      if (direction == vertical) {

        if (delta > 0) {
          mp_view->pan_up ();
        } else {
          mp_view->pan_down ();
        }

      } else if (direction == horizontal) {

        if (delta > 0) {
          mp_view->pan_left ();
        } else {
          mp_view->pan_right ();
        }

      } else {

        double zoom_step = 0.25; // TODO: make variable?

        double f;
        if (delta > 0) {
          f = 1.0 / (1.0 + zoom_step * (delta / 120.0));
        } else {
          f = 1.0 + zoom_step * (-delta / 120.0);
        }

        mp_view->zoom_box (geo::DBox (p.x () - (p.x () - vp.left ()) * f, 
                                     p.y () - (p.y () - vp.bottom ()) * f,
                                     p.x () - (p.x () - vp.right ()) * f, 
                                     p.y () - (p.y () - vp.top ()) * f));

      }

    }

  }
  return false;
}

void 
ZoomService::begin_pan (const geo::DPoint &pos)
{ 
  if (mp_box) {
    delete mp_box;
  }
  mp_box = 0;

  m_p1 = pos;
  m_vp = widget ()->mouse_event_viewport ();

  //  store one state which we are going to update
  mp_view->zoom_box (m_vp);

  widget ()->grab_mouse (this, true);
}

void 
ZoomService::begin (const geo::DPoint &pos)
{ 
  if (mp_box) {
    delete mp_box;
  }

  m_p1 = pos;
  m_p2 = pos;
  mp_box = new lay::RubberBox (widget (), m_color, pos, pos);

  widget ()->grab_mouse (this, true);
}

}

