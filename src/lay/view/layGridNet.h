#ifndef HDR_layGridNet
#define HDR_layGridNet

#include "layViewObject.h"
#include "layPlugin.h"
#include "detail/geoTypes.h"
#include "layPluginDeclaration.h"
#include "geoBox.h"
#include "layConfigPage.h"

namespace Ui {
  class GridNetConfigPage;
}

namespace lay {

class LayoutView;
class ColorButton;

class GridNetPluginDeclaration
  : public lay::PluginDeclaration
{
public:
    GridNetPluginDeclaration(int id):
        lay::PluginDeclaration(id){

    }

  virtual void get_options (std::vector < std::pair<std::string, std::string> > &options) const;
  virtual lay::ConfigPage *config_page (QWidget *parent, std::string &title) const;
  virtual lay::Plugin *create_plugin (geo::Manager*, lay::Dispatcher*, lay::LayoutView*,
                                     lay::ResourceTree*) const;
};

class GridNetConfigPage 
  : public lay::ConfigPage
{
Q_OBJECT

public:
  GridNetConfigPage (QWidget *parent);
  ~GridNetConfigPage ();

  virtual void setup (lay::Dispatcher *root);
  virtual void commit (lay::Dispatcher *root);

private:
  Ui::GridNetConfigPage *mp_ui;
  lay::ColorButton *mp_grid_color_cbtn;
  lay::ColorButton *mp_grid_grid_color_cbtn;
  lay::ColorButton *mp_grid_axis_color_cbtn;
  lay::ColorButton *mp_grid_ruler_color_cbtn;
};

class GridNet
  : public lay::BackgroundViewObject,
    public lay::Plugin
{
public: 
  enum GridStyle {
    Invisible = 0, 
    //  dot styles:
    Dots, DottedLines, LightDottedLines, TenthDottedLines,  
    //  line styles:
    Crosses, Lines, TenthMarkedLines, 
    //  others
    CheckerBoard
  };

  GridNet (lay::LayoutView *view);

private:
  virtual void render_bg (const lay::Viewport &vp, lay::ViewObjectCanvas &canvas);

  //  implementation of the lay::Plugin interface
  virtual bool configure (const std::string &name, const std::string &value);

  lay::LayoutView *mp_view;
  bool m_visible;
  bool m_show_ruler;
  double m_grid;
  QColor m_color;
  QColor m_grid_color;
  QColor m_axis_color;
  QColor m_ruler_color;
  GridStyle m_style0;
  GridStyle m_style1;
  GridStyle m_style2;
};

}

#endif

