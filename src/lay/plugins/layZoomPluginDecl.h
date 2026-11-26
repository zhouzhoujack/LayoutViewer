#pragma once

#include <QAction>

#include "layAbstractMenu.h"
#include "layPlugin.h"
#include "layPluginDeclaration.h"

namespace lay
{

class Dispatcher;
class MainWindow;

class ZoomPluginDeclaration : public lay::PluginDeclaration
{
public:
    ZoomPluginDeclaration(int id);
    ~ZoomPluginDeclaration();

    static ZoomPluginDeclaration* instance();

    // QAction* create_toobar_action(lay::MainWindow* window) override;
    // void get_options(std::vector<std::pair<std::string, std::string>>& options) const override;
    lay::Plugin* create_plugin(geo::Manager* manager, lay::Dispatcher*, lay::LayoutView* view,
                               lay::ResourceTree* tree) const override;

    void get_menu_entries(std::vector<lay::MenuEntry>& menu_entries) const override;
    // virtual bool implements_editable (std::string &title) const;
    // virtual bool implements_mouse_mode (std::string &title) const;
    // virtual bool configure (const std::string &name, const std::string &value);
    // virtual std::vector<std::pair<std::string, lay::ConfigPage*>>
    // config_pages(QWidget* parent) const;
    // virtual void config_finalize ();
    // virtual void initialized (lay::Dispatcher *);
    // virtual void uninitialize (lay::Dispatcher *);
    bool menu_activated(const std::string& symbol) const override;

    // void register_annotation_template (const ant::Template &t);

private:
};

} // namespace lay
