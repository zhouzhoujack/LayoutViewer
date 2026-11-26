#ifndef HDR_layMacroVariableView
#define HDR_layMacroVariableView

#include "gsiInspector.h"
#include "detail/utilsVariant.h"

#include <QTreeWidget>

#include <memory>

namespace tl
{
  class Variant;
}

namespace lay
{

/**
 *  @brief Utility: convert the variant into a nice string
 */
QString pretty_print (const tl::Variant &v);

/**
 *  @brief Provides a QTreeWidget that synchronizes with a gsi::Inspector object
 */
class MacroVariableView
  : public QTreeWidget
{
Q_OBJECT 

public:
  /**
   *  @brief Constructor
   */
  MacroVariableView (QWidget *parent = 0);

  /**
   *  @brief Attach an inspector to the view
   *
   *  The view will take over ownership over the inspector object.
   */
  void set_inspector (gsi::Inspector *inspector);

public slots:
  /**
   *  @brief Sets or resets the "show all" flag
   *
   *  If the "show all" flag is set, items with "IfRequested" visibility are shown too.
   */
  void set_show_all (bool show_all);

private slots:
  void expanded (QTreeWidgetItem *item);

private:
  void sync (QTreeWidgetItem *item, gsi::Inspector *inspector, bool fresh);
  void sync_item (QTreeWidgetItem *parent, gsi::Inspector *inspector, const QString &key, size_t index, int pos, bool fresh);
  void sync (bool fresh);

  std::unique_ptr<gsi::Inspector> mp_inspector;
  bool m_show_all;
};

}

#endif

