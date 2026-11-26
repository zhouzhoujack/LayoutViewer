#ifndef HDR_layEditorOptionsPages
#define HDR_layEditorOptionsPages

#include "layEditorOptionsPage.h"

#include <tlVariant.h>

#include <QFrame>
#include <vector>
#include <string>

class QTabWidget;
class QLabel;

namespace lay
{

class PluginDeclaration;
class Dispatcher;
class Plugin;

/**
 *  @brief The object properties dialog
 */
class EditorOptionsPages
  : public QFrame
{
Q_OBJECT

public:
  EditorOptionsPages (QWidget *parent, const std::vector<lay::EditorOptionsPage *> &pages, lay::Dispatcher *root);
  ~EditorOptionsPages ();

  void unregister_page (lay::EditorOptionsPage *page);
  void activate_page (lay::EditorOptionsPage *page);
  void focusInEvent (QFocusEvent *event);

  const std::vector <lay::EditorOptionsPage *> &pages () const
  {
    return m_pages;
  }

  bool has_content () const;

public slots:
  void apply ();
  void setup ();

private:
  std::vector <lay::EditorOptionsPage *> m_pages;
  lay::Dispatcher *mp_dispatcher;
  QTabWidget *mp_pages;

  void update (lay::EditorOptionsPage *page);
  void do_apply ();
};

}

#endif

