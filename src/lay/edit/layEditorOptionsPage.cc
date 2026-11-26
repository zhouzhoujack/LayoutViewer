#include "tlInternational.h"
#include "layEditorOptionsPage.h"
#include "layEditorOptionsPages.h"
#include "layLayoutView.h"

namespace lay
{

// ------------------------------------------------------------------
//  EditorOptionsPage implementation

EditorOptionsPage::EditorOptionsPage (lay::LayoutView *view, lay::Dispatcher *dispatcher)
  : QWidget (0), mp_owner (0), m_active (true), mp_plugin_declaration (0), mp_dispatcher (dispatcher), mp_view (view)
{
  attach_events ();
}

EditorOptionsPage::~EditorOptionsPage ()
{
  set_owner (0);
}

void
EditorOptionsPage::attach_events ()
{
  detach_from_all_events ();
  view ()->active_cellview_changed_event.add (this, &EditorOptionsPage::on_active_cellview_changed);
  int cv_index = view ()->active_cellview_index ();
  if (cv_index >= 0) {
    view ()->cellview (cv_index)->technology_changed_event.add (this, &EditorOptionsPage::on_technology_changed);
  }
}

void
EditorOptionsPage::on_active_cellview_changed ()
{
  active_cellview_changed ();
  attach_events ();
}

void
EditorOptionsPage::on_technology_changed ()
{
  technology_changed (view ()->active_cellview_ref ()->tech_name ());
}

void
EditorOptionsPage::set_owner (EditorOptionsPages *owner)
{
  if (mp_owner) {
    mp_owner->unregister_page (this);
  }
  mp_owner = owner;
}

void
EditorOptionsPage::activate (bool active)
{
  if (m_active != active) {
    m_active = active;
    if (mp_owner) {
      mp_owner->activate_page (this);
    }
  }
}

}
