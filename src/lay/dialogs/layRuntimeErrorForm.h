#ifndef HDR_rbaRuntimeErrorForm
#define HDR_rbaRuntimeErrorForm

#include <string>
#include "tlException.h"
#include "tlScriptError.h"
#include <vector>

#include <QDialog>

#include "ui_RuntimeErrorForm.h"

namespace tl
{
  class ScriptError;
}

namespace lay
{

class RuntimeErrorForm
  : public QDialog, private Ui::RuntimeErrorForm
{
  Q_OBJECT 

public:
  RuntimeErrorForm (QWidget *parent, const char *name, const tl::ScriptError *error);

public slots:
  void show_details ();
};

}

#endif

