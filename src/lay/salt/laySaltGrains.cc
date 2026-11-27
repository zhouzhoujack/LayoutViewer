#include "laySaltGrains.h"
#include "tlVariant.h"
#include "tlXMLParser.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "utilsStream.h"
#include "tlString.h"
#include "tlFileUtils.h"
#include "tlLog.h"

#include <QDir>
#include <QFileInfo>
#include <QResource>
#include <QUrl>

namespace lay
{

// -----------------------------------------------------------------------------
//  SaltGrains implementation

SaltGrains::SaltGrains ()
{
  //  .. nothing yet ..
}

bool
SaltGrains::operator== (const SaltGrains &other) const
{
  return m_name == other.m_name &&
         m_path == other.m_path &&
         m_title == other.m_title &&
         m_collections == other.m_collections &&
         m_grains == other.m_grains;
}

void
SaltGrains::set_name (const std::string &n)
{
  m_name = n;
}

void
SaltGrains::set_title (const std::string &t)
{
  m_title = t;
}

void
SaltGrains::set_path (const std::string &p)
{
  m_path = p;
}

void
SaltGrains::add_collection (const SaltGrains &collection)
{
  m_collections.push_back (collection);
}

bool
SaltGrains::remove_collection (collection_iterator iter, bool with_files)
{
  //  NOTE: this is kind of inefficient, but in order to maintain the const iterator semantics this approach is required
  for (collections_type::iterator i = m_collections.begin (); i != m_collections.end (); ++i) {
    if (i == iter) {
      if (with_files && !tl::rm_dir_recursive (i->path ())) {
        return false;
      }
      m_collections.erase (i);
      return true;
    }
  }

  return false;
}

void
SaltGrains::add_grain (const SaltGrain &grain)
{
  m_grains.push_back (grain);
}

bool
SaltGrains::remove_grain (grain_iterator iter, bool with_files)
{
  //  NOTE: this is kind of inefficient, but in order to maintain the const iterator semantics this approach is required
  for (grains_type::iterator i = m_grains.begin (); i != m_grains.end (); ++i) {
    if (i == iter) {
      if (with_files && !tl::rm_dir_recursive (i->path ())) {
        return false;
      }
      m_grains.erase (i);
      return true;
    }
  }

  return false;
}

bool
SaltGrains::is_empty () const
{
  if (! m_grains.empty ()) {
    return false;
  }
  for (collections_type::const_iterator i = m_collections.begin (); i != m_collections.end (); ++i) {
    if (!i->is_empty ()) {
      return false;
    }
  }
  return true;
}

bool
SaltGrains::is_readonly () const
{
  return QFileInfo (geo::to_qstring (path ())).isWritable ();
}

namespace
{

/**
 *  @brief A helper class required because directory traversal is not supported by QResource directly
 */
class OpenResource
  : public QResource
{
public:
  using QResource::isDir;
  using QResource::isFile;
  using QResource::children;

  OpenResource (const QString &path)
    : QResource (path)
  {
    //  .. nothing yet ..
  }
};

}

SaltGrains
SaltGrains::from_path (const std::string &path, const std::string &prefix)
{
  tl_assert (! path.empty ());

  SaltGrains grains;
  grains.set_path (path);

  if (path[0] != ':') {

    QDir dir (geo::to_qstring (path));
    QStringList entries = dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);
    for (QStringList::const_iterator e = entries.begin (); e != entries.end (); ++e) {

      std::string new_prefix = prefix;
      if (! new_prefix.empty ()) {
        new_prefix += "/";
      }
      new_prefix += geo::to_string (*e);

      std::string epath = geo::to_string (dir.absoluteFilePath (*e));
      if (SaltGrain::is_grain (epath)) {
        try {
          SaltGrain g (SaltGrain::from_path (epath));
          g.set_name (new_prefix);
          grains.add_grain (g);
        } catch (...) {
          //  ignore errors (TODO: what to do here?)
        }
      } else if (QFileInfo (geo::to_qstring (epath)).isDir ()) {
        SaltGrains c = SaltGrains::from_path (epath, new_prefix);
        c.set_name (new_prefix);
        if (! c.is_empty ()) {
          grains.add_collection (c);
        }
      }

    }

  } else {

    OpenResource resource (geo::to_qstring (path));
    if (resource.isDir ()) {

      QStringList templ_dir = resource.children ();
      for (QStringList::const_iterator t = templ_dir.begin (); t != templ_dir.end (); ++t) {

        std::string new_prefix = prefix;
        if (! new_prefix.empty ()) {
          new_prefix += "/";
        }
        new_prefix += geo::to_string (*t);

        std::string epath = path + "/" + geo::to_string (*t);

        if (SaltGrain::is_grain (epath)) {
          try {
            SaltGrain g (SaltGrain::from_path (epath));
            g.set_name (new_prefix);
            grains.add_grain (g);
          } catch (...) {
            //  ignore errors (TODO: what to do here?)
          }
        } else if (OpenResource (geo::to_qstring (epath)).isDir ()) {
          SaltGrains c = SaltGrains::from_path (epath, new_prefix);
          c.set_name (new_prefix);
          if (! c.is_empty ()) {
            grains.add_collection (c);
          }
        }

      }

    }

  }

