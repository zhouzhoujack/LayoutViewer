#ifndef HDR_layLayerMappingWidget
#define HDR_layLayerMappingWidget

#include <QtWidgets/QFrame>

#include "geoStreamLayers.h"

namespace Ui
{
  class LayerMappingWidget;
}

namespace lay
{

class FileDialog;

/**
 *  @brief A widget for editing the layer mapping for the reader options
 */
class LayerMappingWidget
  : public QFrame
{
Q_OBJECT

public:
  /**
   *  @brief Constructor
   */
  LayerMappingWidget (QWidget *parent);

  /**
   *  @brief Destructor
   */
  ~LayerMappingWidget ();

  /**
   *  @brief Set the layer mapping
   */
  void set_layer_map (const geo::LayerMap &lm);

  /**
   *  @brief Get the layer mapping
   */
  geo::LayerMap get_layer_map () const;

  /**
   *  @brief Get a value indicating whether the layer list is empty
   */
  bool is_empty () const;

signals:
  void layerListChanged ();
  void layerItemDeleted ();
  void layerItemAdded ();
  void enable_all_layers (bool en);

private slots:
  void load_button_pressed ();
  void add_button_pressed ();
  void delete_button_pressed ();
  void edit_button_pressed ();
  void current_tab_changed (int tab);

private:
  lay::FileDialog *mp_layer_table_file_dialog;
  std::string m_layer_table_file;
  Ui::LayerMappingWidget *mp_ui;

  geo::LayerMap get_layer_map_from_tab (int tab) const;
};

} // namespace lay

#endif

