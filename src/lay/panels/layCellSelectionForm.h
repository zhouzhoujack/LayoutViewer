#ifndef HDR_layCellSelectionForm
#define HDR_layCellSelectionForm

#include <QtWidgets/QAction>
#include "detail/utilsDeferredExecution.h"

#include "ui_CellSelectionForm.h"
#include "ui_LibraryCellSelectionForm.h"
#include "layCellView.h"
#include "tlDeferredExecution.h"

#include <vector>
#include <string>

namespace lay
{

class CellView;
class LayoutView;

/**
 *  @brief A form to select a cell and a cell view index
 */
class CellSelectionForm
  : public QDialog, private Ui::CellSelectionForm
{
  Q_OBJECT 

public:
  CellSelectionForm (QWidget *parent, lay::LayoutView *view, const char *name, bool simple_mode = false);

  /**
   *  @brief Obtain the selected cellview's index (with changes)
   */
  int selected_cellview_index () const;

  /**
   *  @brief Get the selected cellview's state
   */
  const lay::CellView &selected_cellview () const;

public slots:
  void view_changed(int);
  void cell_changed(const QModelIndex &current, const QModelIndex &);
  void child_changed(const QModelIndex &current);
  void parent_changed(const QModelIndex &current);
  void name_changed();
  void set_parent();
  void set_child();
  void hide_cell();
  void show_cell();
  void apply_clicked();
  void find_next_clicked();
  void find_prev_clicked();

private:
  lay::LayoutView *mp_view;
  std::vector <lay::CellView> m_cellviews;
  int m_current_cv;
  bool m_name_cb_enabled;
  bool m_cells_cb_enabled;
  bool m_children_cb_enabled;
  bool m_parents_cb_enabled;
  geo::DeferredMethod<CellSelectionForm> m_update_all_dm;
  bool m_simple_mode;
  QAction *mp_use_regular_expressions;
  QAction *mp_case_sensitive;

  void update_cell_list ();
  void update_parents_list ();
  void update_children_list ();
  void update_all ();
  void select_entry (lay::CellView::cell_index_type n);
  void commit_cv ();
  void store_config ();
  void accept ();
  void reject ();
};

/**
 *  @brief A form to select a cell from a library
 */
class LibraryCellSelectionForm
  : public QDialog, private Ui::LibraryCellSelectionForm
{
  Q_OBJECT 

public:
  /**
   *  @brief Create a selection form for cells from the given layout
   *
   *  This version does not provide library selection. \get_current_library will
   *  always return 0.
   *  If all_cells is true, all cells (not only top cells and basic cells) are shown.
   */
  LibraryCellSelectionForm (QWidget *parent, geo::Layout *layout, const char *name, bool all_cells = false);

  /**
   *  @brief Create a selection form for cells plus the library
   *
   *  If all_cells is true, all cells (not only top cells and basic cells) are shown.
   */
  LibraryCellSelectionForm (QWidget *parent, const char *name, bool all_cells = false);

  /**
   *  @brief Set the selected library
   */
  void set_current_library (geo::Library *lib);

  /**
   *  @brief Get the selected library
   */
  geo::Library *get_current_library () const
  {
    return mp_lib;
  }

  /**
   *  @brief Set the selected cell's index
   */
  void set_selected_cell_index (geo::cell_index_type ci);

  /**
   *  @brief Select the PCell with the given ID (when BasicCells is selected in the flags)
   */
  void set_selected_pcell_id (geo::pcell_id_type pci);

  /**
   *  @brief Obtain the selected cell's index
   */
  geo::cell_index_type selected_cell_index () const
  {
    return m_cell_index;
  }

  /**
   *  @brief Returns true, if the selected cell is a pcell
   */
  bool selected_cell_is_pcell () const
  {
    return m_is_pcell;
  }

  /**
   *  @brief Get the PCell ID of the selected cell (if it's a PCell)
   */
  geo::pcell_id_type selected_pcell_id () const
  {
    return m_pcell_id;
  }

public slots:
  void name_changed(const QString &);
  void cell_changed(const QModelIndex &current, const QModelIndex &);
  void find_next_clicked();
  void lib_changed ();
  void show_all_changed ();

private:
  geo::Library *mp_lib;
  const geo::Layout *mp_layout;
  bool m_name_cb_enabled;
  bool m_cells_cb_enabled;
  geo::cell_index_type m_cell_index;
  geo::pcell_id_type m_pcell_id;
  bool m_is_pcell;
  bool m_all_cells;

  void select_entry (geo::cell_index_type n);
  void select_pcell_entry (geo::pcell_id_type n);
  void update_cell_list ();
  void accept ();
};

}

#endif

