#ifndef HDR_rdbMarkerBrowser
#define HDR_rdbMarkerBrowser

#include "layPlugin.h"
#include "ui_MarkerBrowserConfigPage.h"
#include "ui_MarkerBrowserConfigPage2.h"

#include "dbTrans.h"
#include "layConfigPage.h"

#include <algorithm>

namespace rdb
{

enum context_mode_type { AnyCell = 0 , DatabaseTop, Current, CurrentOrAny, Local };
enum window_type { DontChange = 0, FitCell, FitMarker, Center, CenterSize };

class MarkerBrowserConfigPage
  : public lay::ConfigPage,
    private Ui::MarkerBrowserConfigPage
{
  Q_OBJECT 

public:
  MarkerBrowserConfigPage (QWidget *parent);

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

public slots:
  void window_changed (int);
};

class MarkerBrowserConfigPage2
  : public lay::ConfigPage,
    private Ui::MarkerBrowserConfigPage2
{
  Q_OBJECT 

public:
  MarkerBrowserConfigPage2 (QWidget *parent);

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);
};

class MarkerBrowserContextModeConverter
{
public:
  void from_string (const std::string &value, rdb::context_mode_type &mode);
  std::string to_string (rdb::context_mode_type mode);
};

class MarkerBrowserWindowModeConverter
{
public:
  void from_string (const std::string &value, rdb::window_type &mode);
  std::string to_string (rdb::window_type mode);
};

}

#endif

