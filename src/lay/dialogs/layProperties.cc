#include "layProperties.h"

namespace lay
{

// -----------------------------------------------------------------------------
//  Properties implementation

PropertiesPage::PropertiesPage (QWidget *parent, geo::Manager *manager, lay::Editable *editable)
  : QFrame (parent), mp_manager (manager), mp_editable (editable)
{
  //  .. nothing else ..
}

PropertiesPage::~PropertiesPage ()
{
  if (mp_editable) {
    mp_editable->properties_page_deleted ();
  }
}

}

