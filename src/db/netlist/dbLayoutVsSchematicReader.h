#ifndef HDR_dbLayoutVsSchematicReader
#define HDR_dbLayoutVsSchematicReader

#include "dbCommon.h"
#include "dbPolygon.h"
#include "dbCell.h"
#include "dbLayoutVsSchematic.h"
#include "dbLayoutToNetlistReader.h"
#include "tlStream.h"

namespace db {

class LayoutVsSchematic;
class Circuit;
class Cell;
class DeviceAbstract;
class DeviceClass;
class Net;
class Region;

/**
 *  @brief The base class for a LayoutVsSchematic writer
 */
class DB_PUBLIC LayoutVsSchematicReaderBase
{
public:
  LayoutVsSchematicReaderBase () { }
  virtual ~LayoutVsSchematicReaderBase () { }

  void read (db::LayoutVsSchematic *lvs)
  {
    do_read_lvs (lvs);
  }

protected:
  virtual void do_read_lvs (db::LayoutVsSchematic *lvs) = 0;
};

/**
 *  @brief The standard writer
 */
class DB_PUBLIC LayoutVsSchematicStandardReader
  : public LayoutVsSchematicReaderBase, protected LayoutToNetlistStandardReader
{
public:
  LayoutVsSchematicStandardReader (tl::InputStream &stream);

  void read (db::LayoutVsSchematic *lvs)
  {
    do_read_lvs (lvs);
  }

  virtual void do_read_lvs (db::LayoutVsSchematic *lvs);

private:
  void read_netlist (db::LayoutVsSchematic *lvs);

  bool read_status (db::NetlistCrossReference::Status &status);
  bool read_message (std::string &msg);
  void read_xref (db::NetlistCrossReference *xref);
  void read_xrefs_for_circuits (db::NetlistCrossReference *xref, const db::Circuit *circuit_a, const db::Circuit *circuit_b);
  void read_net_pair (db::NetlistCrossReference *xref, const db::Circuit *circuit_a, const db::Circuit *circuit_b);
  void read_pin_pair (db::NetlistCrossReference *xref, const db::Circuit *circuit_a, const db::Circuit *circuit_b);
  void read_device_pair (db::NetlistCrossReference *xref, const db::Circuit *circuit_a, const db::Circuit *circuit_b);
  void read_subcircuit_pair (db::NetlistCrossReference *xref, const db::Circuit *circuit_a, const db::Circuit *circuit_b);
  std::pair<std::string, bool> read_non ();
  std::pair<unsigned int, bool> read_ion ();

  std::map<const db::Circuit *, LayoutToNetlistStandardReader::ObjectMap> m_map_per_circuit_a, m_map_per_circuit_b;
};

}

#endif

