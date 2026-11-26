#ifndef HDR_SelectCellViewForm
#define HDR_SelectCellViewForm


#include <string>
#include <vector>

#include <QDialog>

#include "ui_SelectCellViewForm.h"

namespace lay
{

class CellView;
class LayoutView;

class SelectCellViewForm
  : public QDialog, private Ui::SelectCellViewForm
{
  Q_OBJECT 

public:
  SelectCellViewForm (QWidget *parent, lay::LayoutView *view, const std::string &title, bool single = false);

  /**
   *  @brief Set the selection to a single selection
   */
  void set_selection (int sel);

  /**
   *  @brief Set the title
   */
  void set_title (const std::string &title);
  
  /**
   *  @brief Set the caption
   */
  void set_caption (const std::string &caption);
  
  /**
   *  @brief This method must be called on all cv's before setup () to make the cv's known
   */
  void tell_cellview (const lay::CellView &cv);

  /**
   *  @brief Obtain the selected cellview's index (with changes)
   */
  std::vector <int> selected_cellviews () const;

  /**
   *  @brief Obtain the selected cellview's index (in single selection mode)
   */
  int selected_cellview () const;

  /**
   *  @brief Return true, if all cellviews are selected
   */
  bool all_selected () const;

public slots:
  virtual void select_all ();
};

}

#endif

