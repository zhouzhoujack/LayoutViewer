#ifndef HDR_layLayoutViewConfigPages
#define HDR_layLayoutViewConfigPages

#include "layPlugin.h"
#include "layColorPalette.h"
#include "layStipplePalette.h"
#include "layLineStylePalette.h"
#include "layDitherPattern.h"
#include "layLineStyles.h"
#include "detail/geoObject.h"
#include "layConfigPage.h"

namespace Ui {
  class LayoutViewConfigPage;
  class LayoutViewConfigPage1;
  class LayoutViewConfigPage2a;
  class LayoutViewConfigPage2b;
  class LayoutViewConfigPage2c;
  class LayoutViewConfigPage2d;
  class LayoutViewConfigPage3a;
  class LayoutViewConfigPage3b;
  class LayoutViewConfigPage3c;
  class LayoutViewConfigPage3f;
  class LayoutViewConfigPage4;
  class LayoutViewConfigPage5;
  class LayoutViewConfigPage6;
  class LayoutViewConfigPage6a;
  class LayoutViewConfigPage7;
}

namespace lay
{

class ColorButton;

// -------------------------------------------------------------
//  This file provides the main declarations for the configuration
//  dialogs for the layout view.

class LayoutViewConfigPage 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage (QWidget *parent);
  ~LayoutViewConfigPage ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage *mp_ui;
};

class LayoutViewConfigPage1 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage1 (QWidget *parent);
  ~LayoutViewConfigPage1 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage1 *mp_ui;
};

class LayoutViewConfigPage2a
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage2a (QWidget *parent);
  ~LayoutViewConfigPage2a ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage2a *mp_ui;
};

class LayoutViewConfigPage2b
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage2b (QWidget *parent);
  ~LayoutViewConfigPage2b ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage2b *mp_ui;
};


class LayoutViewConfigPage2d
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage2d (QWidget *parent);
  ~LayoutViewConfigPage2d ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage2d *mp_ui;
};

class LayoutViewConfigPage3a
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage3a (QWidget *parent);
  ~LayoutViewConfigPage3a ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage3a *mp_ui;
};

class LayoutViewConfigPage3b 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage3b (QWidget *parent);
  ~LayoutViewConfigPage3b ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage3b *mp_ui;
};

class LayoutViewConfigPage3c 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage3c (QWidget *parent);
  ~LayoutViewConfigPage3c ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage3c *mp_ui;
};

class LayoutViewConfigPage3f 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage3f (QWidget *parent);
  ~LayoutViewConfigPage3f ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage3f *mp_ui;
};

class LayoutViewConfigPage4 
  : public lay::ConfigPage, 
    public geo::Object
{
Q_OBJECT

public:
  LayoutViewConfigPage4 (QWidget *parent);
  ~LayoutViewConfigPage4 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

  virtual void undo (geo::Op *op);
  virtual void redo (geo::Op *op);

public slots:
  void color_button_clicked ();
  void undo_button_clicked ();
  void redo_button_clicked ();
  void reset_button_clicked ();
  void edit_order_changed (int s);
  
private:
  Ui::LayoutViewConfigPage4 *mp_ui;
  lay::ColorPalette m_palette;
  geo::Manager m_manager;
  bool m_edit_order_changed_disabled;

  void update ();
  void set_edit_order (bool edit_order);
};

class LayoutViewConfigPage5 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage5 (QWidget *parent);
  ~LayoutViewConfigPage5 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

public slots:
  void browse_clicked ();

private:
  Ui::LayoutViewConfigPage5 *mp_ui;
};

class LayoutViewConfigPage6 
  : public lay::ConfigPage, 
    public geo::Object
{
Q_OBJECT

public:
  LayoutViewConfigPage6 (QWidget *parent);
  ~LayoutViewConfigPage6 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

  virtual void undo (geo::Op *op);
  virtual void redo (geo::Op *op);

public slots:
  void stipple_button_clicked ();
  void undo_button_clicked ();
  void redo_button_clicked ();
  void reset_button_clicked ();
  void edit_order_changed (int s);
  
private:
  Ui::LayoutViewConfigPage6 *mp_ui;
  lay::StipplePalette m_palette;
  geo::Manager m_manager;
  bool m_edit_order_changed_disabled;
  lay::DitherPattern m_pattern;

  void update ();
  void set_edit_order (bool edit_order);
};

class LayoutViewConfigPage6a
  : public lay::ConfigPage,
    public geo::Object
{
Q_OBJECT

public:
  LayoutViewConfigPage6a (QWidget *parent);
  ~LayoutViewConfigPage6a ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

  virtual void undo (geo::Op *op);
  virtual void redo (geo::Op *op);

public slots:
  void line_style_button_clicked ();
  void undo_button_clicked ();
  void redo_button_clicked ();
  void reset_button_clicked ();

private:
  Ui::LayoutViewConfigPage6a *mp_ui;
  lay::LineStylePalette m_palette;
  geo::Manager m_manager;
  lay::LineStyles m_style;

  void update ();
  void set_edit_order (bool edit_order);
};

class LayoutViewConfigPage7
  : public lay::ConfigPage
{
Q_OBJECT

public:
  LayoutViewConfigPage7 (QWidget *parent);
  ~LayoutViewConfigPage7 ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::LayoutViewConfigPage7 *mp_ui;
};

}

#endif

