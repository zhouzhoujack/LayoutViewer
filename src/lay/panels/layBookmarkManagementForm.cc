#include "layBookmarkManagementForm.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "detail/geoCellInst.h"
#include "layLayoutView.h"

#include <QListWidgetItem>

namespace lay
{

// ------------------------------------------------------------

class BookmarkListLVI 
  : public QListWidgetItem
{
public:
  BookmarkListLVI (QListWidget *parent, const std::string &name, const lay::DisplayState &state)
    : QListWidgetItem (geo::to_qstring (name), parent), m_state (state)
  {
    setFlags (flags () | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
  }

  const lay::DisplayState &state () const
  {
    return m_state;
  }

private:
  lay::DisplayState m_state;
};

// ------------------------------------------------------------

// -----------------------------------------------------------------------------
//  BookmarkManagementForm implementation

BookmarkManagementForm::BookmarkManagementForm (QWidget *parent, const char *name, const lay::BookmarkList &bookmarks, const std::set<size_t> &selected)
  : QDialog (parent), Ui::BookmarkManagementForm (),
    m_bookmarks (bookmarks)
{
  setObjectName (QString::fromUtf8 (name));

  Ui::BookmarkManagementForm::setupUi (this);

  QListWidgetItem *first_item = 0;

  for (size_t i = 0; i < m_bookmarks.size (); ++i) {
    QListWidgetItem *item = new BookmarkListLVI (bookmark_list, m_bookmarks.name (i), m_bookmarks.state (i));
    item->setSelected (selected.find (i) != selected.end ());
    if (! first_item && item->isSelected ()) {
      first_item = item;
    }
  }

  if (first_item) {
    bookmark_list->scrollToItem (first_item);
  }

  connect (delete_button, SIGNAL (clicked ()), this, SLOT (delete_pressed ()));
}

void
BookmarkManagementForm::delete_pressed ()
{
  QList<QListWidgetItem *> sel = bookmark_list->selectedItems ();
  for (QList<QListWidgetItem *>::const_iterator i = sel.begin (); i != sel.end (); ++i) {
    delete *i;
  }
}

void
BookmarkManagementForm::accept ()
{
  m_bookmarks.clear ();
  m_bookmarks.reserve (bookmark_list->count ());

  //  TODO: is there an iterator? Here we use the trick to select all and then get the
  //  list of items
  bookmark_list->selectAll (); 
  QList<QListWidgetItem *> sel = bookmark_list->selectedItems ();
  for (QList<QListWidgetItem *>::const_iterator i = sel.begin (); i != sel.end (); ++i) {
    BookmarkListLVI *bm = dynamic_cast<BookmarkListLVI *> (*i);
    if (bm) {
      m_bookmarks.add (geo::to_string (bm->text ()), bm->state ());
    }
  }

  QDialog::accept ();
}

}



