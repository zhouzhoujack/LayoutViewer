#include "layLayerMappingWidget.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "tlExceptions.h"
#include "layFileDialog.h"
#include "layDialogs.h"
#include "layLayerProperties.h"
#include "layQtTools.h"
#include "tlXMLParser.h"

#include "ui_LayerMappingWidget.h"

#include <QMessageBox>

#include <fstream>

namespace lay
{

// -----------------------------------------------------------------------------
//  LayerMappingWidget implementation

LayerMappingWidget::LayerMappingWidget (QWidget *parent)
  : QFrame (parent), mp_layer_table_file_dialog (0), mp_ui (0)
{
  mp_ui = new Ui::LayerMappingWidget ();
  mp_ui->setupUi (this);

  connect (mp_ui->add_pb, SIGNAL (clicked ()), this, SLOT (add_button_pressed ()));
  connect (mp_ui->load_pb, SIGNAL (clicked ()), this, SLOT (load_button_pressed ()));
  connect (mp_ui->delete_pb, SIGNAL (clicked ()), this, SLOT (delete_button_pressed ()));
  connect (mp_ui->edit_pb, SIGNAL (clicked ()), this, SLOT (edit_button_pressed ()));

  activate_help_links (mp_ui->help_label);

  mp_ui->layer_lv->viewport ()->acceptDrops ();

  connect (mp_ui->tabs, SIGNAL (currentChanged (int)), this, SLOT (current_tab_changed (int)));

  mp_layer_table_file_dialog = new lay::FileDialog (this, 
                                                    geo::to_string (QObject::tr ("Load Layer Table")), 
                                                    geo::to_string (QObject::tr ("Layer properties and text files (*.lyp *.txt);;Layer properties files (*.lyp);;Text files (*.txt);;All files (*)")));
}

LayerMappingWidget::~LayerMappingWidget ()
{
  delete mp_ui;
  mp_ui = 0;

  delete mp_layer_table_file_dialog;
  mp_layer_table_file_dialog = 0;
}

void 
LayerMappingWidget::set_layer_map (const geo::LayerMap &lm)
{
  std::vector<unsigned int> layer_ids = lm.get_layers ();

  mp_ui->text_edit->setPlainText (geo::to_qstring (lm.to_string_file_format ()));

  mp_ui->layer_lv->reset ();
  mp_ui->layer_lv->clear ();

  for (std::vector<unsigned int>::const_iterator layer_id = layer_ids.begin (); layer_id != layer_ids.end (); ++layer_id) {

    std::string mapping = lm.mapping_str (*layer_id);
    QListWidgetItem *item = new QListWidgetItem (mp_ui->layer_lv);
    item->setData (Qt::DisplayRole, geo::to_qstring (mapping));
    item->setFlags (item->flags () | Qt::ItemIsEditable);
    mp_ui->layer_lv->addItem (item);

  }
}

geo::LayerMap 
LayerMappingWidget::get_layer_map () const
{
  return get_layer_map_from_tab (mp_ui->tabs->currentIndex ());
}

geo::LayerMap
LayerMappingWidget::get_layer_map_from_tab (int tab) const
{
  geo::LayerMap lm;

  if (tab == 0) {

    for (int i = 0; i < mp_ui->layer_lv->count (); ++i) {
      std::string t = geo::to_string (mp_ui->layer_lv->item (i)->data (Qt::DisplayRole).toString ());
      try {
        lm.add_expr (t, (unsigned int) i);
      } catch (...) {
        mp_ui->layer_lv->setCurrentItem (mp_ui->layer_lv->item (i));
        throw;
      }
    }

  } else {

    lm = geo::LayerMap::from_string_file_format (geo::to_string (mp_ui->text_edit->toPlainText ()));

  }

  return lm;
}

bool
LayerMappingWidget::is_empty () const
{
  return (mp_ui->layer_lv->count () == 0);
}

void 
LayerMappingWidget::load_button_pressed ()
{
  BEGIN_PROTECTED

  if (mp_layer_table_file_dialog->get_open (m_layer_table_file)) {

    bool success = false;
   
    //  try to load as .lyp file
    try {

      tl::XMLFileSource in (m_layer_table_file);
      lay::LayerPropertiesList props;
      props.load (in);

      mp_ui->layer_lv->reset ();
      mp_ui->layer_lv->clear ();

      geo::LayerMap lm;

      //  use those layers which have cellview index 0
      unsigned int n = 0;
      for (LayerPropertiesConstIterator lay_iter = props.begin_const_recursive (); ! lay_iter.at_end (); ++lay_iter) {
        if (! lay_iter->has_children () && lay_iter->source (true /*=real*/).cv_index () == 0) {
          geo::LayerProperties db_lp = lay_iter->source (true /*=real*/).layer_props ();
          lm.map (db_lp, (unsigned int) n++);
        }
      }

      set_layer_map (lm);

      //  if successful, stop now.
      success = true;

    } catch (...) {
      //  ...
    }

    if (! success) {

      //  read the text file
      std::ifstream input (m_layer_table_file.c_str (), std::ios::in);
      if (input.good ()) {
        std::string text = std::string (std::istreambuf_iterator<char> (input), std::istreambuf_iterator<char> ());
        set_layer_map (geo::LayerMap::from_string_file_format (text));
        if (is_empty ()) {
          emit enable_all_layers (true);
        }
        emit layerListChanged ();
      }

    }

  }

  END_PROTECTED
}

void 
LayerMappingWidget::add_button_pressed ()
{
  BEGIN_PROTECTED

  bool was_empty = is_empty ();

  if (mp_ui->layer_lv->currentItem ()) {
    mp_ui->layer_lv->reset ();
  } 
  mp_ui->layer_lv->selectionModel ()->clear ();

  std::string data = geo::to_string(mp_ui->layer_lv->count() + 1) + "/0";
  QListWidgetItem *item = new QListWidgetItem (mp_ui->layer_lv);
  item->setData (Qt::DisplayRole, QVariant (geo::to_qstring (data)));
  item->setFlags (item->flags () | Qt::ItemIsEditable);
  mp_ui->layer_lv->addItem (item);
  mp_ui->layer_lv->setCurrentItem (item);
  mp_ui->layer_lv->editItem (item);

  emit layerItemAdded ();
  if (was_empty && !is_empty ()) {
    emit enable_all_layers (false);
  }

  END_PROTECTED
}

void 
LayerMappingWidget::delete_button_pressed ()
{
  BEGIN_PROTECTED

  if (mp_ui->layer_lv->currentItem ()) {

    bool was_empty = is_empty ();

    QList<QListWidgetItem *> sel_items = mp_ui->layer_lv->selectedItems ();
    for (QList<QListWidgetItem *>::const_iterator sel_item = sel_items.begin (); sel_item != sel_items.end (); ++sel_item) {
      delete *sel_item;
    }

    emit layerItemDeleted ();
    if (! was_empty && is_empty ()) {
      emit enable_all_layers (true);
    }

  }

  END_PROTECTED
}

void
LayerMappingWidget::edit_button_pressed ()
{
  BEGIN_PROTECTED

  if (mp_ui->layer_lv->currentItem ()) {
    mp_ui->layer_lv->editItem (mp_ui->layer_lv->currentItem ());
  }

  END_PROTECTED
}

void
LayerMappingWidget::current_tab_changed (int index)
{
  set_layer_map (get_layer_map_from_tab (1 - index));
}

}

