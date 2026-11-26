#include "layRulerConfigPage.h"
#include "tlException.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"

#include "ui_RulerConfigPage.h"
#include "ui_RulerConfigPage2.h"
#include "ui_RulerConfigPage3.h"
#include "ui_RulerConfigPage4.h"

#include "layRulerConfig.h"
#include "layConverters.h"
#include "layDispatcher.h"
#include "layQtTools.h"

#include "tlExceptions.h"

#include <QInputDialog>

namespace lay
{

// ------------------------------------------------------------
//  Implementation of the configuration page

RulerConfigPage::RulerConfigPage (QWidget *parent)
  : lay::ConfigPage (parent)
{
  mp_ui = new Ui::RulerConfigPage ();
  mp_ui->setupUi (this);
}

RulerConfigPage::~RulerConfigPage ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
RulerConfigPage::setup (lay::Dispatcher *root)
{
  //  Snap range
  int snap_range = 0;
  root->config_get (cfg_ruler_snap_range, snap_range);
  mp_ui->ruler_snap_range_edit->setText (geo::to_qstring (geo::to_string (snap_range)));

  //  object and grid snap
  bool f = false;
  root->config_get (cfg_ruler_obj_snap, f);
  mp_ui->ruler_obj_snap_cbx->setChecked (f);
  root->config_get (cfg_ruler_grid_snap, f);
  mp_ui->ruler_grid_snap_cbx->setChecked (f);
}

void 
RulerConfigPage::commit (lay::Dispatcher *root)
{
  root->config_set (cfg_ruler_obj_snap, mp_ui->ruler_obj_snap_cbx->isChecked ());
  root->config_set (cfg_ruler_grid_snap, mp_ui->ruler_grid_snap_cbx->isChecked ());

  int sr = 0;
  geo::from_string (geo::to_string (mp_ui->ruler_snap_range_edit->text ()), sr);
  if (sr < 1 || sr > 1000) {
    throw tl::Exception (geo::to_string (QObject::tr ("Not a valid pixel value (must be non-zero positive and not too large): %s")), geo::to_string (mp_ui->ruler_snap_range_edit->text ()));
  }
  root->config_set (cfg_ruler_snap_range, sr);
}

// ------------------------------------------------------------
//  Implementation of the configuration page 2

RulerConfigPage2::RulerConfigPage2 (QWidget *parent)
  : lay::ConfigPage (parent)
{
  mp_ui = new Ui::RulerConfigPage2 ();
  mp_ui->setupUi (this);
}

RulerConfigPage2::~RulerConfigPage2 ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
RulerConfigPage2::setup (lay::Dispatcher *root)
{
  //  Max. number of rulers
  int max_number_of_rulers = -1;
  root->config_get (cfg_max_number_of_rulers, max_number_of_rulers);
  if (max_number_of_rulers < 0) {
    mp_ui->num_rulers_edit->setText (QString ());
  } else {
    mp_ui->num_rulers_edit->setText (geo::to_qstring (geo::to_string (max_number_of_rulers)));
  }

  //  color
  QColor color;
  root->config_get (cfg_ruler_color, color, lay::ColorConverter ());
  mp_ui->ruler_color_pb->set_color (color);

  //  halo flag
  bool halo = true;
  root->config_get (cfg_ruler_halo, halo);
  mp_ui->halo_cb->setChecked (halo);
}

void 
RulerConfigPage2::commit (lay::Dispatcher *root)
{
  int mr;
  try {
    geo::from_string (geo::to_string (mp_ui->num_rulers_edit->text ()), mr);
  } catch (...) {
    mr = -1;
  }
  root->config_set (cfg_max_number_of_rulers, mr);

  root->config_set (cfg_ruler_color, mp_ui->ruler_color_pb->get_color (), lay::ColorConverter ());
  root->config_set (cfg_ruler_halo, mp_ui->halo_cb->isChecked ());
}

// ------------------------------------------------------------
//  Implementation of the configuration page 3

RulerConfigPage3::RulerConfigPage3 (QWidget *parent)
  : lay::ConfigPage (parent)
{
  mp_ui = new Ui::RulerConfigPage3 ();
  mp_ui->setupUi (this);
}

RulerConfigPage3::~RulerConfigPage3 ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
RulerConfigPage3::setup (lay::Dispatcher *root)
{
  //  snap mode
  lay::angle_constraint_type rm = lay::AC_Any;
  root->config_get (cfg_ruler_snap_mode, rm, ACConverter ());
  mp_ui->ruler_any_angle_rb->setChecked (rm == lay::AC_Any);
  mp_ui->ruler_ortho_rb->setChecked (rm == lay::AC_Ortho);
  mp_ui->ruler_diag_rb->setChecked (rm == lay::AC_Diagonal);
  mp_ui->ruler_hor_rb->setChecked (rm == lay::AC_Horizontal);
  mp_ui->ruler_vert_rb->setChecked (rm == lay::AC_Vertical);
}

void 
RulerConfigPage3::commit (lay::Dispatcher *root)
{
  lay::angle_constraint_type rm = lay::AC_Any;
  if (mp_ui->ruler_any_angle_rb->isChecked ()) {
    rm = lay::AC_Any;
  }
  if (mp_ui->ruler_ortho_rb->isChecked ()) {
    rm = lay::AC_Ortho;
  }
  if (mp_ui->ruler_diag_rb->isChecked ()) {
    rm = lay::AC_Diagonal;
  }
  if (mp_ui->ruler_hor_rb->isChecked ()) {
    rm = lay::AC_Horizontal;
  }
  if (mp_ui->ruler_vert_rb->isChecked ()) {
    rm = lay::AC_Vertical;
  }
  root->config_set (cfg_ruler_snap_mode, rm, ACConverter ());
}

// ------------------------------------------------------------
//  Implementation of the configuration page 4

RulerConfigPage4::RulerConfigPage4 (QWidget *parent)
  : lay::ConfigPage (parent),
    m_current_template (0),
    m_current_changed_enabled (true)
{
  mp_ui = new Ui::RulerConfigPage4 ();
  mp_ui->setupUi (this);

  connect (mp_ui->add_templ_pb, SIGNAL (clicked ()), this, SLOT (add_clicked ()));
  connect (mp_ui->del_templ_pb, SIGNAL (clicked ()), this, SLOT (del_clicked ()));
  connect (mp_ui->up_templ_pb, SIGNAL (clicked ()), this, SLOT (up_clicked ()));
  connect (mp_ui->down_templ_pb, SIGNAL (clicked ()), this, SLOT (down_clicked ()));
  connect (mp_ui->template_list, SIGNAL (currentRowChanged (int)), this, SLOT (current_template_changed (int)));
  connect (mp_ui->template_list, SIGNAL (itemDoubleClicked (QListWidgetItem *)), this, SLOT (double_clicked (QListWidgetItem *)));

  lay::activate_help_links (mp_ui->help_label);
}

RulerConfigPage4::~RulerConfigPage4 ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
RulerConfigPage4::setup (lay::Dispatcher *root)
{
  //  templates
  root->config_get (cfg_ruler_templates, m_ruler_templates, TemplatesConverter ());
  m_current_template = 0;
  root->config_get (cfg_current_ruler_template, m_current_template);
  
  //  add one template if the current index is not pointing to a valid one
  if (m_current_template < 0) {
    m_current_template = 0;
  }
  if (m_current_template >= int (m_ruler_templates.size ())) {
    m_current_template = int (m_ruler_templates.size ());
    m_ruler_templates.push_back (Template ());
  }
  
  update_list ();
  show ();
}

void 
RulerConfigPage4::commit (lay::Dispatcher *root)
{
  commit ();
  
  //  templates
  root->config_set (cfg_ruler_templates, m_ruler_templates, TemplatesConverter ());
  root->config_set (cfg_current_ruler_template, m_current_template);
}

void 
RulerConfigPage4::add_clicked ()
{
  commit ();
  Template new_one;
  if (m_current_template < 0 || m_current_template >= int (m_ruler_templates.size ())) {
    m_current_template = int (m_ruler_templates.size ());
  } else {
    new_one = m_ruler_templates [m_current_template];
  }
  new_one.category (std::string ());
  m_ruler_templates.insert (m_ruler_templates.begin () + m_current_template, new_one);
  m_ruler_templates [m_current_template].title (geo::to_string (QObject::tr ("New Ruler")));
  update_list ();
  show ();
  double_clicked (0); // to edit the name
}

void  
RulerConfigPage4::del_clicked ()
{
BEGIN_PROTECTED

  if (m_current_template >= 0 && m_current_template < int (m_ruler_templates.size ())) {
    if (! m_ruler_templates [m_current_template].category ().empty ()) {
      throw tl::Exception (geo::to_string (tr ("This ruler is a built-in template and cannot be deleted")));
    }
    m_ruler_templates.erase (m_ruler_templates.begin () + m_current_template);
    if (m_current_template > 0) {
      --m_current_template;
    }
    if (m_ruler_templates.empty ()) {
      m_ruler_templates.push_back (Template ());
      m_current_template = 0;
    }
    update_list ();
    show ();
  }

END_PROTECTED
}

void  
RulerConfigPage4::up_clicked ()
{
  if (m_current_template > 0) {
    commit ();
    std::swap (m_ruler_templates [m_current_template], m_ruler_templates [m_current_template - 1]);
    --m_current_template;
    update_list ();
    show ();
  }
}

void  
RulerConfigPage4::down_clicked ()
{
  if (m_current_template >= 0 && m_current_template < int (m_ruler_templates.size () - 1)) {
    commit ();
    std::swap (m_ruler_templates [m_current_template], m_ruler_templates [m_current_template + 1]);
    ++m_current_template;
    update_list ();
    show ();
  }
}

void 
RulerConfigPage4::update_list ()
{
  m_current_changed_enabled = false;
  mp_ui->template_list->clear ();
  for (std::vector <Template>::const_iterator t = m_ruler_templates.begin (); t != m_ruler_templates.end (); ++t) {
    mp_ui->template_list->addItem (geo::to_qstring (t->title ()));
    if (! t->category ().empty ()) {
      QListWidgetItem *item = mp_ui->template_list->item (int (t - m_ruler_templates.begin ()));
      QFont font = item->font ();
      font.setItalic (true);
      item->setFont (font);
    }
  }
  mp_ui->template_list->setCurrentRow (m_current_template);
  m_current_changed_enabled = true;
}

void  
RulerConfigPage4::current_template_changed (int index)
{
  if (m_current_changed_enabled) {
    commit ();
    m_current_template = index;
    show ();
  }
}

void   
RulerConfigPage4::double_clicked (QListWidgetItem *)
{
  if (m_current_template >= 0 && m_current_template < int (m_ruler_templates.size ())) {
    commit ();
    bool ok = false;
    QString new_title = QInputDialog::getText (this, 
                                               QObject::tr ("Enter New Title"),
                                               QObject::tr ("New Title"),
                                               QLineEdit::Normal,
                                               geo::to_qstring (m_ruler_templates [m_current_template].title ()),
                                               &ok);
    if (ok) {
      m_ruler_templates [m_current_template].title (geo::to_string (new_title));
      update_list ();
      show ();
    }
  }
}

void 
RulerConfigPage4::show ()
{
  mp_ui->fmt_le->setText (geo::to_qstring (m_ruler_templates [m_current_template].fmt ()));
  mp_ui->fmt_x_le->setText (geo::to_qstring (m_ruler_templates [m_current_template].fmt_x ()));
  mp_ui->fmt_y_le->setText (geo::to_qstring (m_ruler_templates [m_current_template].fmt_y ()));
  
  mp_ui->style_cb->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].style ());
  mp_ui->outline_cb->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].outline ());
  mp_ui->t_angle_cb->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].angle_constraint ());
  mp_ui->t_snap_cbx->setChecked (m_ruler_templates [m_current_template].snap ());
  mp_ui->t_mode_cb->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].mode ());

  mp_ui->main_position->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].main_position ());
  mp_ui->main_xalign->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].main_xalign ());
  mp_ui->main_yalign->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].main_yalign ());
  mp_ui->xlabel_xalign->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].xlabel_xalign ());
  mp_ui->xlabel_yalign->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].xlabel_yalign ());
  mp_ui->ylabel_xalign->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].ylabel_xalign ());
  mp_ui->ylabel_yalign->setCurrentIndex ((unsigned int) m_ruler_templates [m_current_template].ylabel_yalign ());
}

