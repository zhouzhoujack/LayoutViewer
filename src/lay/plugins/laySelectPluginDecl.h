#pragma once

#include <QAction>
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"

#include "layAbstractMenu.h"
#include "layPlugin.h"
#include "layPluginDeclaration.h"

namespace lay
{

class Dispatcher;
class MainWindow;

class SelectPluginDeclaration : public lay::PluginDeclaration
{
public:
    SelectPluginDeclaration(int id);
    ~SelectPluginDeclaration();

    static SelectPluginDeclaration* instance();

    void create_toobar_action(Dispatcher* dispatcher, QToolBar* toolbar, QActionGroup* actiongroup) override;
    void get_options(std::vector<std::pair<std::string, std::string>>& options) const override;
    lay::Plugin* create_plugin(geo::Manager* manager, lay::Dispatcher*,
                               lay::LayoutView* view, lay::ResourceTree* tree) const override;

    // const; virtual void get_menu_entries (std::vector<lay::MenuEntry> &menu_entries) const;
    // virtual bool implements_editable (std::string &title) const;
    // virtual bool implements_mouse_mode (std::string &title) const;
    // virtual bool configure (const std::string &name, const std::string &value);
    virtual std::vector<std::pair<std::string, lay::ConfigPage*>>
    config_pages(QWidget* parent) const;
    // virtual void config_finalize ();
    // virtual void initialized (lay::Dispatcher *);
    // virtual void uninitialize (lay::Dispatcher *);
    // virtual bool menu_activated (const std::string &symbol) const;

    // void register_annotation_template (const ant::Template &t);

private:
    void update_current_template();
    void update_menu();

    // std::vector<ant::Template> m_templates;
    int m_current_template;
    geo::weak_collection<lay::ConfigureAction> m_actions;
    bool m_current_template_updated;
    bool m_templates_updated;
};

} // namespace lay
