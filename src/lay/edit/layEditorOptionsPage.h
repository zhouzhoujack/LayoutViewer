#ifndef HDR_layEditorOptionsPage
#define HDR_layEditorOptionsPage


#include "tlObject.h"
#include "detail/utilsObject.h"

#include <QWidget>

namespace lay
{

class PluginDeclaration;
class Dispatcher;
class LayoutView;
class Plugin;
class CellView;
class EditorOptionsPages;

/**
 *  @brief The base class for a object properties page
 */
class EditorOptionsPage
  : public QWidget, public geo::RefCounted
{
Q_OBJECT

public:
  EditorOptionsPage (lay::LayoutView *view, lay::Dispatcher *dispatcher);
  virtual ~EditorOptionsPage ();

  virtual std::string title () const = 0;
  virtual int order () const = 0;
  virtual void apply (lay::Dispatcher * /*root*/) { }
  virtual void setup (lay::Dispatcher * /*root*/) { }
  virtual void commit_recent (lay::Dispatcher * /*root*/) { }

  bool active () const { return m_active; }
  void activate (bool active);
  void set_owner (EditorOptionsPages *owner);

  const lay::PluginDeclaration *plugin_declaration () const { return mp_plugin_declaration; }
  void set_plugin_declaration (const lay::PluginDeclaration *pd) { mp_plugin_declaration = pd; }

protected slots:
  void edited ()
  {
    apply (dispatcher ());
  }

protected:
  lay::Dispatcher *dispatcher () const
  {
    return mp_dispatcher;
  }

  lay::LayoutView *view () const
  {
    return mp_view;
  }

  virtual void active_cellview_changed () { }
  virtual void technology_changed (const std::string & /*tech*/) { }

private:
  EditorOptionsPages *mp_owner;
  bool m_active;
  const lay::PluginDeclaration *mp_plugin_declaration;
  lay::Dispatcher *mp_dispatcher;
  lay::LayoutView *mp_view;

  void on_active_cellview_changed ();
  void on_technology_changed ();
  void attach_events ();
};

}

#endif
