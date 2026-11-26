#ifndef _HDR_dbPin
#define _HDR_dbPin

#include "dbCommon.h"
#include "dbNetlistObject.h"
#include "dbMemStatistics.h"

#include <string>

namespace db
{

/**
 *  @brief The definition of a pin of a circuit
 *
 *  A pin is some place other nets can connect to a circuit.
 */
class DB_PUBLIC Pin
  : public db::NetlistObject
{
public:
  /**
   *  @brief Default constructor
   */
  Pin ();

  /**
   *  @brief Creates a pin with the given name.
   */
  Pin (const std::string &name);

  /**
   *  @brief Gets the name of the pin
   */
  const std::string &name () const
  {
    return m_name;
  }

  /**
   *  @brief Gets a name which always non-empty
   *  This method will pick a name like "$<id>" if the explicit name is empty.
   */
  std::string expanded_name () const;

  /**
   *  @brief Gets the ID of the pin (only pins inside circuits have valid ID's)
   */
  size_t id () const
  {
    return m_id;
  }

  /**
   *  @brief Sets the name of the pin
   *  CAUTION: don't use this method on pins stored inside a netlist.
   */
  void set_name (const std::string &name)
  {
    m_name = name;
  }

  /**
   *  @brief Generate memory statistics
   */
  void mem_stat (MemStatistics *stat, MemStatistics::purpose_t purpose, int cat, bool no_self = false, void *parent = 0) const
  {
    if (! no_self) {
      stat->add (typeid (*this), (void *) this, sizeof (*this), sizeof (*this), parent, purpose, cat);
    }

    db::mem_stat (stat, purpose, cat, m_name, true, (void *) this);
  }

private:
  friend class Circuit;

  std::string m_name;
  size_t m_id;

  void set_id (size_t id)
  {
    m_id = id;
  }
};

/**
 *  @brief Memory statistics for Pin
 */
inline void mem_stat (MemStatistics *stat, MemStatistics::purpose_t purpose, int cat, const Pin &x, bool no_self, void *parent)
{
  x.mem_stat (stat, purpose, cat, no_self, parent);
}

}

#endif
