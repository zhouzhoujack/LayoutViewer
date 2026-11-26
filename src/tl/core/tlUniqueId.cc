#include "tlUniqueId.h"
#include "tlThreads.h"

namespace tl
{

static tl::Mutex s_lock;
static id_type s_next_id = 0;

// -----------------------------------------------------------------------------
//  UniqueId implementation

UniqueId::UniqueId ()
{
  tl::MutexLocker locker (&s_lock);
  do {
    m_id = ++s_next_id;
  } while (m_id == 0);
}

} // namespace tl


