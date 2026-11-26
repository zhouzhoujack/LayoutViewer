#ifndef HDR_layBookmarkManagementForm
#define HDR_layBookmarkManagementForm

#include <QObject>  //  required during the dependency pass
#include "ui_BookmarkManagementForm.h"

#include "layLayoutView.h"

#include <set>

namespace lay
{

class BookmarkManagementForm
  : public QDialog, private Ui::BookmarkManagementForm
{
  Q_OBJECT 

public:
  BookmarkManagementForm (QWidget *parent, const char *name, const lay::BookmarkList &bookmarks, const std::set<size_t> &selected);

  /**
   *  @brief Obtain the bookmark list
   */
  const lay::BookmarkList &bookmarks ()
  {
    return m_bookmarks;
  }

  virtual void accept ();

public slots:
  void delete_pressed ();

private:
  lay::BookmarkList m_bookmarks;
};

}

#endif

