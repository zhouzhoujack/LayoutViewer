#pragma once

#include "detail/geoPoint.h"
#include "layCursor.h"

#include <QColor>

namespace lay
{

class ViewObjectWidget;
class DragDropDataBase;

class ViewService
{
public:
    ViewService(ViewObjectWidget* widget = 0);

    virtual ~ViewService();

    /**
     *  @brief Key press event handler
     *
     *  This method will be called by the ViewObjectWidget object to
     *  dispatch key press events.
     *
     *  The active service will receive that call and should return true
     *  if the event is taken. Otherwise the event will be passed further.
     */
    virtual bool key_event(unsigned int /*key*/, unsigned int /*buttons*/)
    {
        return false;
    }

    /**
     *  @brief The drag enter event
     *
     *  If something is dragged into the view, this event is triggered.
     *  p will be the position of the event.
     *
     *  The implementation should return true, if the drop is accepted.
     */
    virtual bool drag_enter_event(const geo::DPoint& /*p*/, const DragDropDataBase* /*data*/)
    {
        return false;
    }

    /**
     *  @brief The drag move event
     */
    virtual bool drag_move_event(const geo::DPoint& /*p*/, const DragDropDataBase* /*data*/)
    {
        return false;
    }

    /**
     *  @brief The drag leave event
     */
    virtual void drag_leave_event()
    {
    }

    /**
     *  @brief The drop event
     */
    virtual bool drop_event(const geo::DPoint& /*p*/, const DragDropDataBase* /*data*/)
    {
        return false;
    }

    /**
     *  @brief Mouse press event handler
     *
     *  This method will be called by the ViewObjectWidget object to
     *  dispatch mouse press events. First, the objects that grabbed
     *  the mouse will receive the events with prio "true" in the reverse order they
     *  grabbed the mouse (last one first). This loop will terminate
     *  if one of the objects has returned "true".
     *  If no service has grabbed the mouse or none of them was taking the
     *  event, the active service receives the event with prio set to "true".
     *  If this is not the case, then all objects will receive
     *  the event with "prio" set to "false", unless one of the objects
     *  returns "true".
     *  This event is not sent immediately when the mouse button is pressed
     *  but when a signification movement for the mouse cursor away from the
     *  original position is detected. If the mouse button is released before
     *  that, a mouse_clicked_event is sent rather than a press-move-release
     *  sequence.
     *
     *  @param p The point at which the button was pressed
     *  @param buttons A ored combination of ShiftButton etc.
     *  @return True to terminate dispatcher
     */
    virtual bool mouse_press_event(const geo::DPoint& /*p*/, unsigned int /*buttons*/, bool /*prio*/)
    {
        return false;
    }

    /**
     *  @brief Mouse single-click event handler
     *
     *  Analogous to mouse_press_event (see above), but sent if the mouse was not moved.
     *  The click event is coincident with the release of the mouse button.
     */
    virtual bool mouse_click_event(const geo::DPoint& /*p*/, unsigned int /*buttons*/, bool /*prio*/)
    {
        return false;
    }

    /**
     *  @brief Mouse double-click event handler
     *
     *  Analogous to mouse_press_event (see above), but sent if a double-click was detected.
     */
    virtual bool mouse_double_click_event(const geo::DPoint& /*p*/, unsigned int /*buttons*/,
                                          bool /*prio*/)
    {
        return false;
    }

    /**
     *  @brief Mouse leave event handler
     */
    virtual bool leave_event(bool /*prio*/)
    {
        return false;
    }

    /**
     *  @brief Mouse enter event handler
     */
    virtual bool enter_event(bool /*prio*/)
    {
        return false;
    }

    /**
     *  @brief Mouse move event handler
     *
     *  Analogous to mouse_press_event (see above).
     */
    virtual bool mouse_move_event(const geo::DPoint& /*p*/, unsigned int /*buttons*/, bool /*prio*/)
    {
        return false;
    }
    /**
     *  @brief Mouse release event handler
     *
     *  Analogous to mouse_press_event (see above).
     */
    virtual bool mouse_release_event(const geo::DPoint& /*p*/, unsigned int /*buttons*/,
                                     bool /*prio*/)
    {
        return false;
    }

    /**
     *  @brief Wheel event handler
     *
     *  Analogous to mouse_press_event (see above).
     *
     *  @param delta The rotation angle in eights of a degree
     *  @param horizontal True, if the horizontal wheel was turned
     *  @param p The position where the mouse currently is at (in micron units)
     */
    virtual bool wheel_event(int /*delta*/, bool /*horizontal*/, const geo::DPoint& /*p*/,
                             unsigned int /*buttons*/, bool /*prio*/)
    {
        return false;
    }

    /**
     *  @brief Set the mouse cursor
     *
     *  This method can be called from a mouse event handler to set the cursor for the view.
     *  It is only effective if called from a mouse event handler.
     */
    void set_cursor(lay::Cursor::cursor_shape cursor);

    /**
     *  @brief Do an update
     *
     *  This method is supposed to update the internals,
     *  specifically upon changing of the coordinate system
     */
    virtual void update()
    {
        //  The default implementation does nothing
    }

    /**
     *  @brief Accessor to the widget pointer
     */
    ViewObjectWidget* widget() const
    {
        return mp_widget;
    }

    /**
     *  @brief This method is called on the service that got activated
     */
    virtual void activated()
    {
    }

    /**
     *  @brief This method is called on the service that got deactivated
     */
    virtual void deactivated()
    {
    }

    /**
     *  @brief This method is called to set the background and text (foreground) color
     */
    virtual void set_colors(QColor /*background*/, QColor /*text*/)
    {
    }

    /**
     *  @brief This method is called when a drag operation should be cancelled
     */
    virtual void drag_cancel()
    {
    }

    /**
     *  @brief Enable or disable a service
     *
     *  If a service is disabled, it will not receive mouse events
     */
    void enable(bool en);

    /**
     *  @brief Returns true, if the service is enabled
     */
    bool enabled() const
    {
        return m_enabled;
    }

private:
    friend class ViewObjectWidget;

    ViewObjectWidget* mp_widget;
    bool m_abs_grab;
    bool m_enabled;
};

} // namespace lay
