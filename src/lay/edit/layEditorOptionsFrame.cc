#include "layEditorOptionsFrame.h"
#include "tlClassRegistry.h"
#include "layEditorOptionsPage.h"
#include "layEditorOptionsPages.h"
#include "layPlugin.h"
#include "layLayoutView.h"
#include "layPluginDeclaration.h"

#include <QVBoxLayout>

namespace lay
{

// -----------------------------------------------------------------------------
//  EditorOptionsFrame implementation

EditorOptionsFrame::EditorOptionsFrame (QWidget *parent)
  : QFrame (parent), mp_pages (0)
{
  setObjectName (QString::fromUtf8 ("editor_options_frame"));

  QVBoxLayout *left_frame_ly = new QVBoxLayout (this);
  left_frame_ly->setMargin (0);
  left_frame_ly->setSpacing (0);
}

EditorOptionsFrame::~EditorOptionsFrame ()
{
  //  .. nothing yet ..
}

void
EditorOptionsFrame::populate (LayoutView *view)
{
  std::vector<lay::EditorOptionsPage *> prop_dialog_pages;
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    cls->get_editor_options_pages (prop_dialog_pages, view, view->dispatcher ());
  }

  for (std::vector<lay::EditorOptionsPage *>::const_iterator op = prop_dialog_pages.begin (); op != prop_dialog_pages.end (); ++op) {
    (*op)->activate (false);
  }

  if (mp_pages) {
    delete mp_pages;
  }

  mp_pages = new lay::EditorOptionsPages (this, prop_dialog_pages, view);
  layout ()->addWidget (mp_pages);
  setFocusProxy (mp_pages);
}

}
