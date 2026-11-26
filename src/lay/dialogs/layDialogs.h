#ifndef HDR_layDialogs
#define HDR_layDialogs

#include "detail/geoPoint.h"
#include "detail/geoVector.h"
#include "detail/geoTypes.h"
#include "detail/geoPropertiesRepository.h"

#include <QtWidgets/QDialog>
#include <memory>

class QTreeWidgetItem;

namespace geo
{
  class Layout;
  struct LayerProperties;
}

namespace lay
{
  class GenericSyntaxHighlighterAttributes;
}

namespace Ui
{
  class LayerSourceDialog;
  class NewLayoutPropertiesDialog;
  class NewLayerPropertiesDialog;
  class NewCellPropertiesDialog;
  class MoveOptionsDialog;
  class MoveToOptionsDialog;
  class DeleteCellModeDialog;
  class CopyCellModeDialog;
  class ReplaceCellOptionsDialog;
  class ClearLayerModeDialog;
  class OpenLayoutModeDialog;
  class RenameCellDialog;
  class DuplicateLayerDialog;
  class AlignCellOptionsDialog;
  class FlattenInstOptionsDialog;
  class UserPropertiesForm;
  class UserPropertiesEditForm;
}

namespace lay
{

class CellView;
class LayoutView;

/**
 *  @brief The layer source dialog
 */
class LayerSourceDialog
  : public QDialog
{
Q_OBJECT

public:
  LayerSourceDialog (QWidget *parent);
  ~LayerSourceDialog ();

  bool exec_dialog (std::string &s);

private:
  Ui::LayerSourceDialog *mp_ui;
};

/**
 *  @brief The new cell properties dialog
 */
class NewCellPropertiesDialog 
  : public QDialog
{
Q_OBJECT

public:
  NewCellPropertiesDialog (QWidget *parent);
  virtual ~NewCellPropertiesDialog ();

  bool exec_dialog (const geo::Layout *layout, std::string &cell_name, double &size);

private:
  virtual void accept ();

  Ui::NewCellPropertiesDialog *mp_ui;
  const geo::Layout *mp_layout;
};

/**
 *  @brief The new layer properties dialog
 */
class NewLayerPropertiesDialog 
  : public QDialog
{
Q_OBJECT

public:
  NewLayerPropertiesDialog (QWidget *parent);
  virtual ~NewLayerPropertiesDialog ();

  bool exec_dialog (const lay::CellView &cv, geo::LayerProperties &src);
  bool exec_dialog (geo::LayerProperties &src);

private:
  virtual void accept ();
  void get (geo::LayerProperties &src);

  Ui::NewLayerPropertiesDialog *mp_ui;
};

/**
 *  @brief The move options dialog
 */
class MoveOptionsDialog 
  : public QDialog
{
Q_OBJECT

public:
  MoveOptionsDialog (QWidget *parent);
  virtual ~MoveOptionsDialog ();

  bool exec_dialog (geo::DVector &disp);

private:
  virtual void accept ();

  Ui::MoveOptionsDialog *mp_ui;
};

/**
 *  @brief The move "to" options dialog
 */
class MoveToOptionsDialog 
  : public QDialog
{
Q_OBJECT

public:
  MoveToOptionsDialog (QWidget *parent);
  virtual ~MoveToOptionsDialog ();

  bool exec_dialog (int &mode_x, int &mode_y, geo::DPoint &target);

private slots:
  void button_clicked ();

private:
  virtual void accept ();

  Ui::MoveToOptionsDialog *mp_ui;
};

/**
 *  @brief The rename cell options dialog
 */
class RenameCellDialog 
  : public QDialog
{
Q_OBJECT

public:
  RenameCellDialog (QWidget *parent);
  virtual ~RenameCellDialog ();

  bool exec_dialog (const geo::Layout &layout, std::string &name);

private:
  virtual void accept ();

  Ui::RenameCellDialog *mp_ui;
  const geo::Layout *mp_layout;
};

/**
 *  @brief The replace cell options dialog
 */
class ReplaceCellOptionsDialog
  : public QDialog
{
Q_OBJECT

public:
  ReplaceCellOptionsDialog (QWidget *parent);
  virtual ~ReplaceCellOptionsDialog ();

  /** 
   *  @brief Execute the dialog
   *
   *  The mode is either 0 (for shallow), 1 (for deep) and 2 (for complete)
   */
  bool exec_dialog (const lay::CellView &cv, int &replace_mode, geo::cell_index_type &cell);

protected:
  virtual void accept ();

private:
  Ui::ReplaceCellOptionsDialog *mp_ui;
};

/**
 *  @brief The copy cell options dialog
 */
class CopyCellModeDialog 
  : public QDialog
{
Q_OBJECT

public:
  CopyCellModeDialog (QWidget *parent);
  virtual ~CopyCellModeDialog ();

  /** 
   *  @brief Execute the dialog
   *
   *  The mode is either 0 (for shallow), 1 (for deep)
   */
  bool exec_dialog (int &copy_mode);

private:
  Ui::CopyCellModeDialog *mp_ui;
};

/**
 *  @brief The delete cell options dialog
 */
class DeleteCellModeDialog 
  : public QDialog
{
Q_OBJECT

public:
  DeleteCellModeDialog (QWidget *parent);
  virtual ~DeleteCellModeDialog ();

  /** 
   *  @brief Execute the dialog
   *
   *  The mode is either 0 (for shallow), 1 (for deep) and 2 (for complete)
   */
  bool exec_dialog (int &delete_mode);

private:
  Ui::DeleteCellModeDialog *mp_ui;
};

/**
 *  @brief The delete cell options dialog
 */
class ClearLayerModeDialog 
  : public QDialog
{
Q_OBJECT

public:
  ClearLayerModeDialog (QWidget *parent);
  virtual ~ClearLayerModeDialog ();

  /** 
   *  @brief Execute the dialog
   *
   *  The mode is either 0 (for locally), 1 (for hierarchically) and 2 (for all)
   */
  bool exec_dialog (int &clear_mode);

private:
  Ui::ClearLayerModeDialog *mp_ui;
};

/**
 *  @brief The open layout mode dialog
 */
class OpenLayoutModeDialog 
  : public QDialog
{
Q_OBJECT

public:
  OpenLayoutModeDialog (QWidget *parent);
  virtual ~OpenLayoutModeDialog ();

  /** 
   *  @brief Execute the dialog
   *
   *  The mode is either 0 (to replace current view), 1 (to create new view) and 2 (add to current view)
   */
  bool exec_dialog (int &open_mode);

private:
  Ui::OpenLayoutModeDialog *mp_ui;
};

/**
 *  @brief The new layout properties dialog
 */
class NewLayoutPropertiesDialog 
  : public QDialog
{
Q_OBJECT

public:
  NewLayoutPropertiesDialog (QWidget *parent);
  virtual ~NewLayoutPropertiesDialog ();

  bool exec_dialog (std::string &tech_name, std::string &cell_name, double &dbu, double &window_size, std::vector<geo::LayerProperties> &layers, bool &current_panel);

private slots:
  void tech_changed ();

private:
  virtual void accept ();

  Ui::NewLayoutPropertiesDialog *mp_ui;
};

/**
 *  @brief The duplicate layer operation options
 */
class DuplicateLayerDialog 
  : public QDialog
{
Q_OBJECT

public:
  DuplicateLayerDialog (QWidget *parent);
  virtual ~DuplicateLayerDialog ();

  bool exec_dialog (lay::LayoutView *view, int &cv, int &layer, int &cv_res, int &layer_res, int &hier_mode, bool &clear_before);

public slots:
  void cv_changed (int);

private:
  virtual void accept ();

  Ui::DuplicateLayerDialog *mp_ui;
  lay::LayoutView *mp_view;
};

/**
 *  @brief A data structure holding the options for the "align cell" dialog
 */
struct AlignCellOptions
{
  AlignCellOptions ()
    : mode_x (-1), mode_y (-1), xpos (0.0), ypos (0.0), visible_only (false), adjust_parents (true)
  { }

