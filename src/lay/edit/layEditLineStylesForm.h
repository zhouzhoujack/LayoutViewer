#ifndef HDR_layEditLineStylesForm
#define HDR_layEditLineStylesForm

#include <QDialog>

#include "layLineStyles.h"
#include "detail/geoObject.h"

class QListWidgetItem;

namespace Ui
{
  class EditLineStylesForm;
}

namespace lay
{

class LayoutView;

class EditLineStylesForm
  : public QDialog, public geo::Object
{
  Q_OBJECT 

public:
  EditLineStylesForm (lay::LayoutView *view, const lay::LineStyles &styles);
  ~EditLineStylesForm ();

  //  ...

  const lay::LineStyles &styles () const
  {
    return m_styles;
  }

  int selected () const
  {
    return m_selected;
  }
  
  void undo (geo::Op *op);
  void redo (geo::Op *op);

public slots:
  void sel_changed (QListWidgetItem *current, QListWidgetItem *); 
  void double_clicked (QListWidgetItem *item); 
  void new_button_clicked ();
  void delete_button_clicked ();
  void clone_button_clicked ();
  void up_button_clicked ();
  void down_button_clicked ();
  void clear_button_clicked ();
  void invert_button_clicked ();
  void fliph_button_clicked ();
  void sleft_button_clicked ();
  void sright_button_clicked ();
  void undo_button_clicked ();
  void redo_button_clicked ();
  void edited ();
  void size_changed ();

private slots:
  void editor_size_changed ();

protected:
  lay::LineStyles::iterator current ();
  lay::LineStyles::iterator index_of (QListWidgetItem *item);
  void update ();
  void update_current_item ();
  void select_item (int index);
  void handle_op (geo::Op *op, bool undo);

private:
  Ui::EditLineStylesForm *mp_ui;
  int m_selected;
  lay::LineStyles m_styles;
  geo::Manager m_manager;
  lay::LayoutView *mp_view;
  bool m_selection_changed_enabled;
};

}

#endif

