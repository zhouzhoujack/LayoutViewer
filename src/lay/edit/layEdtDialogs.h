#ifndef HDR_edtDialogs
#define HDR_edtDialogs

#include <limits>
#include <list>
#include <utility>

#include <QtWidgets/QDialog>

#include "geoLayout.h"
#include "detail/geoPoint.h"

#include "ui_InstantiationForm.h"
#include "ui_ChangeLayerOptionsDialog.h"
#include "ui_AlignOptionsDialog.h"
#include "ui_DistributeOptionsDialog.h"
#include "ui_CopyModeDialog.h"
#include "ui_MakeCellOptionsDialog.h"
#include "ui_MakeArrayOptionsDialog.h"
#include "ui_RoundCornerOptionsDialog.h"

namespace lay
{
  class LayoutView;
  class Marker;
  class ObjectInstPath;
}

namespace edt {

/**
 *  @brief The copy mode dialog
 */
class CopyModeDialog 
  : public QDialog,
    public Ui::CopyModeDialog
{
Q_OBJECT

public:
  CopyModeDialog (QWidget *parent);
  virtual ~CopyModeDialog ();

  bool exec_dialog (unsigned int &mode, bool &dont_ask);
};

/**
 *  @brief The instantiation report form
 */
class InstantiationForm 
  : public QDialog,
    public Ui::InstantiationForm
{
Q_OBJECT

public:
  InstantiationForm (QWidget *parent);
  virtual ~InstantiationForm ();

  void show (lay::LayoutView *view, const lay::ObjectInstPath &path);

public slots:
  void display_mode_changed (bool);
  void double_clicked (QListWidgetItem *item);

private:
  void update ();

  lay::LayoutView *mp_view;
  const lay::ObjectInstPath *mp_path;
  lay::Marker *mp_marker;
  bool m_enable_cb_callbacks;
};

/**
 *  @brief The change layer options dialog
 */
class ChangeLayerOptionsDialog 
  : public QDialog,
    public Ui::ChangeLayerOptionsDialog
{
Q_OBJECT

public:
  ChangeLayerOptionsDialog (QWidget *parent);
  virtual ~ChangeLayerOptionsDialog ();

  bool exec_dialog (lay::LayoutView *view, int cv_index, unsigned int &new_layer);
};

/**
 *  @brief Align function options dialog
 */
class AlignOptionsDialog 
  : public QDialog,
    public Ui::AlignOptionsDialog
{
Q_OBJECT

public:
  AlignOptionsDialog (QWidget *parent);
  virtual ~AlignOptionsDialog ();

  bool exec_dialog (lay::LayoutView *view, int &hmode, int &vmode, bool &visible_layers);
};

/**
 *  @brief Distribute function options dialog
 */
class DistributeOptionsDialog
  : public QDialog,
    public Ui::DistributeOptionsDialog
{
Q_OBJECT

public:
  DistributeOptionsDialog (QWidget *parent);
  virtual ~DistributeOptionsDialog ();

  bool exec_dialog (lay::LayoutView *view, bool &hdistribute, int &hmode, double &hpitch, double &hspace, bool &vdistribute, int &vmode, double &vpitch, double &vspace, bool &visible_layers);
};

/**
 *  @brief Options dialog for the "make cell" function
 */
class MakeCellOptionsDialog
  : public QDialog, 
    private Ui::MakeCellOptionsDialog
{
Q_OBJECT

public:
  MakeCellOptionsDialog (QWidget *parent);
  bool exec_dialog (const geo::Layout &layout, std::string &name, int &mode_x, int &mode_y);

private slots:
  void button_clicked ();
};

/**
 *  @brief Options dialog for the "make array" function
 */
class MakeArrayOptionsDialog
  : public QDialog, 
    private Ui::MakeArrayOptionsDialog
{
Q_OBJECT

public:
  MakeArrayOptionsDialog (QWidget *parent);
  bool exec_dialog (geo::DVector &a, unsigned int &na, geo::DVector &b, unsigned int &nb);

  virtual void accept ();
};

/**
 *  @brief Options dialog for the "round corners" function
 */
class RoundCornerOptionsDialog
  : public QDialog, 
    private Ui::RoundCornerOptionsDialog
{
Q_OBJECT

public:
  RoundCornerOptionsDialog (QWidget *parent);
  ~RoundCornerOptionsDialog ();

  bool exec_dialog (const geo::Layout &layout, double &router, double &rinner, unsigned int &npoints, bool &undo_before_apply, double router_extracted, double rinner_extracted, unsigned int npoints_extracted, bool has_extracted);

  virtual void accept ();

private slots:
  void amend_changed ();

private:
  const geo::Layout *mp_layout;
  double m_router_extracted, m_rinner_extracted;
  unsigned int m_npoints_extracted;
  bool m_has_extracted;
};

} // namespace edt

#endif

