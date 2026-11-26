#ifndef HDR_layHelpProvider
#define HDR_layHelpProvider

#include <QDomDocument>
#include <string>

namespace lay
{

/**
 *  @brief A provider for documentation in the help system
 *
 *  A help provider is responsible for providing documention of a certain 
 *  category. That can be written documentation or generated documentation.
 *  Each help provider provides documents under a certain folder, i.e.
 *  "doc/..".
 *  It must be able to deliver a keyword list for the search system and 
 *  a DOM model for a given URL below that folder.
 */
class HelpProvider
{
public:
  /**
   *  @brief Constructor
   */
  HelpProvider ();

  /**
   *  @brief Destructor
   */
  virtual ~HelpProvider () { }

  /**
   *  @brief Gets the main entry page for this category
   *
   *  @return The documentation path for the main entry point for this provider.
   */
  virtual std::string index () const
  {
    return "/" + folder () + "/index.xml";
  }

  /**
   *  @brief Gets the DOM for a given URL
   *
   *  The DOM is the document in XML form which can be converted to HTML form for example
   *  or scanned for keywords.
   */
  virtual QDomDocument get (const std::string & /*path*/) const
  {
    return QDomDocument ();
  }
  
  /**
   *  @brief Delivers the folder name below which the help documents of this provider are located
   *
   *  If this string is "doc" for example, all help documents will be looked up under
   *  "doc/...".
   */
  virtual std::string folder () const = 0;

  /**
   *  @brief Gets the title for this category
   */
  virtual std::string title () const = 0;
};

}

#endif

