#include "tlEvents.h"
#include "tlLog.h"
#include "tlCommon.h"

namespace tl
{
// -----------------------------------------------------------------------------
//  Events implementation


void handle_event_exception (tl::Exception &ex)
{
  //  Exceptions have unpredictable side effects for events so we just log
  //  them here.
  tl::warn << ex.msg ();
}

void handle_event_exception (std::exception &ex)
{
  //  Exceptions have unpredictable side effects for events so we just log
  //  them here.
  tl::warn << ex.what ();
}

}
