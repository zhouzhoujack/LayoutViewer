#pragma once

#include "layPluginDeclaration.h"

#include <QAction>
#include <QToolBar>

#include <string>
#include <vector>

namespace lay
{

class Dispatcher;
class MainWindow;

class GotoPluginDeclaration : public lay::PluginDeclaration
{
public:
    GotoPluginDeclaration(int id);
    ~GotoPluginDeclaration();

    static GotoPluginDeclaration* instance();

    void create_toobar_action(Dispatcher* dispatcher, QToolBar* toolbar, QActionGroup* actiongroup) override;

    lay::Plugin* create_plugin(geo::Manager* manager, lay::Dispatcher* dispatcher, lay::LayoutView* view,
                               lay::ResourceTree* tree) const override;

    void get_menu_entries(std::vector<lay::MenuEntry>& menu_entries) const override;

    bool menu_activated(const std::string& symbol) const override;

    bool implements_mouse_mode(std::string& title) const override;

    /**
     *  @brief Sets the checked state of the "Pick Position on Canvas" mode action
     *
     *  Called by the GotoPlugin when the goto view service is activated / deactivated
     *  so the toolbar button always reflects the actual pick mode.
     */
    void set_pick_mode_checked(bool on);

private:
    QAction* mp_pick_action{0};
};

} // namespace lay
