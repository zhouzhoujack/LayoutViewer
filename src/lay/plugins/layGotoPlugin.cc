#include "layGotoPlugin.h"
#include "tlException.h"
#include "detail/utilsInternational.h"
#include "detail/utilsString.h"
#include "detail/utilsExpression.h"

#include "layGotoPluginDecl.h"
#include "layCanvasPlane.h"
#include "layLayoutView.h"
#include "layMainWindow.h"
#include "layRenderer.h"
#include "layResourceTree.h"
#include "layViewOp.h"

#include "geoBox.h"
#include "detail/geoEdge.h"
#include "detail/geoHersheyFont.h"
#include "detail/geoPoint.h"
#include "detail/geoTrans.h"
#include "tlExpression.h"
#include "tlString.h"

#include <QInputDialog>
#include <QLineEdit>

#include <algorithm>

namespace lay
{

// ---------------------------------------------------------------------------------------------
//  GotoView implementation

GotoView::GotoView(lay::ViewObjectWidget* widget)
    : lay::ViewObject(widget, false /*non-static, redrawn on every paint*/), m_pos(0.0, 0.0), m_has_pos(false),
      m_color(), m_id(0), m_name()
{
    //  .. nothing yet ..
}

void GotoView::set_position(const geo::DPoint& p)
{
    m_pos = p;
    m_has_pos = true;
    redraw();
}

void GotoView::render(const lay::Viewport& vp, lay::ViewObjectCanvas& canvas)
{
    if (!m_has_pos)
    {
        return;
    }

    int basic_width = int(0.5 + 1.0 / canvas.resolution());

    QColor c(m_color);
    if (!c.isValid())
    {
        c = QColor(canvas.foreground_color().rgb());
    }

    lay::CanvasPlane* plane =
        canvas.plane(lay::ViewOp(c.rgb(), lay::ViewOp::Copy, 0, 0, 0, lay::ViewOp::Rect, basic_width));

    //  draw a crosshair (horizontal / vertical bar plus an X) centered on the target position.
    //  the renderer works in pixel space, so the marker position and size are given in
    //  micron (database) units and transformed with "vp.trans()" (micron -> pixel space).
    //  "half" is chosen so the marker appears as ~14 pixels on screen.
    double half = 14.0 / canvas.resolution() / vp.trans().mag();
    double d = half * 0.35;
    const geo::DPoint& p = m_pos;

    lay::Renderer& renderer = canvas.renderer();
    renderer.draw(geo::DEdge(p - geo::DVector(half, 0.0), p + geo::DVector(half, 0.0)), vp.trans(), 0, plane, 0, 0);
    renderer.draw(geo::DEdge(p - geo::DVector(0.0, half), p + geo::DVector(0.0, half)), vp.trans(), 0, plane, 0, 0);

    //  draw the marker name to the upper-right of the crosshair.
    //  the anchor is computed in pixel space: the crosshair half-size is "half" micron,
    //  which shows up as (14 / resolution) pixels on screen.
    if (!m_name.empty())
    {
        double px = 14.0 / canvas.resolution();
        geo::DPoint tp = vp.trans() * p + geo::DVector(px + 4.0, -(px + 4.0));
        renderer.draw(geo::DBox(tp, tp), m_name, geo::DefaultFont, geo::HAlignLeft, geo::VAlignBottom,
                      geo::DFTrans(geo::DFTrans::r0), 0, 0, 0, plane);
    }
}

// ---------------------------------------------------------------------------------------------
//  GotoPlugin implementation

GotoPlugin::GotoPlugin(lay::LayoutView* view, lay::ResourceTree* tree)
    : lay::ViewService(view->view_object_widget()), lay::Plugin(0), mp_view(view), mp_tree(tree), m_color(),
      m_next_id(0)
{
    //  start disabled: only the active (goto) mode may receive mouse events
    enable(false);
}

GotoPlugin::~GotoPlugin()
{
    for (std::vector<GotoView*>::iterator v = m_markers.begin(); v != m_markers.end(); ++v)
    {
        delete *v;
    }
    m_markers.clear();
}

void GotoPlugin::set_colors(QColor /*background*/, QColor text)
{
    m_color = text;
    for (std::vector<GotoView*>::iterator v = m_markers.begin(); v != m_markers.end(); ++v)
    {
        (*v)->set_color(m_color);
    }
}

bool GotoPlugin::mouse_click_event(const geo::DPoint& p, unsigned int buttons, bool /*prio*/)
{
    //  source-level mutual exclusion: only react if the goto service is the currently
    //  active one. When another tool (ruler, select, ...) is active, the goto service
    //  must not react to clicks.
    if (widget() && widget()->active_service() == this && (buttons & lay::LeftButton) != 0)
    {
        goto_position(p);
        return true;
    }
    return false;
}

void GotoPlugin::activated()
{
    //  the pick mode is now active -> receive mouse events + check the toolbar button
    enable(true);
    if (lay::GotoPluginDeclaration::instance())
    {
        lay::GotoPluginDeclaration::instance()->set_pick_mode_checked(true);
    }
}

void GotoPlugin::deactivated()
{
    //  the pick mode was left -> stop receiving mouse events + uncheck the toolbar button
    enable(false);
    if (lay::GotoPluginDeclaration::instance())
    {
        lay::GotoPluginDeclaration::instance()->set_pick_mode_checked(false);
    }
}

void GotoPlugin::goto_position(const geo::DPoint& p, double s)
{
    if (mp_view)
    {
        mp_view->goto_window(p, s);
    }

    int id = m_next_id++;
    GotoView* marker = new GotoView(widget());
    marker->id(id);
    marker->name("G" + geo::to_string(id));
    marker->set_color(m_color);
    marker->set_position(p);
    m_markers.push_back(marker);

    if (mp_tree)
    {
        mp_tree->add_goto_item(id, p);
    }

    //  echo the operation into the command panel (unless the command originated
    //  from the panel itself)
    if (lay::MainWindow::instance())
    {
        std::string cmd = "goto " + geo::to_string(p.x()) + " " + geo::to_string(p.y());
        if (s > 0.0)
        {
            cmd += " " + geo::to_string(s);
        }
        lay::MainWindow::instance()->echo_command(cmd);
    }
}

void GotoPlugin::remove_goto(int id)
{
    for (std::vector<GotoView*>::iterator v = m_markers.begin(); v != m_markers.end(); ++v)
    {
        if ((*v)->id() == id)
        {
            delete *v;
            m_markers.erase(v);
            mp_tree->remove_goto_item(id);
            break;
        }
    }
}

void GotoPlugin::remove_all_gotos()
{
    for (std::vector<GotoView*>::iterator v = m_markers.begin(); v != m_markers.end(); ++v)
    {
        delete *v;
    }
    m_markers.clear();

    if (mp_tree)
    {
        mp_tree->remove_all_goto_items();
    }
}

void GotoPlugin::goto_position_dialog(QWidget* parent)
{
    if (!mp_view)
    {
        return;
    }

    while (true)
    {
        bool ok = false;

        geo::DBox box(mp_view->box());
        std::string pos;
        pos += geo::micron_to_string(box.center().x()) + "," + geo::micron_to_string(box.center().y());
        pos += ",";
        pos += geo::micron_to_string(std::min(box.width(), box.height()));

        QString text =
            QInputDialog::getText(parent, QObject::tr("Enter Position"),
                                  QObject::tr("Enter position either as pair (x,y) or with window size (x,y,s)\n"
                                              "(x,y) will be the new window center position, s (if given) the new "
                                              "window size"),
                                  QLineEdit::Normal, geo::to_qstring(pos), &ok);

        if (!ok)
        {
            break;
        }
        else if (text.isEmpty())
        {
            throw tl::Exception(geo::to_string(QObject::tr("Enter the position")));
        }
        else
        {
            double x = 0.0, y = 0.0, s = 0.0;
            std::string tt(geo::to_string(text));

            geo::Extractor ex(tt.c_str());
            x = geo::Eval().parse(ex).execute().to_double();
            ex.test(",");
            y = geo::Eval().parse(ex).execute().to_double();

            geo::DPoint pt(x, y);

            if (!ex.at_end())
            {
                ex.test(",");
                s = geo::Eval().parse(ex).execute().to_double();
                goto_position(pt, s);
            }
            else
            {
                goto_position(pt);
            }

            break;
        }
    }
}

} // namespace lay
