#pragma once

#include "layEditable.h"
#include "detail/utilsDeferredExecution.h"
#include "layObjectInstPath.h"
#include "layPlugin.h"
#include "layViewObject.h"
#include "layViewService.h"

#include <QObject>
#include <QTimer>

namespace lay
{

class RubberBox;
class LayoutView;
class LayoutCanvas;

class SelectionService : public QObject, public lay::ViewService, public lay::Plugin, public lay::Editable
{
    Q_OBJECT

public:
    typedef std::set<lay::ObjectInstPath> objects;
    typedef objects::const_iterator obj_iterator;

    SelectionService(lay::LayoutView* view);

    ~SelectionService();

    lay::LayoutView* view() const
    {
        tl_assert(mp_view != 0);
        return mp_view;
    }

    void set_colors(QColor background, QColor color);
    void begin(const geo::DPoint& pos);

    bool dragging() const { return mp_box != 0; }
    void hover_reset();

    // Plugin implement
    lay::ViewService* view_service_interface() override { return this; }
    lay::Editable* editable_interface() override { return this; }

    //  ViewService implement
    virtual bool leave_event(bool prio);
    virtual bool enter_event(bool prio);
    virtual bool mouse_move_event(const geo::DPoint& p, unsigned int buttons, bool prio);
    virtual bool mouse_release_event(const geo::DPoint& p, unsigned int buttons, bool prio);
    virtual bool mouse_press_event(const geo::DPoint& p, unsigned int buttons, bool prio);
    virtual bool mouse_click_event(const geo::DPoint& p, unsigned int buttons, bool prio);
    virtual bool mouse_double_click_event(const geo::DPoint& p, unsigned int buttons, bool prio);
    virtual bool wheel_event(int delta, bool horizontal, const geo::DPoint& p, unsigned int buttons, bool prio);

    // Editable implement
    bool select(const geo::DBox& /*box*/, SelectionMode /*mode*/) override;
    void clear_previous_selection() override;

public slots:
    void timeout();

private:
    void reset_box();
    bool select(const lay::ObjectInstPath& obj, lay::Editable::SelectionMode mode);
    void selection_to_view();
    void do_selection_to_view();
    virtual void deactivated();

    geo::DTrans m_move_trans;
    bool m_cell_inst_service;
    objects m_selection;
    objects m_previous_selection;
    objects m_transient_selection;
    bool m_top_level_sel;
    std::vector<lay::ViewObject*> m_markers;
    geo::ShapeIterator::flags_type m_flags;
    unsigned long m_seq;
    bool m_indicate_secondary_selection;
    geo::DeferredMethod<SelectionService> dm_selection_to_view;

    geo::DPoint m_p1, m_p2;
    geo::DPoint m_current_position;
    lay::LayoutView* mp_view;
    lay::RubberBox* mp_box;
    unsigned int m_color;
    unsigned int m_buttons;
    QTimer m_timer;
    bool m_hover;
    bool m_hover_wait;
    geo::DPoint m_hover_point;
    bool m_mouse_in_window;
};

} // namespace lay
