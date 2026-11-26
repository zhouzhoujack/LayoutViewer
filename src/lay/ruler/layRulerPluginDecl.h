#ifndef HDR_antPlugin
#define HDR_antPlugin

#include "layPlugin.h"
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"
#include "layPluginDeclaration.h"

namespace lay
{

class Template;

class RulerPluginDeclaration : public lay::PluginDeclaration
{
public:
    RulerPluginDeclaration(int id);
    ~RulerPluginDeclaration();

    static RulerPluginDeclaration* instance();

    /**
     *  @brief Gets the ruler creation mode (0 = Common Ruler, 1 = Measure)
     */
    int ruler_mode() const { return m_ruler_mode; }

    /**
     *  @brief Sets the ruler creation mode (0 = Common Ruler, 1 = Measure)
     *  and applies it to all open views.
     */
    void set_ruler_mode(int mode);

    void create_toobar_action(Dispatcher* dispatcher, QToolBar* , QActionGroup*) override;

    lay::Plugin* create_plugin(geo::Manager* manager, lay::Dispatcher* dispatcher,
                               lay::LayoutView* view, lay::ResourceTree* tree) const override;

    virtual void get_options(std::vector<std::pair<std::string, std::string>>& options) const;
    virtual void get_menu_entries(std::vector<lay::MenuEntry>& menu_entries) const;

    virtual bool implements_editable(std::string& title) const;
    virtual bool implements_mouse_mode(std::string& title) const;
    virtual bool configure(const std::string& name, const std::string& value);
    virtual std::vector<std::pair<std::string, lay::ConfigPage*>>
    config_pages(QWidget* parent) const;
    virtual void config_finalize();
    virtual void initialized(lay::Dispatcher*);
    virtual void uninitialize(lay::Dispatcher*);
    virtual bool menu_activated(const std::string& symbol) const;

    void register_annotation_template(const lay::Template& t);

private:
    void update_current_template();
    void update_menu();

    std::vector<lay::Template> m_templates;
    int m_current_template;
    int m_ruler_mode;
    // geo::weak_collection<lay::ConfigureAction> m_actions;
    bool m_current_template_updated;
    bool m_templates_updated;
};

} // namespace lay

#endif
