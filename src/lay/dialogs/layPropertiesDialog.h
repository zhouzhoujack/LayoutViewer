#ifndef HDR_layPropertiesDialog
#define HDR_layPropertiesDialog

#include <vector>

#include <QMutex>
#include <QDialog>
#include <QLabel>
#include <QStatusBar>
#include <QMessageBox>

#include <detail/geoManager.h>

#include "ui_PropertiesDialog.h"

#include <memory>

class QStackedLayout;

namespace lay
{

class Editable;
class Editables;
class PropertiesPage;
class MainWindow;

/**
 *  @brief The properties dialog
 *
 *  This is the implementation of the properties dialog that
 *  is opened to edit or view the properties of a set of selected
 *  objects from a set of editables.
 */

class PropertiesDialog
  : public QDialog, private Ui::PropertiesDialog
{
Q_OBJECT

public:
  /**
   *  @brief Constructor
   */
  PropertiesDialog (QWidget *parent, geo::Manager *manager, lay::Editables *editables);

  /**
   *  @brief The Destructor
   */
  ~PropertiesDialog ();

private:
  std::vector<lay::PropertiesPage *> mp_properties_pages;
  geo::Manager *mp_manager;
  lay::Editables *mp_editables;
  int m_index;
  QStackedLayout *mp_stack;
  lay::MainWindow *mp_mw;
  size_t m_objects, m_current_object;
  bool m_auto_applied;
  geo::Manager::transaction_id_t m_transaction_id;

  void disconnect ();
  bool any_prev () const;
  bool any_next () const;
  void update_title ();

public slots:
  void apply ();
  void next_pressed ();
  void prev_pressed ();
  void cancel_pressed ();
  void ok_pressed ();

protected:
  void reject ();
};

}

#endif