void   
RulerConfigPage4::commit ()
{
  std::string fmt, fmt_x, fmt_y;
  fmt = geo::to_string (mp_ui->fmt_le->text ());
  fmt_x = geo::to_string (mp_ui->fmt_x_le->text ());
  fmt_y = geo::to_string (mp_ui->fmt_y_le->text ());
  m_ruler_templates [m_current_template].fmt (fmt);
  m_ruler_templates [m_current_template].fmt_x (fmt_x);
  m_ruler_templates [m_current_template].fmt_y (fmt_y);

  RulerObject::style_type style = RulerObject::style_type (mp_ui->style_cb->currentIndex ());
  m_ruler_templates [m_current_template].style (style);
  
  RulerObject::outline_type outline = RulerObject::outline_type (mp_ui->outline_cb->currentIndex ());
  m_ruler_templates [m_current_template].outline (outline);
  
  lay::angle_constraint_type ac = lay::angle_constraint_type (mp_ui->t_angle_cb->currentIndex ());
  m_ruler_templates [m_current_template].angle_constraint (ac);

  Template::ruler_mode_type mode = Template::ruler_mode_type (mp_ui->t_mode_cb->currentIndex ());
  m_ruler_templates [m_current_template].set_mode (mode);

  m_ruler_templates [m_current_template].snap (mp_ui->t_snap_cbx->isChecked ());

  m_ruler_templates [m_current_template].set_main_position (RulerObject::position_type (mp_ui->main_position->currentIndex ()));
  m_ruler_templates [m_current_template].set_main_xalign (RulerObject::alignment_type (mp_ui->main_xalign->currentIndex ()));
  m_ruler_templates [m_current_template].set_main_yalign (RulerObject::alignment_type (mp_ui->main_yalign->currentIndex ()));
  m_ruler_templates [m_current_template].set_xlabel_xalign (RulerObject::alignment_type (mp_ui->xlabel_xalign->currentIndex ()));
  m_ruler_templates [m_current_template].set_xlabel_yalign (RulerObject::alignment_type (mp_ui->xlabel_yalign->currentIndex ()));
  m_ruler_templates [m_current_template].set_ylabel_xalign (RulerObject::alignment_type (mp_ui->ylabel_xalign->currentIndex ()));
  m_ruler_templates [m_current_template].set_ylabel_yalign (RulerObject::alignment_type (mp_ui->ylabel_yalign->currentIndex ()));
}

} // namespace ant
