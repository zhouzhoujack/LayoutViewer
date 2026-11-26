#ifndef HDR_edtPropertiesPages
#define HDR_edtPropertiesPages

#include "layPlugin.h"
#include "layProperties.h"
#include "layEdtService.h"
#include "ui_PolygonPropertiesPage.h"
#include "ui_BoxPropertiesPage.h"
#include "ui_PathPropertiesPage.h"
#include "ui_EditablePathPropertiesPage.h"
#include "ui_TextPropertiesPage.h"

namespace edt
{

class ChangeApplicator;

class ShapePropertiesPage
  : public lay::PropertiesPage
{
Q_OBJECT

public:
  ShapePropertiesPage (edt::Service *service, geo::Manager *manager, QWidget *parent);
  ~ShapePropertiesPage ();

  virtual void back ();
  virtual void front ();
  virtual bool at_begin () const;
  virtual bool at_end () const;
  virtual void operator-- ();
  virtual void operator++ ();
  virtual void leave ();

protected:
  virtual bool readonly ();

private:
  virtual void update ();
  virtual void apply ();
  virtual void apply_to_all (bool relative);
  virtual bool can_apply_to_all () const;
  virtual void do_apply (bool current_only, bool relative);
  void recompute_selection_ptrs (const std::vector<lay::ObjectInstPath> &new_sel);

protected:
  std::vector<edt::Service::obj_iterator> m_selection_ptrs;
  unsigned int m_index;
  edt::Service *mp_service;
  bool m_enable_cb_callback;
  geo::properties_id_type m_prop_id;

  virtual void do_update (const geo::Shape &shape, double dbu, const std::string &lname) = 0;
  virtual ChangeApplicator *create_applicator (geo::Shapes &shapes, const geo::Shape &shape, double dbu) = 0;
  virtual QCheckBox *dbu_checkbox () const = 0;
  virtual QCheckBox *abs_checkbox () const = 0;
  bool dbu_units () const;
  bool abs_trans () const;
  geo::ICplxTrans trans () const;
  void setup ();

public slots:
  void show_inst ();
  void show_props ();
  void display_mode_changed (bool);
  void update_shape ();
};


class PolygonPropertiesPage
  : public ShapePropertiesPage,
    public Ui::PolygonPropertiesPage
{
Q_OBJECT

public:
  PolygonPropertiesPage (edt::Service *service, geo::Manager *manager, QWidget *parent);

  virtual void do_update (const geo::Shape &shape, double dbu, const std::string &lname);
  virtual ChangeApplicator *create_applicator (geo::Shapes &shapes, const geo::Shape &shape, double dbu);

protected:
  virtual QCheckBox *dbu_checkbox () const { return dbu_cb; }
  virtual QCheckBox *abs_checkbox () const { return abs_cb; }

public slots:
  void text_changed ();

private:
  bool m_in_text_changed;
};

class BoxPropertiesPage
  : public ShapePropertiesPage,
    public Ui::BoxPropertiesPage
{
Q_OBJECT

public:
  BoxPropertiesPage (edt::Service *service, geo::Manager *manager, QWidget *parent);

  virtual void do_update (const geo::Shape &shape, double dbu, const std::string &lname);
  virtual ChangeApplicator *create_applicator (geo::Shapes &shapes, const geo::Shape &shape, double dbu);

public slots:
  void changed ();

protected:
  virtual QCheckBox *dbu_checkbox () const { return dbu_cb; }
  virtual QCheckBox *abs_checkbox () const { return abs_cb; }

private:
  bool m_recursion_sentinel;
  int m_tab_index;
  double m_dbu;
  mutable bool m_lr_swapped, m_tb_swapped;

  geo::Box get_box (int mode) const;
  void set_box (const geo::Box &box);
};

class TextPropertiesPage
  : public ShapePropertiesPage,
    public Ui::TextPropertiesPage
{
Q_OBJECT

public:
  TextPropertiesPage (edt::Service *service, geo::Manager *manager, QWidget *parent);

  virtual void do_update (const geo::Shape &shape, double dbu, const std::string &lname);
  virtual ChangeApplicator *create_applicator (geo::Shapes &shapes, const geo::Shape &shape, double dbu);

protected:
  virtual QCheckBox *dbu_checkbox () const { return dbu_cb; }
  virtual QCheckBox *abs_checkbox () const { return abs_cb; }
};

class PathPropertiesPage
  : public ShapePropertiesPage,
    public Ui::PathPropertiesPage
{
Q_OBJECT

public:
  PathPropertiesPage (edt::Service *service, geo::Manager *manager, QWidget *parent);

  virtual void do_update (const geo::Shape &shape, double dbu, const std::string &lname);
  virtual ChangeApplicator *create_applicator (geo::Shapes &shapes, const geo::Shape &shape, double dbu);

protected:
  virtual QCheckBox *dbu_checkbox () const { return dbu_cb; }
  virtual QCheckBox *abs_checkbox () const { return abs_cb; }

private:
  bool m_in_text_changed;
};

class EditablePathPropertiesPage
  : public ShapePropertiesPage,
    public Ui::EditablePathPropertiesPage
{
Q_OBJECT

public:
  EditablePathPropertiesPage (edt::Service *service, geo::Manager *manager, QWidget *parent);

  virtual void do_update (const geo::Shape &shape, double dbu, const std::string &lname);
  virtual ChangeApplicator *create_applicator (geo::Shapes &shapes, const geo::Shape &shape, double dbu);

protected:
  virtual QCheckBox *dbu_checkbox () const { return dbu_cb; }
  virtual QCheckBox *abs_checkbox () const { return abs_cb; }

public slots:
  void type_selected (int); 
  void text_changed ();

private:
  bool m_in_text_changed;
};

}

#endif
