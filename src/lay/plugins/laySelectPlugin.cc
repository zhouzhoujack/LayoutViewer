
#include "laySelectPlugin.h"
#include "detail/utilsInternational.h"

#include "layFinder.h"
#include "layLayoutView.h"
#include "layRubberBox.h"
#include "tlException.h"
#include "tlLog.h"
#include "layEdtUtils.h"
#include "layMarker.h"

#include <QApplication>
#include <QMessageBox>

namespace lay
{

// -------------------------------------------------------------
//  SelectionService implementation

SelectionService::SelectionService(lay::LayoutView* view)
    : QObject(view),
    lay::ViewService(view->view_object_widget()),
    lay::Plugin(0),
    lay::Editable (view),
    mp_view(view),
    m_flags (geo::ShapeIterator::Regions),
    mp_box(0), m_color(0), m_buttons(0), m_hover(false), m_hover_wait(false),
    m_mouse_in_window(false), dm_selection_to_view(this, &SelectionService::do_selection_to_view)
{
    m_timer.setInterval(100 /*hover time*/);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

SelectionService::~SelectionService()
{
    if (mp_box)
    {
        delete mp_box;
        mp_box = 0;
    }
}

void
SelectionService::clear_previous_selection ()
{
    m_previous_selection.clear ();
}

void SelectionService::do_selection_to_view()
{
    //  Hint: this is a lower bound:
    m_markers.reserve(m_selection.size());

    //  build the transformation variants cache
    edt::TransformationVariants tv(view());

    for (std::set<lay::ObjectInstPath>::iterator r = m_selection.begin(); r != m_selection.end();
         ++r)
    {

        const lay::CellView& cv = view()->cellview(r->cv_index());

        //  compute the global transformation including movement, context and explicit
        //  transformation
        double dbu = cv->layout().dbu();
        geo::ICplxTrans gt = geo::VCplxTrans(1.0 / dbu) * geo::DCplxTrans(m_move_trans) *
                            geo::CplxTrans(dbu) * cv.context_trans() * r->trans();

        tl_assert(r->is_cell_inst() == m_cell_inst_service);

        if (m_cell_inst_service)
        {

            const std::vector<geo::DCplxTrans>* tv_list = tv.per_cv(r->cv_index());
            if (tv_list != 0)
            {

                if (view()->is_editable())
                {
                    lay::InstanceMarker* marker = new lay::InstanceMarker(view(), r->cv_index());

                    marker->set_vertex_shape(lay::ViewOp::Cross);
                    marker->set_vertex_size(9 /*cross vertex size*/);

                    if (r->seq() > 0 && m_indicate_secondary_selection)
                    {
                        marker->set_dither_pattern(3);
                    }
                    marker->set(r->back().inst_ptr, gt, *tv_list);
                    m_markers.push_back(marker);
                }
                else
                {
                    lay::Marker* marker = new lay::Marker(view(), r->cv_index());
                    marker->set_vertex_shape(lay::ViewOp::Cross);
                    marker->set_vertex_size(9 /*cross vertex size*/);

                    if (r->seq() > 0 && m_indicate_secondary_selection)
                    {
                        marker->set_dither_pattern(3);
                    }
                    geo::box_convert<geo::CellInst> bc(cv->layout());
                    marker->set(
                        bc(r->back().inst_ptr.cell_inst().object()),
                        gt * r->back().inst_ptr.cell_inst().complex_trans(*r->back().array_inst),
                        *tv_list);
                    m_markers.push_back(marker);
                }
            }
        }
        else
        {
            const std::vector<geo::DCplxTrans>* tv_list =
                tv.per_cv_and_layer(r->cv_index(), r->layer());
            if (tv_list != 0)
            {

                lay::ShapeMarker* marker = new lay::ShapeMarker(view(), r->cv_index());
                if (r->seq() > 0 && m_indicate_secondary_selection)
                {
                    marker->set_dither_pattern(3);
                }

                marker->set(r->shape(), gt, *tv_list);
                if (r->shape().is_text())
                {
                    //  show the origins as crosses for texts
                    marker->set_vertex_shape(lay::ViewOp::Cross);
                    marker->set_vertex_size(9 /*cross vertex size*/);
                }

                m_markers.push_back(marker);
            }
        }
    }
}

void SelectionService::set_colors(QColor /*background*/, QColor color)
{
    m_color = color.rgb();
    if (mp_box)
    {
        mp_box->set_color(m_color);
    }
}

void SelectionService::deactivated()
{
    mp_view->clear_transient_selection();
    reset_box();
}

void SelectionService::hover_reset()
{
    if (m_hover_wait)
    {
        m_timer.stop();
        m_hover_wait = false;
    }
    if (m_hover)
    {
        mp_view->clear_transient_selection();
        m_hover = false;
    }
}

void SelectionService::timeout()
{
    m_hover_wait = false;
    m_hover = true;
    mp_view->clear_transient_selection();
    mp_view->transient_select(m_hover_point);
}

void SelectionService::reset_box()
{
    if (mp_box)
    {

        widget()->ungrab_mouse(this);

        delete mp_box;
        mp_box = 0;
    }
}

bool SelectionService::wheel_event(int /*delta*/, bool /*horizontal*/, const geo::DPoint& /*p*/,
                                   unsigned int /*buttons*/, bool /*prio*/)
{
    return false;
}

void SelectionService::selection_to_view()
{
    //  we don't handle the transient selection properly, so clear it for safety reasons
    clear_transient_selection();

    //  the selection objects need to be recreated since we destroyed the old markers
    for (std::vector<lay::ViewObject*>::iterator r = m_markers.begin(); r != m_markers.end(); ++r)
    {
        delete *r;
    }
    m_markers.clear();

    dm_selection_to_view();
}

bool SelectionService::select(const lay::ObjectInstPath& obj, lay::Editable::SelectionMode mode)
{
    //  allocate next sequence number
    if (mode == lay::Editable::Replace)
    {
        m_seq = 0;
    }
    else if (mode != lay::Editable::Reset)
    {
        ++m_seq;
    }

    if (mode == lay::Editable::Replace || mode == lay::Editable::Add)
    {
        //  select
        if (m_selection.find(obj) == m_selection.end())
        {

            obj_iterator o = m_selection.insert(obj).first;
            (const_cast<lay::ObjectInstPath&>(*o))
                .set_seq(m_seq); // we can do that since the sequence number is not part of the less
                                 // operator
            selection_to_view();
            return true;
        }
    }
    else if (mode == lay::Editable::Reset)
    {

        //  unselect
        if (m_selection.find(obj) != m_selection.end())
        {

            m_selection.erase(obj);
            selection_to_view();
            return true;
        }
    }
    else
    {
        //  invert selection
        if (m_selection.find(obj) != m_selection.end())
        {
            m_selection.erase(obj);
        }
        else
        {
            obj_iterator o = m_selection.insert(obj).first;
            (const_cast<lay::ObjectInstPath&>(*o))
                .set_seq(m_seq); // we can do that since the sequence number is not part of the less
                                 // operator
        }

        selection_to_view();
        return true;
    }

    return false;
}

bool SelectionService::select(const geo::DBox& box, SelectionMode mode)
{
    //  compute search box
    double l = box.is_point() ? catch_distance() : catch_distance_box();
    geo::DBox search_box = box.enlarged(geo::DVector(l, l));

    bool needs_update = false;
    bool any_selected = false;

    //  clear before unless "add" is selected
    if (mode == lay::Editable::Replace)
    {
        if (!m_selection.empty())
        {
            m_selection.clear();
            needs_update = true;
        }
    }

    //  for single-point selections either exclude the current selection or the
    //  accumulated previous selection from the search.
    const objects* exclude = 0;
    if (mode == lay::Editable::Replace)
    {
        exclude = &m_previous_selection;
    }
    else if (mode == lay::Editable::Add)
    {
        exclude = &m_selection;
    }
    else if (mode == lay::Editable::Reset)
    {
        //  TODO: the finder should favor the current selection in this case.
    }

    lay::ShapeFinder finder(box.is_point(), view()->is_editable() && m_top_level_sel, m_flags,
                            exclude);

    //  go through all visible layers of all cellviews
    finder.find(view(), search_box);

    //  guiding shapes are only selected in point selection mode and even then, we
    //  only select the first shape
    lay::ShapeFinder::iterator f0 = finder.begin();
    if (box.is_point() && f0 != finder.end() &&
        f0->layer() == view()->cellview(f0->cv_index())->layout().guiding_shape_layer())
    {

        m_selection.clear();
        select(*f0, mode);
        m_previous_selection.insert(*f0);
        needs_update = true;
        any_selected = true;
    }
    else
    {
        //  clear the selection if it was consisting of a guiding shape before
        objects::const_iterator s0 = m_selection.begin();
        if (s0 != m_selection.end() &&
            s0->layer() == view()->cellview(s0->cv_index())->layout().guiding_shape_layer())
        {
            m_selection.clear();
        }

        //  collect the founds from the finder
        for (lay::ShapeFinder::iterator f = finder.begin(); f != finder.end(); ++f)
        {
            if (f->layer() != view()->cellview(f->cv_index())->layout().guiding_shape_layer())
            {
                select(*f, mode);
                if (box.is_point())
                {
                    m_previous_selection.insert(*f);
                }
                needs_update = true;
                any_selected = true;
            }
        }
    }

    //  if required, update the list of ruler objects to display the selection
    if (needs_update) {
        selection_to_view ();
    }

    return any_selected;
}

bool SelectionService::enter_event(bool /*prio*/)
{
    m_mouse_in_window = true;
    return false;
}

bool SelectionService::leave_event(bool prio)
{
    m_mouse_in_window = false;

    hover_reset();

    if (prio)
    {
        reset_box();
    }

    return false;
}

bool SelectionService::mouse_move_event(const geo::DPoint& p, unsigned int buttons, bool prio)
{
    if (prio)
    {

        m_current_position = p;

        if ((buttons & LeftButton) == 0)
        {
            reset_box();
        }

        if (mp_box)
        {
            m_p2 = p;
            mp_box->set_points(m_p1, m_p2);
        }
        else if (m_mouse_in_window && mp_view->transient_selection_mode())
        {
            m_hover_wait = true;
            m_timer.start();
            m_hover_point = p;
        }
    }

    return false;
}

bool SelectionService::mouse_double_click_event(const geo::DPoint& /*p*/, unsigned int buttons,
                                                bool prio)
{
    hover_reset();

    if (prio)
    {
        reset_box();
    }

    if (prio && (buttons & lay::LeftButton) != 0)
    {
        mp_view->show_properties(QApplication::activeWindow());
        return true;
    }

    return false;
}

bool SelectionService::mouse_press_event(const geo::DPoint& p, unsigned int buttons, bool prio)
{
    hover_reset();

    if (prio)
    {

        reset_box();

        if ((buttons & lay::LeftButton) != 0)
        {
            mp_view->stop_redraw(); // TODO: how to restart if selection is aborted?
            m_buttons = buttons;
            begin(p);
            return true;
        }
    }

    return false;
}

bool SelectionService::mouse_click_event(const geo::DPoint& p, unsigned int buttons, bool prio)
{
    if (prio)
    {
        reset_box();
    }

    if (prio && mp_view && widget()->mouse_event_viewport().contains(p) &&
        (buttons & lay::LeftButton) != 0)
    {

        lay::Editable::SelectionMode mode = lay::Editable::Replace;
        bool shift = ((buttons & lay::ShiftButton) != 0);
        bool ctrl = ((buttons & lay::ControlButton) != 0);
        if (shift && ctrl)
        {
            mode = lay::Editable::Invert;
        }
        else if (shift)
        {
            mode = lay::Editable::Add;
        }
        else if (ctrl)
        {
            mode = lay::Editable::Reset;
        }

        //  select is allowed to throw an exception
        try
        {

            mp_view->select(p, mode);

            //  add a transient selection trigger to capture the "next" selection.
            if (mp_view->transient_selection_mode())
            {
                m_hover_wait = true;
                m_timer.start();
                m_hover_point = p;
            }
        }
        catch (tl::Exception& ex)
        {
            LOGE << ex.msg();
            QMessageBox::critical(0, QObject::tr("Error"), geo::to_qstring(ex.msg()));
            //  clear selection
            mp_view->select(geo::DBox(), lay::Editable::Reset);
        }
    }

    return false;
}

bool SelectionService::mouse_release_event(const geo::DPoint& /*p*/, unsigned int /*buttons*/,
                                           bool prio)
{
    hover_reset();

    if (prio && mp_box)
    {

        reset_box();

        if (mp_view)
        {

            lay::Editable::SelectionMode mode = lay::Editable::Replace;
            bool shift = ((m_buttons & lay::ShiftButton) != 0);
            bool ctrl = ((m_buttons & lay::ControlButton) != 0);
            if (shift && ctrl)
            {
                mode = lay::Editable::Invert;
            }
            else if (shift)
            {
                mode = lay::Editable::Add;
            }
            else if (ctrl)
            {
                mode = lay::Editable::Reset;
            }

            //  select is allowed to throw an exception
            try
            {
                mp_view->select(geo::DBox(m_p1, m_p2), mode);
            }
            catch (tl::Exception& ex)
            {
                LOGE << ex.msg();
                QMessageBox::critical(0, QObject::tr("Error"), geo::to_qstring(ex.msg()));
                //  clear selection
                mp_view->select(geo::DBox(), lay::Editable::Reset);
            }
        }
    }

    return false;
}

void SelectionService::begin(const geo::DPoint& pos)
{
    if (mp_box)
    {
        delete mp_box;
    }

    m_p1 = pos;
    m_p2 = pos;
    mp_box = new lay::RubberBox(widget(), m_color, pos, pos);
    mp_box->set_stipple(6); // coarse hatched

    widget()->grab_mouse(this, true);
}

} // namespace lay
