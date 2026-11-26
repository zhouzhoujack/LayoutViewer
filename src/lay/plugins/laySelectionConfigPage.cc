#include "laySelectionConfigPage.h"

#include "layConfigPage.h"
#include "layDispatcher.h"
#include "layConverters.h"
#include "laySelectConfig.h"

#include "ui/ui_LayoutViewConfigPage2c.h"

namespace lay
{

// ------------------------------------------------------------
//  LayoutConfigPage2c implementation

LayoutViewConfigPage2c::LayoutViewConfigPage2c (QWidget *parent)
  : lay::ConfigPage (parent)
{
  mp_ui = new Ui::LayoutViewConfigPage2c ();
  mp_ui->setupUi (this);
}

LayoutViewConfigPage2c::~LayoutViewConfigPage2c ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
LayoutViewConfigPage2c::setup (lay::Dispatcher *root)
{
  QColor color;
  root->config_get (cfg_sel_color, color, lay::ColorConverter ());
  mp_ui->color_pb->set_color (color);

  int lw = 0;
  root->config_get (cfg_sel_line_width, lw);
  mp_ui->lw_spinbx->setValue (lw);

  int vs = 0;
  root->config_get (cfg_sel_vertex_size, vs);
  mp_ui->vs_spinbx->setValue (vs);

  int dp = 0;
  root->config_get (cfg_sel_dither_pattern, dp);
  mp_ui->stipple_pb->set_dither_pattern (dp);

  bool halo = 0;
  root->config_get (cfg_sel_halo, halo);
  mp_ui->halo_cb->setChecked (halo);

  bool tm = 0;
  root->config_get (cfg_sel_transient_mode, tm);
  mp_ui->transient_mode_cb->setChecked (tm);

  bool ipm = 0;
  root->config_get (cfg_sel_inside_pcells_mode, ipm);
  mp_ui->sel_inside_pcells_cb->setChecked (ipm);

  unsigned int sr = 0;
  root->config_get (cfg_search_range, sr);
  mp_ui->search_range_spinbx->setValue (sr);

  unsigned int srbox = 0;
  root->config_get (cfg_search_range_box, srbox);
  mp_ui->search_range_box_spinbx->setValue (srbox);
}

void 
LayoutViewConfigPage2c::commit (lay::Dispatcher *root)
{
  lay::ColorConverter cc;
  root->config_set (cfg_sel_color, mp_ui->color_pb->get_color (), cc);
  root->config_set (cfg_sel_line_width, mp_ui->lw_spinbx->value ());
  root->config_set (cfg_sel_vertex_size, mp_ui->vs_spinbx->value ());
  root->config_set (cfg_sel_dither_pattern, mp_ui->stipple_pb->dither_pattern ());
  root->config_set (cfg_sel_halo, mp_ui->halo_cb->isChecked ());
  root->config_set (cfg_sel_transient_mode, mp_ui->transient_mode_cb->isChecked ());
  root->config_set (cfg_sel_inside_pcells_mode, mp_ui->sel_inside_pcells_cb->isChecked ());
  root->config_set (cfg_search_range, (unsigned int) mp_ui->search_range_spinbx->value ());
  root->config_set (cfg_search_range_box, (unsigned int) mp_ui->search_range_box_spinbx->value ());
}

}   // namespace lay