  int mode_x, mode_y;
  double xpos, ypos;
  bool visible_only;
  bool adjust_parents;
};

/**
 *  @brief The merge operation options
 */
class AlignCellOptionsDialog 
  : public QDialog
{
Q_OBJECT

public:
  AlignCellOptionsDialog (QWidget *parent);
  virtual ~AlignCellOptionsDialog ();

  bool exec_dialog (AlignCellOptions &data);

private slots:
  void button_clicked ();
  void accept ();

private:
  Ui::AlignCellOptionsDialog *mp_ui;
};

/**
 *  @brief Options dialog for the "flatten instances" function
 */
class FlattenInstOptionsDialog
  : public QDialog
{
public:
  FlattenInstOptionsDialog (QWidget *parent, bool enable_pruning = true);
  virtual ~FlattenInstOptionsDialog ();

  bool exec_dialog (int &levels, bool &prune); 

private:
  Ui::FlattenInstOptionsDialog *mp_ui;
};

/**
 *  @brief The user properties report form
 */
class UserPropertiesForm 
  : public QDialog
{
Q_OBJECT

public:
  UserPropertiesForm (QWidget *parent);
  virtual ~UserPropertiesForm ();

  bool show (lay::LayoutView *view, unsigned int cv_index, geo::properties_id_type &prop_id);

public slots:
  void add ();
  void remove ();
  void edit ();
  void dbl_clicked (QTreeWidgetItem *, int);
  void tab_changed (int);

private:
  geo::PropertiesRepository::properties_set get_properties (int tab);
  void set_properties (const geo::PropertiesRepository::properties_set &props);
  void accept ();

  bool m_editable;
  geo::PropertiesRepository *mp_prep;
  Ui::UserPropertiesForm *mp_ui;
  std::unique_ptr<lay::GenericSyntaxHighlighterAttributes> mp_hl_attributes, mp_hl_basic_attributes;
};

/**
 *  @brief The user properties report form
 */
class UserPropertiesEditForm 
  : public QDialog
{
public:
  UserPropertiesEditForm (QWidget *parent);
  virtual ~UserPropertiesEditForm ();

  bool show (QString &key, QString &value);
  virtual void accept ();

  Ui::UserPropertiesEditForm *mp_ui;
};

}

#endif


