#include "layRedrawLayerInfo.h"
#include "layLayerProperties.h"

namespace lay 
{

// -------------------------------------------------------------
//  Implementation of RedrawLayerInfo

RedrawLayerInfo::RedrawLayerInfo (const lay::LayerProperties &lp)
{
  visible          = lp.visible (true /*real*/);
  cell_frame       = lp.is_cell_box_layer ();
  xfill            = lp.xfill (true /*real*/);
  layer_index      = lp.layer_index ();
  cellview_index   = lp.cellview_index ();
  trans            = lp.trans ();
  hier_levels      = lp.hier_levels ();
  prop_sel         = lp.prop_sel ();
  inverse_prop_sel = lp.inverse_prop_sel ();
  enabled          = true;
}

}

