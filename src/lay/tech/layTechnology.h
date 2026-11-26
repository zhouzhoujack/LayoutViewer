#ifndef HDR_layTechnology
#define HDR_layTechnology


#include "detail/geoTechnology.h"

#include <QtWidgets/QFrame>

namespace lay
{

/**
 *  @brief A base class for an editor for a technology component
 *
 *  A technology component provider can provide an editor for the component
 *  To do so, it must implement a TechnologyComponentEditor that provides an 
 *  editor for it's component.
 */
class TechnologyComponentEditor
  : public QFrame
{
public:
  /**
   *  @brief The constructor
   */
  TechnologyComponentEditor (QWidget *parent)
    : QFrame (parent), mp_tech (0), mp_tech_component (0)
  {
    // .. nothing yet ..
  }

  /**
   *  @brief The destructor
   */
  virtual ~TechnologyComponentEditor ()
  {
    // .. nothing yet ..
  }

  /**
   *  @brief Set the technology and component for the editor
   */
  void set_technology (geo::Technology *tech, geo::TechnologyComponent *tech_component)
  {
    mp_tech = tech;
    mp_tech_component = tech_component;
  }

  /**
   *  @brief Sets up the editor with the given technology and component
   */
  virtual void setup () 
  {
    // .. nothing yet ..
  }

  /**
   *  @brief Commits the edits to the technology and component
   */
  virtual void commit () 
  {
    // .. nothing yet ..
  }

protected:
  geo::Technology *tech ()
  {
    return mp_tech;
  }

  geo::TechnologyComponent *tech_component ()
  {
    return mp_tech_component;
  }

private:
  geo::Technology *mp_tech;
  geo::TechnologyComponent *mp_tech_component;
};

/**
 *  @brief A base class for a technology component provider
 */
class TechnologyEditorProvider
{
public:
  /**
   *  @brief The constructor
   */
  TechnologyEditorProvider ()
  {
    // .. nothing yet ..
  }

  /**
   *  @brief The destructor
   */
  virtual ~TechnologyEditorProvider ()
  {
    // .. nothing yet ..
  }

  /**
   *  @brief Creates the technology component
   */
  virtual TechnologyComponentEditor *create_editor (QWidget * /*parent*/) const 
  {
    return 0;
  }
};

}

#endif

