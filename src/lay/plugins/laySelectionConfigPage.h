#pragma once

#include "layConfigPage.h"

namespace Ui {
  class LayoutViewConfigPage2c;
}

namespace lay {

class LayoutViewConfigPage2c 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage2c (QWidget *parent);
  ~LayoutViewConfigPage2c ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage2c *mp_ui;
};

}
