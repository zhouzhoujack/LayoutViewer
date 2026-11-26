#ifndef HDR_layMacroPropertiesDialog
#define HDR_layMacroPropertiesDialog

#include "ui_MacroPropertiesDialog.h"
#include "layLymMacro.h"

namespace lay
{

/**
 *  @brief A dialog to edit the properties of a macro
 */
class MacroPropertiesDialog
  : public QDialog, private Ui::MacroPropertiesDialog
{
Q_OBJECT

public:
  MacroPropertiesDialog (QWidget *parent);

  int exec_dialog (lym::Macro *macro);

public slots:
  void shortcut_edited ();

private:
  void update (const lym::Macro *macro);
  void commit (lym::Macro *macro);
};

}

#endif

