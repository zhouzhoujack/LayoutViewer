#ifndef HDR_antPropertiesPage
#define HDR_antPropertiesPage

#include "layPlugin.h"
#include "layProperties.h"
#include "layRulerPlugin.h"
#include "ui_RulerPropertiesPage.h"

namespace lay
{

class RulerPropertiesPage
  : public lay::PropertiesPage,
    public Ui::RulerPropertiesPage
{
Q_OBJECT

public:
  RulerPropertiesPage (RulerPlugin *rulers, geo::Manager *manager, QWidget *parent);
  ~RulerPropertiesPage ();

  virtual void back ();
  virtual void front ();
  virtual bool at_begin () const;
  virtual bool at_end () const;
  virtual void operator-- ();
  virtual void operator++ ();
  virtual void update ();
  virtual void leave ();
  virtual bool readonly ();
  virtual void apply (); 

private slots:
  void swap_points_clicked ();
  void snap_to_layout_clicked ();

private:
  std::vector <RulerPlugin::obj_iterator> m_selection;
  std::vector <RulerPlugin::obj_iterator>::iterator m_pos;
  RulerPlugin *mp_rulers;
  bool m_enable_cb_callback;

  const RulerObject &current () const;
  void get_points(geo::DPoint &p1, geo::DPoint &p2);
};

}

#endif
