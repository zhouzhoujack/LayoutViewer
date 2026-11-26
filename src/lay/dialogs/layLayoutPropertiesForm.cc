#include "layLayoutPropertiesForm.h"
#include "tlException.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "detail/utilsExpression.h"
#include "layLayoutView.h"
#include "tlExceptions.h"
#include "layDialogs.h"
#include "tlInternational.h"
#include "tlString.h"
#include "tlExpression.h"

#include <set>
#include <sstream>

#include <QMessageBox>

namespace lay
{

// ------------------------------------------------------------

// -----------------------------------------------------------------------------
//  LayoutPropertiesForm implementation

LayoutPropertiesForm::LayoutPropertiesForm (QWidget *parent, lay::LayoutView *view, const char *name)
  : QDialog (parent), Ui::LayoutPropertiesForm ()
{
  m_editable = view->is_editable ();
  mp_view = view;

  setObjectName (QString::fromUtf8 (name));

  Ui::LayoutPropertiesForm::setupUi (this);

  //  collect the distinct layout handles 
  std::set <lay::LayoutHandle *> handles;
  for (unsigned int n = 0; n < view->cellviews (); ++n) {
    handles.insert (view->cellview (n).operator-> ());
  }
  
  m_handles.reserve (handles.size ());
  for (unsigned int n = 0; n < view->cellviews (); ++n) {
    lay::LayoutHandle *h = view->cellview (n).operator-> ();
    if (handles.find (h) != handles.end ()) {
      m_handles.push_back (h);
      handles.erase (h);
      layout_cbx->addItem (geo::to_qstring (h->name ()));
    }
  }

  m_index = -1;
  layout_cbx->setCurrentIndex (view->active_cellview_index ());

  connect (layout_cbx, SIGNAL (activated (int)), this, SLOT (layout_selected (int)));
  connect (prop_pb, SIGNAL (clicked ()), this, SLOT (prop_pb_clicked ()));

  dbu_le->setEnabled (m_editable);

  layout_selected (layout_cbx->currentIndex ());
}

void
LayoutPropertiesForm::accept ()
{
BEGIN_PROTECTED
  commit ();
  QDialog::accept ();
END_PROTECTED
}

void
LayoutPropertiesForm::commit ()
{
  if (m_index >= int (m_handles.size ()) || m_index < 0) {
    return;
  }

  geo::Layout &layout = m_handles [m_index]->layout ();

  //  get the database unit
  double dbu = 0.001;
  geo::from_string (geo::to_string (dbu_le->text ()), dbu);
  if (dbu < 1e-6 || dbu > 1e3) {
    throw tl::Exception (geo::to_string (QObject::tr ("Invalid database unit")));
  }

  if (fabs (dbu - layout.dbu ()) > 1e-6) {
    mp_view->manager ()->transaction (geo::to_string (QObject::tr ("Change layout's database unit"))); 
    layout.dbu (dbu);
    mp_view->manager ()->commit ();
  }

  //  get the selected technology name
  std::string technology;
  int technology_index = tech_cbx->currentIndex ();
  const geo::Technology *tech = 0;
  if (technology_index >= 0 && technology_index < (int) geo::Technologies::instance ()->technologies ()) {
    tech = &(geo::Technologies::instance ()->begin () [technology_index]);
    technology = tech->name ();
  }

  if (tech) {

    bool tech_has_changed = (technology != m_handles [m_index]->tech_name ());
    m_handles [m_index]->set_tech_name (technology);

    std::string lyp_file = tech->eff_layer_properties_file ();
    if (tech_has_changed && ! lyp_file.empty ()) {

      //  if the new technology has a layer properties file attached, ask whether to load it
      if (QMessageBox::question (this, QObject::tr ("Load Layer Properties File"), 
                                       geo::to_qstring (geo::to_string (QObject::tr ("The new technology specifies a layer properties file (")) + lyp_file + geo::to_string (QObject::tr (").\nLoad the new layer properties file?"))),
                                       QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {

        //  Interpolate the layer properties file name and load the file
        geo::Eval expr;
        expr.set_var ("layoutfile", m_handles [m_index]->filename ());
        lyp_file = expr.interpolate (lyp_file);
        mp_view->load_layer_props (lyp_file);

      }

    }

  }
}

void 
LayoutPropertiesForm::prop_pb_clicked ()
{
  if (m_index >= int (m_handles.size ()) || m_index < 0) {
    return;
  }

  geo::Layout &layout = m_handles [m_index]->layout ();
  geo::properties_id_type prop_id = layout.prop_id ();

  lay::UserPropertiesForm props_form (this);
  if (props_form.show (mp_view, m_index, prop_id)) {

    mp_view->manager ()->transaction (geo::to_string (QObject::tr ("Edit layout's user properties"))); 
    layout.prop_id (prop_id);
    mp_view->manager ()->commit ();

  }
}

void
LayoutPropertiesForm::layout_selected (int index)
{
BEGIN_PROTECTED
  if (index == m_index) {
    return;
  }

  if (m_index >= 0) {
    try {
      commit ();
    } catch (...) {
      layout_cbx->setCurrentIndex (m_index);
      throw;
    }
    m_index = -1;
  }

  if (index >= int (m_handles.size ()) || index < 0) {
    return;
  }

  m_index = index;

  const geo::Layout &layout = m_handles [index]->layout ();

  tech_cbx->clear ();
  unsigned int technology_index = 0;
  for (geo::Technologies::const_iterator t = geo::Technologies::instance ()->begin (); t != geo::Technologies::instance ()->end (); ++t, ++technology_index) {

    tech_cbx->addItem (geo::to_qstring (t->get_display_string ()));
    if (t->name () == m_handles [index]->tech_name ()) {
      tech_cbx->setCurrentIndex (technology_index);
    }

  }

  dbu_le->setText (geo::to_qstring (geo::to_string (layout.dbu ())));

END_PROTECTED
}

}



