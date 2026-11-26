#pragma once

#include "gsiObject.h"
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"
#include "detail/utilsEvents.h"
#include "tlObjectCollection.h"

#include <QAction>
#include <QObject>
#include <QToolBar>

namespace geo
{
class Manager;
class TechnologyComponentProvider;
} 

namespace lay
{

class ConfigPage;
class Action;
class Dispatcher;
class MainWindow;
class MenuEntry;
class Plugin;
class LayoutView;
class EditorOptionsPage;
class ResourceTree;

class PluginDeclaration : public QObject, public gsi::ObjectBase
{
    Q_OBJECT

public:
    PluginDeclaration(int id = -1);
    virtual ~PluginDeclaration();

    int id() const { return m_id; }

    virtual void create_toobar_action(Dispatcher*, QToolBar*, QActionGroup*)
    {
        // return 0;
    }

    virtual lay::Plugin* create_plugin(geo::Manager*, lay::Dispatcher*, lay::LayoutView*,
                                       lay::ResourceTree* = 0) const
    {
        return 0;
    }

    virtual ConfigPage* config_page(QWidget* /*parent*/, std::string& /*title*/) const { return 0; }

    /**
     *  @brief This method is supposed to deliver the option names available
     *
     *  @param options A vector of names and default value strings.
     */
    virtual void get_options(std::vector<std::pair<std::string, std::string>>& /*options*/) const
    {
        //  the default implementation does not add any options
    }

    /**
     *  @brief Fetch the configuration pages for the configuration dialog
     *
     *  @return A list of titles and ConfigPage instances
     *
     *  This method can be reimplemented alternatively to "config_page" when multiple pages shall be
     *  used. The pages are inserted in the order they are defined.
     */
    virtual std::vector<std::pair<std::string, ConfigPage*>> config_pages(QWidget* /*parent*/) const
    {
        return std::vector<std::pair<std::string, ConfigPage*>>();
    }

    /**
     *  @brief The global configuration
     *
     *  By implementing this method, a configuration can be treated globally.
     *  Otherwise the individual plugin object (associated with a view) must handle
     *  the configuration. One example for handling the configuration globally is
     *  dynamic menu configuration.
     */
    virtual bool configure(const std::string& /*name*/, const std::string& /*value*/)
    {
        return false;
    }

    /**
     *  @brief Global menu handler
     *
     *  This menu handler is called before the view specific handler is
     *  called (lay::Plugin::menu_activated).
     *  The implementation is supposed to return true if it handles the menu entry.
     */
    virtual bool menu_activated(const std::string& /*symbol*/) const
    {
        //  The default implementation does nothing.
        return false;
    }

    /**
     *  @brief Configuration finalization
     *
     *  This method is called after all configuration changes have been applied.
     *  A derived class might implement this method in order to do some post-
     *  processing of the configuration.
     */
    virtual void config_finalize()
    {
        //  .. the default implementation does nothing ..
    }

    /**
     *  @brief Basic initialization
     *
     *  Reimplementation of this method offers a chance to initialize static resources such as
     *  dialogs etc.
     */
    virtual void initialize(lay::Dispatcher* /*dispatcher*/)
    {
        //  .. the default implementation does nothing ..
    }

    /**
     *  @brief Basic initialization
     *
     *  Reimplementation of this method offers a chance to initialize static resources such as
     *  dialogs etc.
     *  While initialize is called before any configuration is loaded, "initialized" will be
     *  called after the plugin system has been initially configured.
     */
    virtual void initialized(lay::Dispatcher* /*dispatcher*/)
    {
        //  .. the default implementation does nothing ..
    }

    /**
     *  @brief Uninitialize the plugin
     */
    virtual void uninitialize(lay::Dispatcher* /*dispatcher*/)
    {
        //  .. the default implementation does nothing ..
    }

    /**
     *  @brief Gets a value indicating whether the plugin can exit
     *
     *  If the plugin wants to prevent the application from closing, it may return false
     *  in this method.
     */
    virtual bool can_exit(lay::Dispatcher* /*dispatcher*/) const { return true; }
    /**
     *  @brief Fetches the menu objects for this plugin
     *
     *  The implementation of this method is supposed to call the base
     *  classes "get_menu_entries" method and add it's own entries.
     */
    virtual void get_menu_entries(std::vector<lay::MenuEntry>& /*menu_entries*/) const
    {
        //  .. nothing yet ..
    }