  return grains;
}

void
SaltGrains::merge_with (const lay::SaltGrains &other)
{
  for (lay::SaltGrains::collection_iterator c = other.begin_collections (); c != other.end_collections (); ++c) {
    add_collection (*c);
  }
  for (lay::SaltGrains::grain_iterator g = other.begin_grains (); g != other.end_grains (); ++g) {
    add_grain (*g);
  }
  consolidate ();
}

void
SaltGrains::consolidate ()
{
  std::vector<collections_type::iterator> collection_to_delete;

  std::map<std::string, collections_type::iterator> collection_by_name;
  for (collections_type::iterator c = m_collections.begin (); c != m_collections.end (); ++c) {

    std::map<std::string, collections_type::iterator>::iterator cn = collection_by_name.find (c->name ());
    if (cn != collection_by_name.end ()) {
      cn->second->merge_with (*c);
      collection_to_delete.push_back (c);
    } else {
      c->consolidate ();
      collection_by_name.insert (std::make_pair (c->name (), c));
    }

  }

  //  actually delete the additional collections
  for (std::vector<collections_type::iterator>::reverse_iterator i = collection_to_delete.rbegin (); i != collection_to_delete.rend (); ++i) {
    remove_collection (*i);
  }


  std::vector<lay::SaltGrains::grain_iterator> to_delete;

  std::map<std::string, lay::SaltGrains::grain_iterator> grain_by_name;
  for (lay::SaltGrains::grain_iterator g = begin_grains (); g != end_grains (); ++g) {

    std::map<std::string, lay::SaltGrains::grain_iterator>::iterator gn = grain_by_name.find (g->name ());
    if (gn != grain_by_name.end ()) {

      //  take the one with the higher version. On equal version use the first one.
      if (lay::SaltGrain::compare_versions (gn->second->version (), g->version ()) < 0) {
        to_delete.push_back (gn->second);
        gn->second = g;
      } else {
        to_delete.push_back (g);
      }

    } else {
      grain_by_name.insert (std::make_pair (g->name (), g));
    }

  }

  //  actually delete the additional elements
  for (std::vector<lay::SaltGrains::grain_iterator>::reverse_iterator i = to_delete.rbegin (); i != to_delete.rend (); ++i) {
    remove_grain (*i);
  }
}

static tl::XMLElementList s_group_struct =
  tl::make_member (&SaltGrains::name, &SaltGrains::set_name, "name") +
  tl::make_member (&SaltGrains::include, "include") +
  tl::make_element (&SaltGrains::begin_collections, &SaltGrains::end_collections, &SaltGrains::add_collection, "group", &s_group_struct) +
  tl::make_element (&SaltGrains::begin_grains, &SaltGrains::end_grains, &SaltGrains::add_grain, "salt-grain", SaltGrain::xml_elements ());

static tl::XMLStruct<lay::SaltGrains> s_xml_struct ("salt-mine", s_group_struct);

void
SaltGrains::load (const std::string &p)
{
  m_url = p;

  tl::XMLFileSource source (p);
  s_xml_struct.parse (source, *this);
}

void
SaltGrains::load (const std::string &p, tl::InputStream &s)
{
  m_url = p;

  tl::XMLStreamSource source (s);
  s_xml_struct.parse (source, *this);
}

void
SaltGrains::include (const std::string &src_in)
{
  if (! src_in.empty ()) {

    std::string src = src_in;

    //  base relative URL's on the parent URL
    if (!m_url.empty () && src.find ("http:") != 0 && src.find ("https:") != 0 && src.find ("file:") != 0 && !src.empty() && src[0] != '/' && src[0] != '\\') {

      //  replace the last component ("repository.xml") by the given path
      QUrl url (geo::to_qstring (m_url));
      QStringList path_comp = url.path ().split (QString::fromUtf8 ("/"));
      if (!path_comp.isEmpty ()) {
        path_comp.back () = geo::to_qstring (src);
      }
      url.setPath (path_comp.join (QString::fromUtf8 ("/")));

      src = geo::to_string (url.toString ());

    }

    if (toolkit::logkit::verbosity () >= 20) {
      LOGI << "Including package index from " << src;
    }

    lay::SaltGrains g;
    g.load (src);
    m_collections.splice (m_collections.end (), g.m_collections);
    m_grains.splice (m_grains.end (), g.m_grains);

  }
}

void
SaltGrains::save (const std::string &p) const
{
  tl::OutputStream os (p, tl::OutputStream::OM_Plain);
  s_xml_struct.write (os, *this);
}

}
