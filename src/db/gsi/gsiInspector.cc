#include "gsiInspector.h"

namespace gsi
{

Inspector::Inspector ()
{
  //  .. the base class implementation does nothing ..
}

Inspector::~Inspector ()
{
  //  .. the base class implementation does nothing ..
}

std::string Inspector::description () const
{
  return std::string ();
}

bool Inspector::has_keys () const
{
  return true;
}

std::string Inspector::key (size_t /*index*/) const
{
  return std::string ();
}
  
tl::Variant Inspector::keyv (size_t /*index*/) const
{
  return tl::Variant ();
}

std::string Inspector::type (size_t /*index*/) const
{
  return std::string ();
}

Inspector::Visibility Inspector::visibility (size_t /*index*/) const
{
  return Always;
}

tl::Variant Inspector::value (size_t /*index*/) const
{
  return tl::Variant ();
}

size_t Inspector::count () const
{
  return 0;
}

bool Inspector::has_children (size_t /*index*/) const
{
  return false;
}

Inspector *Inspector::child_inspector (size_t /*index*/) const
{
  return 0;
}

bool Inspector::equiv (const gsi::Inspector * /*other*/) const
{
  return false;
}

}

