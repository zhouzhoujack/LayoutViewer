#include "dbWriter.h"
#include "dbStream.h"
#include "tlClassRegistry.h"
#include "tlAssert.h"
#include "tlStream.h"
#include "tlTimer.h"
#include "tlLog.h"

namespace db
{

// -----------------------------------------------------------------------------
//  Writer implementation

Writer::Writer (const db::SaveLayoutOptions &options)
  : mp_writer (0), m_options (options)
{
  for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end () && ! mp_writer; ++fmt) {
    if (m_options.format () == fmt->format_name ()) {
      mp_writer = fmt->create_writer ();
    }
  }
  if (! mp_writer) {
    throw tl::Exception (tl::to_string (tr ("Unknown stream format: %s")), m_options.format ());
  }
}

Writer::~Writer ()
{
  if (mp_writer) {
    delete mp_writer;
  }
  mp_writer = 0;
}

void 
Writer::write (db::Layout &layout, tl::OutputStream &stream)
{
  tl::SelfTimer timer (toolkit::logkit::verbosity () >= 21, tl::to_string (tr ("Writing file: ")) + stream.path ());

  tl_assert (mp_writer != 0);
  mp_writer->write (layout, stream, m_options);
}

}

