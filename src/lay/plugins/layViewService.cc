#include "layViewService.h"


#include "layViewObject.h"

namespace lay{

// ---------------------------------------------------------------
//  ViewService implementation

ViewService::ViewService (ViewObjectWidget *widget)
  : mp_widget (widget), m_abs_grab (false), m_enabled (true)
{
  if (widget) {
    widget->register_service (this);
  }
}

ViewService::~ViewService ()
{
  if (mp_widget) {
    mp_widget->unregister_service (this);
  }
  mp_widget = 0;
}

void
ViewService::enable (bool en)
{
  m_enabled = en;
}

void 
ViewService::set_cursor (lay::Cursor::cursor_shape cursor)
{
  mp_widget->set_cursor (cursor);
}

}
