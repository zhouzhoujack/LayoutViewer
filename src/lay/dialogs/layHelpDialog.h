#ifndef HDR_layHelpDialog
#define HDR_layHelpDialog

#include <QtWidgets/QDialog>

#include <memory>
#include <string>

namespace Ui
{
  class HelpDialog;
}

namespace lay
{

class HelpSource;
class BrowserPanel;

/**
 *  @brief The help dialog (aka assistant)
 */
class HelpDialog 
  : public QDialog 
{
Q_OBJECT 

public:
  HelpDialog (QWidget *parent, bool modal = false);
  ~HelpDialog ();

  void search (const std::string &topic);
  void load (const std::string &url);
  void showEvent (QShowEvent *);
  void hideEvent (QHideEvent *);

protected slots:
  void title_changed (const QString &t);

private:
  Ui::HelpDialog *mp_ui;
  QRect m_geometry;
  static lay::HelpSource *mp_help_source;
  QString m_def_title;
  bool m_initialized;

  void initialize ();
};

}

#endif

