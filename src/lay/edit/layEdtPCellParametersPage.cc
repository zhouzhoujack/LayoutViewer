#include "layEdtPCellParametersPage.h"
#include "tlException.h"
#include "detail/utilsInternational.h"
#include "detail/utilsString.h"
#include "detail/utilsVariant.h"
#include "layEdtPropertiesPageUtils.h"
#include "layWidgets.h"
#include "layQtTools.h"
#include "layLayoutView.h"
#include "tlScriptError.h"

#include <QtWidgets/QFrame>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QGroupBox>
#include <QScrollBar>

namespace edt
{

static void set_value (const geo::PCellParameterDeclaration &p, QWidget *widget, const geo::Variant &value)
{
  if (p.get_choices ().empty ()) {

    switch (p.get_type ()) {
      
    case geo::PCellParameterDeclaration::t_int:
      {
        QLineEdit *le = dynamic_cast<QLineEdit *> (widget);
        if (le) {
          le->blockSignals (true);
          le->setText (value.cast<int> ().to_qstring ());
          le->blockSignals (false);
        }
      }
      break;

    case geo::PCellParameterDeclaration::t_double:
      {
        QLineEdit *le = dynamic_cast<QLineEdit *> (widget);
        if (le) {
          le->blockSignals (true);
          le->setText (value.cast<double> ().to_qstring ());
          le->blockSignals (false);
        }
      }
      break;

    case geo::PCellParameterDeclaration::t_string:
      {
        QLineEdit *le = dynamic_cast<QLineEdit *> (widget);
        if (le) {
          le->blockSignals (true);
          le->setText (value.to_qstring ());
          le->blockSignals (false);
        }
      }
      break;

    case geo::PCellParameterDeclaration::t_list:
      {
        QLineEdit *le = dynamic_cast<QLineEdit *> (widget);
        if (le) {
          le->blockSignals (true);
          le->setText (value.to_qstring ());
          le->blockSignals (false);
        }
      }
      break;

    case geo::PCellParameterDeclaration::t_layer:
      {
        lay::LayerSelectionComboBox *ly = dynamic_cast<lay::LayerSelectionComboBox *> (widget);
        if (ly) {

          geo::LayerProperties lp;
          if (value.is_user<geo::LayerProperties> ()) {
            lp = value.to_user<geo::LayerProperties> ();
          } else {
            std::string s = value.to_string ();
            geo::Extractor ex (s.c_str ());
            lp.read (ex);
          }

          ly->blockSignals (true);
          ly->set_current_layer (lp);
          ly->blockSignals (false);

        }
      }
      break;

    case geo::PCellParameterDeclaration::t_boolean:
      {
        QCheckBox *cbx = dynamic_cast<QCheckBox *> (widget);
        if (cbx) {
          cbx->blockSignals (true);
          cbx->setChecked (value.to_bool ());
          cbx->blockSignals (false);
        }
      }
      break;

    default:
      break;
    }

  } else {

    QComboBox *cb = dynamic_cast<QComboBox *> (widget);
    if (cb) {
      int i = 0;
      for (std::vector<geo::Variant>::const_iterator c = p.get_choices ().begin (); c != p.get_choices ().end (); ++c, ++i) {
        if (*c == value) {
          cb->blockSignals (true);
          cb->setCurrentIndex (i);
          cb->blockSignals (false);
        }
      }
    }

  }
}

// -----------------------------------------------------------------------------
//  EdtPCellParametersPage implementation

PCellParametersPage::PCellParametersPage (QWidget *parent, bool dense)
  : QFrame (parent), m_dense (dense), dm_parameter_changed (this, &PCellParametersPage::do_parameter_changed)
{
  init ();
}

void
PCellParametersPage::init ()
{
  mp_pcell_decl.reset (0);
  mp_view = 0;
  m_cv_index = 0;
  mp_parameters_area = 0;

  QGridLayout *frame_layout = new QGridLayout (this);
  //  spacing and margin for tool windows
  frame_layout->setMargin (0);
  setLayout (frame_layout);

  mp_error_icon = new QLabel (this);
  mp_error_icon->setPixmap (QPixmap (":/warn.png"));
  mp_error_icon->hide ();
  frame_layout->addWidget (mp_error_icon, 1, 0, 1, 1);

  mp_error_label = new QLabel (this);
  mp_error_label->setWordWrap (true);
  QPalette palette = mp_error_label->palette ();
  palette.setColor (QPalette::Foreground, Qt::red);
  mp_error_label->setPalette (palette);
  QFont font = mp_error_label->font ();
  font.setBold (true);
  mp_error_label->setFont (font);
  mp_error_label->hide ();
  frame_layout->addWidget (mp_error_label, 1, 1, 1, 1);
  frame_layout->setColumnStretch (1, 1);
}

void
PCellParametersPage::setup (lay::LayoutView *view, int cv_index, const geo::PCellDeclaration *pcell_decl, const geo::pcell_parameters_type &parameters)
{
  mp_pcell_decl.reset (const_cast<geo::PCellDeclaration *> (pcell_decl));  //  no const weak_ptr ...
  mp_view = view;
  m_cv_index = cv_index;
  m_parameters = parameters;

  if (mp_parameters_area) {
    delete mp_parameters_area;
  }

  m_widgets.clear ();

  mp_parameters_area = new QScrollArea (this);
  mp_parameters_area->setFrameShape (QFrame::NoFrame);
  QGridLayout *frame_layout = dynamic_cast<QGridLayout *> (QFrame::layout ());
  frame_layout->addWidget (mp_parameters_area, 0, 0, 1, 2);
  frame_layout->setRowStretch (0, 1);

  QFrame *fi = new QFrame (mp_parameters_area);
  QWidget *inner_frame = fi;
  fi->setFrameShape (QFrame::NoFrame);
  setFrameShape (QFrame::NoFrame);

  QGridLayout *inner_grid = new QGridLayout (inner_frame);
  inner_frame->setLayout (inner_grid);
  if (m_dense) {
    inner_grid->setMargin (4);
    inner_grid->setHorizontalSpacing (6);
    inner_grid->setVerticalSpacing (2);
  }

  QWidget *main_frame = inner_frame;
  QGridLayout *main_grid = inner_grid;

  int main_row = 0;
  int row = 0;
  std::string group_title;

  int r = 0;
  const std::vector<geo::PCellParameterDeclaration> &pcp = pcell_decl->parameter_declarations ();
  for (std::vector<geo::PCellParameterDeclaration>::const_iterator p = pcp.begin (); p != pcp.end (); ++p, ++r) {

    if (p->is_hidden () || p->get_type () == geo::PCellParameterDeclaration::t_shape) {
      m_widgets.push_back (0);
      continue;
    }

    std::string gt, description;
    size_t tab = p->get_description ().find ("\t");
    if (tab != std::string::npos) {
      gt = std::string (p->get_description (), 0, tab);
      description = std::string (p->get_description (), tab + 1, std::string::npos);
    } else {
      description = p->get_description ();
    }

    if (gt != group_title) {

      if (! gt.empty ()) {

        //  create a new group
        QGroupBox *gb = new QGroupBox (main_frame);
        gb->setTitle (geo::to_qstring (gt));
        main_grid->addWidget (gb, main_row, 0, 1, 2);
        
        inner_grid = new QGridLayout (gb);
        if (m_dense) {
          inner_grid->setMargin (4);
          inner_grid->setHorizontalSpacing (6);
          inner_grid->setVerticalSpacing (2);
        }
        gb->setLayout (inner_grid);
        inner_frame = gb;

        row = 0;
        ++main_row;

      } else {

        //  back to the main group
        inner_grid = main_grid;
        inner_frame = main_frame;
        row = main_row;

      }

      group_title = gt;

    } 

    inner_grid->addWidget (new QLabel (geo::to_qstring (description), inner_frame), row, 0);

    geo::Variant value;
    if (r < int (parameters.size ())) {
      value = parameters [r];
    } else {
      value = p->get_default ();
    }

    if (p->get_choices ().empty ()) {

      switch (p->get_type ()) {
        
      case geo::PCellParameterDeclaration::t_int:
      case geo::PCellParameterDeclaration::t_double:
        {
          QFrame *f = new QFrame (inner_frame);
          QHBoxLayout *hb = new QHBoxLayout (f);
          hb->setMargin (0);
          f->setLayout (hb);
          f->setFrameShape (QFrame::NoFrame);

          QLineEdit *le = new QLineEdit (f);
          le->setEnabled (! p->is_readonly ());
          hb->addWidget (le);
          le->setMaximumWidth (150);
          le->setObjectName (geo::to_qstring (p->get_name ()));
          m_widgets.push_back (le);

          QLabel *ul = new QLabel (f);
          hb->addWidget (ul, 1);
          ul->setText (geo::to_qstring (p->get_unit ()));

          inner_grid->addWidget (f, row, 1);

          connect (le, SIGNAL (editingFinished ()), this, SLOT (parameter_changed ()));
        }
        break;

      case geo::PCellParameterDeclaration::t_string:
      case geo::PCellParameterDeclaration::t_shape:
      case geo::PCellParameterDeclaration::t_list:
        {
          QLineEdit *le = new QLineEdit (inner_frame);
          le->setEnabled (! p->is_readonly ());
          le->setObjectName (geo::to_qstring (p->get_name ()));
          m_widgets.push_back (le);
          inner_grid->addWidget (le, row, 1);

          connect (le, SIGNAL (editingFinished ()), this, SLOT (parameter_changed ()));
        }
        break;

      case geo::PCellParameterDeclaration::t_layer:
        {
          lay::LayerSelectionComboBox *ly = new lay::LayerSelectionComboBox (inner_frame);
          ly->setEnabled (! p->is_readonly ());
          ly->set_no_layer_available (true);
          ly->set_view (mp_view, m_cv_index, true /*all layers*/);
          ly->setObjectName (geo::to_qstring (p->get_name ()));
          m_widgets.push_back (ly);
          inner_grid->addWidget (ly, row, 1);

          connect (ly, SIGNAL (activated (int)), this, SLOT (parameter_changed ()));
        }
        break;

      case geo::PCellParameterDeclaration::t_boolean:
        {
          QCheckBox *cbx = new QCheckBox (inner_frame);
          //  this makes the checkbox not stretch over the full width - better when navigating with tab
          cbx->setSizePolicy (QSizePolicy (QSizePolicy::Fixed, QSizePolicy::Preferred));
          cbx->setEnabled (! p->is_readonly ());
          cbx->setObjectName (geo::to_qstring (p->get_name ()));
          m_widgets.push_back (cbx);
          inner_grid->addWidget (cbx, row, 1);

          connect (cbx, SIGNAL (stateChanged (int)), this, SLOT (parameter_changed ()));
        }
        break;

      default:
        m_widgets.push_back (0);
        break;
      }

    } else {

      QComboBox *cb = new QComboBox (inner_frame);
      cb->setObjectName (geo::to_qstring (p->get_name ()));

      int i = 0;
      for (std::vector<geo::Variant>::const_iterator c = p->get_choices ().begin (); c != p->get_choices ().end (); ++c, ++i) {
        if (i < int (p->get_choice_descriptions ().size ())) {
          cb->addItem (geo::to_qstring (p->get_choice_descriptions () [i]));
        } else {
          cb->addItem (geo::to_qstring (c->to_string ()));
        }
      }

      connect (cb, SIGNAL (activated (int)), this, SLOT (parameter_changed ()));

      cb->setEnabled (! p->is_readonly ());
      cb->setMinimumContentsLength (30);
      cb->setSizeAdjustPolicy (QComboBox::AdjustToMinimumContentsLengthWithIcon);
      m_widgets.push_back (cb);
      inner_grid->addWidget (cb, row, 1);

    }

    set_value (*p, m_widgets.back (), value);

    ++row;
    if (inner_frame == main_frame) {
      ++main_row;
    }

  }

  mp_parameters_area->setWidget (main_frame);
  main_frame->show ();

  //  does a first coerce and update. Ignore errors for now.
  bool ok = false;
  get_parameters (&ok);
}

PCellParametersPage::State
PCellParametersPage::get_state ()
{
  State s;

  s.valid = true;
  s.vScrollPosition = mp_parameters_area->verticalScrollBar ()->value ();
  s.hScrollPosition = mp_parameters_area->horizontalScrollBar ()->value ();

  if (focusWidget ()) {
    s.focusWidget = focusWidget ()->objectName ();
  }

  return s;
}

void
PCellParametersPage::set_state (const State &s)
{
  if (s.valid) {

    mp_parameters_area->verticalScrollBar ()->setValue (s.vScrollPosition);
    mp_parameters_area->horizontalScrollBar ()->setValue (s.hScrollPosition);

    if (! s.focusWidget.isEmpty ()) {
      QWidget *c = findChild<QWidget *> (s.focusWidget);
      if (c) {
        c->setFocus ();
      }
    }

  }
}

void  
PCellParametersPage::parameter_changed ()
{
  dm_parameter_changed ();
}

void
PCellParametersPage::do_parameter_changed ()
{
  //  does a coerce and update
  bool ok = false;
  get_parameters (&ok);
  if (ok) {
    emit edited ();
  }
}

std::vector<geo::Variant> 
PCellParametersPage::get_parameters (bool *ok)
{
  std::vector<geo::Variant> parameters;

  try {

    if (! mp_pcell_decl) {
      throw tl::Exception (geo::to_string (tr ("PCell no longer valid.")));
    }

    bool edit_error = true;

    int r = 0;
    const std::vector<geo::PCellParameterDeclaration> &pcp = mp_pcell_decl->parameter_declarations ();
    for (std::vector<geo::PCellParameterDeclaration>::const_iterator p = pcp.begin (); p != pcp.end (); ++p, ++r) {

      if (p->is_hidden () || p->get_type () == geo::PCellParameterDeclaration::t_shape) {

        if (r < (int) m_parameters.size ()) {
          parameters.push_back (m_parameters [r]);
        } else {
          parameters.push_back (p->get_default ());
        }

      } else {

        parameters.push_back (geo::Variant ());

        if (p->get_choices ().empty ()) {

          switch (p->get_type ()) {

          case geo::PCellParameterDeclaration::t_int:
            {
              QLineEdit *le = dynamic_cast<QLineEdit *> (m_widgets [r]);
              if (le) {

                try {

                  int v = 0;
                  geo::from_string (geo::to_string (le->text ()), v);

                  parameters.back () = geo::Variant (v);
                  lay::indicate_error (le, (tl::Exception *) 0);

                } catch (tl::Exception &ex) {

                  lay::indicate_error (le, &ex);
                  edit_error = false;

                }

              }
            }
            break;

          case geo::PCellParameterDeclaration::t_double:
            {
              QLineEdit *le = dynamic_cast<QLineEdit *> (m_widgets [r]);
              if (le) {

                try {

                  double v = 0;
                  geo::from_string (geo::to_string (le->text ()), v);

                  parameters.back () = geo::Variant (v);
                  lay::indicate_error (le, (tl::Exception *) 0);

                } catch (tl::Exception &ex) {

                  lay::indicate_error (le, &ex);
                  edit_error = false;

                }

              }
            }
            break;

          case geo::PCellParameterDeclaration::t_string:
            {
              QLineEdit *le = dynamic_cast<QLineEdit *> (m_widgets [r]);
              if (le) {
                parameters.back () = geo::Variant (geo::to_string (le->text ()));
              }
            }
            break;

          case geo::PCellParameterDeclaration::t_list:
            {
              QLineEdit *le = dynamic_cast<QLineEdit *> (m_widgets [r]);
              if (le) {
                std::vector<std::string> values = tl::split (geo::to_string (le->text ()), ",");
                parameters.back () = geo::Variant (values.begin (), values.end ());
              }
            }
            break;

          case geo::PCellParameterDeclaration::t_layer:
            {
              lay::LayerSelectionComboBox *ly = dynamic_cast<lay::LayerSelectionComboBox *> (m_widgets [r]);
              if (ly) {
                parameters.back () = geo::Variant (ly->current_layer_props ());
              }
            }
            break;
          case geo::PCellParameterDeclaration::t_boolean:
            {
              QCheckBox *cbx = dynamic_cast<QCheckBox *> (m_widgets [r]);
              if (cbx) {
                parameters.back () = geo::Variant (cbx->isChecked ());
              }
            }
            break;

          default:
            break;
          }

        } else {

          QComboBox *cb = dynamic_cast<QComboBox*> (m_widgets [r]);
          if (cb && cb->currentIndex () >= 0 && cb->currentIndex () < int (p->get_choices ().size ())) {
            parameters.back () = p->get_choices () [cb->currentIndex ()];
          }

        }

      }

    }

    if (! edit_error) {
      throw tl::Exception (geo::to_string (tr ("There are errors. See the highlighted edit fields for details.")));
    }

    //  coerce the parameters
    if (mp_view->cellview (m_cv_index).is_valid ()) {
      mp_pcell_decl->coerce_parameters (mp_view->cellview (m_cv_index)->layout (), parameters);
    }
    set_parameters (parameters);

    mp_error_label->hide ();
    mp_error_icon->hide ();

    if (ok) {
      *ok = true;
    }

  } catch (tl::ScriptError &ex) {

    if (ok) {
      mp_error_label->setText (geo::to_qstring (ex.basic_msg ()));
      mp_error_label->setToolTip (geo::to_qstring (ex.msg ()));
      mp_error_icon->show ();
      mp_error_label->show ();
      *ok = false;
    } else {
      throw;
    }

  } catch (tl::Exception &ex) {

    if (ok) {
      mp_error_label->setText (geo::to_qstring (ex.msg ()));
      mp_error_icon->show ();
      mp_error_label->show ();
      *ok = false;
    } else {
      throw;
    }

  }

  return parameters;
}

void 
PCellParametersPage::set_parameters (const std::vector<geo::Variant> &parameters)
{
  if (! mp_pcell_decl) {
    return;
  }

  //  write the changed value back
  size_t r = 0;
  const std::vector<geo::PCellParameterDeclaration> &pcp = mp_pcell_decl->parameter_declarations ();
  for (std::vector<geo::PCellParameterDeclaration>::const_iterator p = pcp.begin (); p != pcp.end (); ++p, ++r) {
    if (r < parameters.size () && m_widgets [r]) {
      set_value (*p, m_widgets [r], parameters [r]);
    }
  }
}

}


