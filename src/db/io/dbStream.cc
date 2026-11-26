#include "dbStream.h"
#include "tlClassRegistry.h"
#include "tlXMLParser.h"

#include <string.h>

namespace db
{

// ------------------------------------------------------------------
//  Implementation of load_options_xml_element_list


tl::XMLElementList load_options_xml_element_list ()
{
  tl::XMLElementList elements;

  for (tl::Registrar<db::StreamFormatDeclaration>::iterator cls = tl::Registrar<db::StreamFormatDeclaration>::begin (); cls != tl::Registrar<db::StreamFormatDeclaration>::end (); ++cls) {
    const db::StreamFormatDeclaration *decl = dynamic_cast <const db::StreamFormatDeclaration *> (&*cls);
    if (decl) {
      elements.append (decl->xml_reader_options_element ());
    }
  }

  return elements;
}

tl::XMLElementList save_options_xml_element_list ()
{
  tl::XMLElementList elements;

  for (tl::Registrar<db::StreamFormatDeclaration>::iterator cls = tl::Registrar<db::StreamFormatDeclaration>::begin (); cls != tl::Registrar<db::StreamFormatDeclaration>::end (); ++cls) {
    const StreamFormatDeclaration *decl = dynamic_cast <const StreamFormatDeclaration *> (&*cls);
    if (decl) {
      elements.append (decl->xml_writer_options_element ());
    }
  }

  return elements;
}

}


