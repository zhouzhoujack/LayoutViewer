#ifndef HDR_layBookmarksView
#define HDR_layBookmarksView


#include "layBookmarkList.h"

#include <QFrame>
#include <QListView>

#include <set>

namespace lay
{

class LayoutView;
class AbstractMenu;

/**
 *  @brief A widget to display a bookmark list
 */
class BookmarksView
  : public QFrame
{
Q_OBJECT

public:
  BookmarksView (LayoutView *view, QWidget *parent, const char *name);
  ~BookmarksView ();

  void set_background_color (QColor c);
  void set_text_color (QColor c);
  void follow_selection (bool f);

  std::set<size_t> selected_bookmarks ();

  void refresh ();

public slots:
  void bookmark_triggered (const QModelIndex &index);
  void current_bookmark_changed (const QModelIndex &index);
  void context_menu (const QPoint &p);

private:
  LayoutView *mp_view;
  QListView *mp_bookmarks;
  bool m_follow_selection;
};

} // namespace lay

#endif

