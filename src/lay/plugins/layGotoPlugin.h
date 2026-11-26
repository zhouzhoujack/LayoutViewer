#pragma once

#include "layPlugin.h"
#include "layViewObject.h"
#include "layViewService.h"

#include <QColor>

#include <string>
#include <vector>

namespace lay
{

class LayoutView;
class ResourceTree;
class ViewObjectCanvas;
class Viewport;

// ---------------------------------------------------------------------------------------------
//  GotoView
//  A "goto marker": a crosshair drawn at the target position.

class GotoView : public lay::ViewObject
{
public:
    GotoView(lay::ViewObjectWidget* widget);

    /**
     *  @brief Set the marker position (in database coordinates)
     */
    void set_position(const geo::DPoint& p);

    /**
     *  @brief Get the marker position (in database coordinates)
     */
    const geo::DPoint& position() const { return m_pos; }

    /**
     *  @brief Set the marker color
     */
    void set_color(const QColor& c)
    {
        m_color = c;
        redraw();
    }

    /**
     *  @brief The marker id (used to reference the marker in the resource tree)
     */
    int id() const { return m_id; }
    void id(int i) { m_id = i; }

    /**
     *  @brief The name displayed next to the marker
     */
    void name(const std::string& n)
    {
        m_name = n;
        redraw();
    }
    const std::string& name() const { return m_name; }

private:
    virtual void render(const lay::Viewport& vp, lay::ViewObjectCanvas& canvas) override;

    geo::DPoint m_pos;
    bool m_has_pos;
    QColor m_color;
    int m_id;
    std::string m_name;
};

// ---------------------------------------------------------------------------------------------
//  GotoPlugin
//  A plugin that manages "goto" markers: each goto position creates a marker that is
//  shown on the canvas and listed in the resource tree. Markers can be deleted
//  individually (from the tree) or all at once.

class GotoPlugin : public lay::ViewService, public lay::Plugin
{
public:
    GotoPlugin(lay::LayoutView* view, lay::ResourceTree* tree);
    ~GotoPlugin();

    lay::ViewService* view_service_interface() override { return this; }

    /**
     *  @brief Navigation command: center the view on "p" and add a marker there.
     *  If "s" > 0, "s" is the window size (box edge length) after the goto.
     */
    void goto_position(const geo::DPoint& p, double s = -1.0);

    /**
     *  @brief Open the "enter position" dialog and perform the goto
     */
    void goto_position_dialog(QWidget* parent);

    /**
     *  @brief Delete the marker with the given id
     */
    void remove_goto(int id);

    /**
     *  @brief Delete all markers
     */
    void remove_all_gotos();

    /**
     *  @brief Delete all markers (alias for remove_all_gotos)
     */
    void clear_marker() { remove_all_gotos(); }

    /**
     *  @brief Accessor for the view this plugin belongs to
     */
    lay::LayoutView* view() const { return mp_view; }

    /**
     *  @brief Accessor for the goto markers
     */
    const std::vector<GotoView*>& markers() const { return m_markers; }

    void set_colors(QColor background, QColor text) override;

private:
    //  ViewService events
    virtual bool mouse_click_event(const geo::DPoint& p, unsigned int buttons, bool prio) override;
    virtual void activated() override;
    virtual void deactivated() override;

    lay::LayoutView* mp_view;
    lay::ResourceTree* mp_tree;
    std::vector<GotoView*> m_markers;
    QColor m_color;
    int m_next_id;
};

} // namespace lay
