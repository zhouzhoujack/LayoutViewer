#include "tlExceptions.h"

#include "tlException.h"
#include "tlInternational.h"
#include "tlLog.h"
#include "tlScriptError.h"

#include <stdexcept>

namespace tl
{
// -----------------------------------------------------------------------------
//  Exceptions implementation


static void (*s_ui_exception_handler_tl) (const tl::Exception &ex, QWidget *parent) = 0;
static void (*s_ui_exception_handler_std) (const std::exception &ex, QWidget *parent) = 0;
static void (*s_ui_exception_handler_def) (QWidget *parent) = 0;

void set_ui_exception_handlers (void (*handler_tl) (const tl::Exception &, QWidget *parent),
                                void (*handler_std) (const std::exception &, QWidget *parent),
                                void (*handler_def) (QWidget *parent))
{
  s_ui_exception_handler_tl = handler_tl;
  s_ui_exception_handler_std = handler_std;
  s_ui_exception_handler_def = handler_def;
}

void handle_exception_silent (const tl::Exception &ex)
{
  const tl::ScriptError *script_error = dynamic_cast <const tl::ScriptError *> (&ex);
  if (script_error) {
    if (script_error->line () > 0) {
      LOGE << script_error->sourcefile () << ":" << script_error->line () << ": " 
                << script_error->msg () << tl::to_string (tr (" (class ")) << script_error->cls () << ")";
    } else {
      LOGE << script_error->msg () << tl::to_string (tr (" (class ")) << script_error->cls () << ")";
    }
  } else {
    LOGE << ex.msg (); 
  }
}

void handle_exception (const tl::Exception &ex)
{
  handle_exception_ui (ex);
}

void handle_exception_ui (const tl::Exception &ex, QWidget *parent)
{
  if (s_ui_exception_handler_tl) {
    (*s_ui_exception_handler_tl) (ex, parent);
  } else {
    handle_exception_silent (ex);
  }
}

void handle_exception_silent (const std::exception &ex)
{
  LOGE << ex.what (); 
}

void handle_exception (const std::exception &ex)
{
  handle_exception_ui (ex);
}

void handle_exception_ui (const std::exception &ex, QWidget *parent)
{
  if (s_ui_exception_handler_std) {
    (*s_ui_exception_handler_std) (ex, parent);
  } else {
    handle_exception_silent (ex);
  }
}

void handle_exception_silent ()
{
  LOGE << tl::to_string (tr ("An unspecific error occurred"));
}

void handle_exception ()
{
  handle_exception_ui ();
}

void handle_exception_ui (QWidget *parent)
{
  if (s_ui_exception_handler_def) {
    (*s_ui_exception_handler_def) (parent);
  } else {
    handle_exception_silent ();
  }
}

}