    /**
     *  @brief Tells if the plugin implements the "lay::Editable" interface
     *
     *  This method is supposed to return true if the plugin implements the
     *  lay::Editable interface and wants to be listed as an "edit" mode in the
     *  "Edit" menu. The string returned in "title" is displayed in the "Edit" menu's
     *  corresponding mode entry.
     */
    virtual bool implements_editable(std::string& /*title*/) const { return false; }

    /**
     *  @brief Gets the editor options pages
     *
     *  Editor options pages are shown in the editor options dockable tool widget. Each plugin can
     * create such pages and these will be shown in tabs inside this widget.
     *
     *  The new pages are returned in the "pages" vector. The layout view will take ownership of
     * these pages.
     */
    virtual void get_editor_options_pages(std::vector<lay::EditorOptionsPage*>& /*pages*/,
                                          lay::LayoutView* /*view*/,
                                          lay::Dispatcher* /*dispatcher*/) const
    {
        //  .. no pages in the default implementation ..
    }

    /**
     *  @brief Tells if the plugin implements a "lay::ViewService" active mouse mode
     *
     *  This method is supposed to return true if the plugin implements the
     *  lay::ViewService interface and wants to be listed as an mouse mode in the
     *  "Edit" menu. The string returned in "title" is displayed in the "Edit" menu's
     *  corresponding mode entry.
     */
    virtual bool implements_mouse_mode(std::string& /*title*/) const { return false; }

    /**
     *  @brief Specifies the primary mouse modes
     *
     *  These are built-in modes from the LayoutView. This method is intended for
     *  the LayoutView's standard modes only.
     */
    virtual void implements_primary_mouse_modes(
        std::vector<std::pair<std::string, std::pair<std::string, int>>>& /*modes*/)
    {
        //  .. nothing yet ..
    }

    /**
     *  @brief Returns the technology component provider
     *
     *  If this plugin wants to register a technology component, it's declaration must
     *  include a technology component provider. This is responsible for creating
     *  a technology component and an associated editor.
     *
     *  @return The provide object (no ownership is taken) or 0 if no technology component is
     * created.
     */
    virtual const geo::TechnologyComponentProvider* technology_component_provider() const
    {
        return 0;
    }

    /**
     *  @brief Creates the menu resources for this plugin
     *
     *  This method will create the menu resources for the plugin and perform the
     *  required connect operations. The dispatcher provides the menu and the
     *  event endpoints.
     */
    void init_menu(lay::Dispatcher* dispatcher);

    /**
     *  @brief Gets the available menu symbols from all plugins
     *
     *  This does not mean all symbols will be available.
     */
    static std::vector<std::string> menu_symbols();

    /**
     *  @brief Removes the menu resources associated with this plugin
     */
    void remove_menu_items(lay::Dispatcher* dispatcher);

    /**
     *  @brief Enables this editable part of the plugin
     *
     *  If the editable part of this plugin is enabled, the objects managed by this mode can be
     * manipulated. Otherwise, the are not considered for selection. This property can be monitored
     * with the editable_enabled observer.
     */
    void set_editable_enabled(bool f);

    /**
     *  @brief Gets a value indicating whether the editable part of the plugin is enabled
     */
    bool editable_enabled() const { return m_editable_enabled; }

    /**
     *  @brief Gets a value indicating whether the plugin will accept a dropped file with the given
     * URL or path
     */
    virtual bool accepts_drop(const std::string& /*path_or_url*/) const { return false; }

    /**
     *  @brief Gets called when a file or URL is dropped on the plugin
     */
    virtual void drop_url(const std::string& /*path_or_url*/)
    {
        //  .. nothing yet ..
    }
    /**
     *  @brief Notifies that dispatcher that a new plugin was registered
     *
     *  This method must be called when a plugin is dynamically created. It is important
     *  that when this method is called, the menu items and other properties are set already.
     */
    void register_plugin();

    /**
     *  @brief An event indicating that the editable state has changed
     */
    geo::Event editable_enabled_changed_event;

private slots:
    void toggle_editable_enabled();

private:
    int m_id;
    geo::weak_ptr<lay::Action> mp_editable_mode_action;
    geo::weak_ptr<lay::Action> mp_mouse_mode_action;
    geo::weak_collection<lay::Action> m_menu_actions;
    bool m_editable_enabled;
};

} // namespace lay
