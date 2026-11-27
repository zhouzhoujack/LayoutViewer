#include "layTextProgress.h"
#include "detail/utilsProgress.h"
#include "tlLog.h"

namespace lay
{

// -----------------------------------------------------------------------------
//  TextProgress implementation

TextProgress::TextProgress (int verbosity)
  : m_verbosity (verbosity)
{
  //  .. nothing yet ..
}

void TextProgress::update_progress (geo::Progress *progress)
{
  if (! progress || progress->is_abstract ()) {
    return;
  }

  std::string text = progress->desc ();
  if (m_progress_text != text && toolkit::logkit::verbosity () >= m_verbosity) {
    LOGI << text << " ..";
    m_progress_text = text;
  }

  std::string value = progress->formatted_value ();
  for (geo::Progress *p = progress->next (); p != 0; p = p->next ()) {
    value += " " + p->formatted_value ();
  }

  if (m_progress_value != value && toolkit::logkit::verbosity () >= m_verbosity) {
    LOGI << ".. " << value;
    m_progress_value = value;
  }
}

void TextProgress::show_progress_bar (bool /*show*/)
{
  m_progress_text.clear ();
  m_progress_value.clear ();
}

}
