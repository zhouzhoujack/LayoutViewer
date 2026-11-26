#include "layStream.h"
#include "tlClassRegistry.h"
#include "layPlugin.h"
#include "laybasicConfig.h"
#include "layTechnology.h"
#include "detail/geoStream.h"
#include "geoLoadLayoutOptions.h"
#include "geoSaveLayoutOptions.h"
#include "tlAssert.h"
#include "tlString.h"

namespace lay
{

// ------------------------------------------------------------------
//  StreamPluginDeclarationBase implementation

geo::StreamFormatDeclaration &
StreamPluginDeclarationBase::stream_fmt () 
{
  if (! mp_stream_fmt) {
    for (tl::Registrar<geo::StreamFormatDeclaration>::iterator fmt = tl::Registrar<geo::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<geo::StreamFormatDeclaration>::end (); ++fmt) {
      if (fmt->format_name () == m_format_name) {
        mp_stream_fmt = const_cast <geo::StreamFormatDeclaration *> (&*fmt);
        break;
      }
    }
  }

  tl_assert (mp_stream_fmt);

  return *mp_stream_fmt;
}

// ------------------------------------------------------------------
//  StreamReaderPluginDeclaration implementation

const StreamReaderPluginDeclaration *StreamReaderPluginDeclaration::plugin_for_format (const std::string &format_name)
{
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamReaderPluginDeclaration *decl = dynamic_cast <const StreamReaderPluginDeclaration *> (&*cls);
    if (decl && decl->format_name () == format_name) {
      return decl;
    }
  }
  return 0;
}

// ------------------------------------------------------------------
//  StreamWriterPluginDeclaration implementation

const StreamWriterPluginDeclaration *StreamWriterPluginDeclaration::plugin_for_format (const std::string &format_name)
{
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamWriterPluginDeclaration *decl = dynamic_cast <const StreamWriterPluginDeclaration *> (&*cls);
    if (decl && decl->format_name () == format_name) {
      return decl;
    }
  }
  return 0;
}

}

