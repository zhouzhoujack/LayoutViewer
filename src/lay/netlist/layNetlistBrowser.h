#ifndef HDR_layNetlistBrowser
#define HDR_layNetlistBrowser

#include "layPlugin.h"
#include "layColorPalette.h"
#include "ui_NetlistBrowserConfigPage.h"
#include "ui_NetlistBrowserConfigPage2.h"

#include "dbTrans.h"
#include "layConfigPage.h"

#include <algorithm>

namespace lay
{

struct NetlistBrowserConfig
{
  enum net_window_type { DontChange = 0, FitNet, Center, CenterSize };
};

class NetlistBrowserConfigPage
  : public lay::ConfigPage,
    private Ui::NetlistBrowserConfigPage
{
  Q_OBJECT

public:
  NetlistBrowserConfigPage (QWidget *parent);

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

public slots:
  void window_changed (int);
};

class NetlistBrowserConfigPage2
  : public lay::ConfigPage,
    private Ui::NetlistBrowserConfigPage2
{
  Q_OBJECT

public:
  NetlistBrowserConfigPage2 (QWidget *parent);

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

public slots:
  void color_button_clicked ();

private:
  void update_colors ();

  lay::ColorPalette m_palette;
};

class NetlistBrowserWindowModeConverter
{
public:
  void from_string (const std::string &value, lay::NetlistBrowserConfig::net_window_type &mode);
  std::string to_string (lay::NetlistBrowserConfig::net_window_type mode);
};

}

#endif

