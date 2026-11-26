#ifndef HDR_layMacroEditorSetupPage
#define HDR_layMacroEditorSetupPage

#include "layPlugin.h"
#include "ui_MacroEditorSetupPage.h"
#include "layConfigPage.h"

namespace lay
{

struct MacroEditorSetupDialogData;

/**
 *  @brief The dialog for editing the properties of the debugger/editor
 */
class MacroEditorSetupPage
  : public lay::ConfigPage, private Ui::MacroEditorSetupPage
{
Q_OBJECT

public:
  MacroEditorSetupPage (QWidget *parent);
  ~MacroEditorSetupPage ();

  virtual void setup (Dispatcher *root);
  virtual void commit (Dispatcher *root);

protected slots:
  void current_attribute_changed (QListWidgetItem *current, QListWidgetItem *previous);
  void cb_changed (int n);
  void color_changed (QColor c);
  void update_font ();
  void clear_exception_list ();

private:
  void commit_attributes (QListWidgetItem *to_item);
  void update_attributes (QListWidgetItem *from_item);
  void update_ignore_exception_list ();

  MacroEditorSetupDialogData *mp_data;
};

}

#endif

