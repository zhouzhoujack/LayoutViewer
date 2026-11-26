#include "tlCopyOnWrite.h"

namespace tl
{
// -----------------------------------------------------------------------------
//  CopyOnWrite implementation


tl::Mutex tl::CopyOnWritePtrBase::ms_lock;

}
