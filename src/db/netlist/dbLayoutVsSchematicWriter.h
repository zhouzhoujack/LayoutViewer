#ifndef HDR_dbLayoutVsSchematicWriter
#define HDR_dbLayoutVsSchematicWriter

#include "dbCommon.h"
#include "dbLayoutToNetlistWriter.h"
#include "tlStream.h"

namespace db
{

class Circuit;
class Net;
class LayoutVsSchematic;
class NetlistCrossReference;

/**
 *  @brief The base class for a LayoutVsSchematic writer
 */
class DB_PUBLIC LayoutVsSchematicWriterBase
{
public:
  LayoutVsSchematicWriterBase ();
  virtual ~LayoutVsSchematicWriterBase ();

  void write (const db::LayoutVsSchematic *lvs);

protected:
  virtual void do_write_lvs (const db::LayoutVsSchematic *lvs) = 0;

private:
  std::string m_filename;
};

/**
 *  @brief The standard writer
 */
class DB_PUBLIC LayoutVsSchematicStandardWriter
  : public LayoutVsSchematicWriterBase
{
public:
  LayoutVsSchematicStandardWriter (tl::OutputStream &stream, bool short_version);

protected:
  void do_write_lvs (const db::LayoutVsSchematic *lvs);

private:
  tl::OutputStream *mp_stream;
  bool m_short_version;
};

}

#endif
