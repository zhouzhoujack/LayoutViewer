#include "layMouseTracker.h"

#include "layLayoutCanvas.h"
#include "layLayoutView.h"
#include "layPlugin.h"

namespace lay
{

// -----------------------------------------------------------------------------
//  MouseTracker implementation

MouseTracker::MouseTracker(lay::LayoutView* view)
    : lay::ViewService(view->view_object_widget()), lay::Plugin(0), mp_view(view)
{
    widget()->grab_mouse(this, false);
}

bool MouseTracker::mouse_move_event(const geo::DPoint& p, unsigned int /*buttons*/, bool prio)
{
    if (prio)
    {
        mp_view->current_pos(p.x(), p.y());
    }
    return false;
}

} // namespace lay
