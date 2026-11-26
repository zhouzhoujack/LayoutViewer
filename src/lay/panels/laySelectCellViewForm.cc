#include "laySelectCellViewForm.h"
#include "detail/utilsInternational.h"
#include "layCellView.h"
#include "layLayoutView.h"

namespace lay
{

// ------------------------------------------------------------

// -----------------------------------------------------------------------------
//  SelectCellViewForm implementation

SelectCellViewForm::SelectCellViewForm (QWidget *parent, lay::LayoutView *view, const std::string &title, bool single) 
  : QDialog (parent), Ui::SelectCellViewForm ()
{
  setObjectName (QString::fromUtf8 ("select_cv"));

  Ui::SelectCellViewForm::setupUi (this);

  if (single) {
    cvs_lb->setSelectionMode (QAbstractItemView::SingleSelection);
  }

  // signals and slots connections
  connect( ok_button, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( cancel_button, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( select_all_pb, SIGNAL( clicked() ), this, SLOT( select_all() ) );

  if (single) {
    select_all_pb->hide ();
  }

  for (unsigned int i = 0; i < view->cellviews (); ++i) {
    tell_cellview (view->cellview (i));
  }

  set_title (title);
}

void 
SelectCellViewForm::set_selection (int sel)
{
  for (int i = 0; i < int (cvs_lb->count ()); ++i) {
    cvs_lb->item (i)->setSelected (false);
  }
  if (sel >= 0 && sel < int (cvs_lb->count ())) {
    cvs_lb->setCurrentItem (cvs_lb->item (sel));
    cvs_lb->item (sel)->setSelected (true);
  }
}

void 
SelectCellViewForm::set_title (const std::string &title)
{
  title_lbl->setText (geo::to_qstring (title));
}
  
void 
SelectCellViewForm::set_caption (const std::string &caption)
{
  setWindowTitle (geo::to_qstring (caption));
}
  
void 
SelectCellViewForm::tell_cellview (const lay::CellView &cv)
{
  cvs_lb->addItem (geo::to_qstring (cv->name ()));
  cvs_lb->setCurrentItem (0);
  cvs_lb->item (0)->setSelected (true);
}

bool 
SelectCellViewForm::all_selected () const
{
  for (int i = 0; i < int (cvs_lb->count ()); ++i) {
    if (! cvs_lb->item (i)->isSelected ()) {
      return false;
    }
  }
  return true;
}

std::vector <int> 
SelectCellViewForm::selected_cellviews () const
{
  std::vector <int> res;

  for (int i = 0; i < int (cvs_lb->count ()); ++i) {
    if (cvs_lb->item (i)->isSelected ()) {
      res.push_back (i);
    }
  }

  return res;
}

int 
SelectCellViewForm::selected_cellview () const
{
  for (int i = 0; i < int (cvs_lb->count ()); ++i) {
    if (cvs_lb->item (i)->isSelected ()) {
      return i;
    }
  }
  return -1;
}

void 
SelectCellViewForm::select_all ()
{
  cvs_lb->clearSelection ();  //  without this, not all items may be selected in "selectAll"
  cvs_lb->selectAll ();
}

}
  
