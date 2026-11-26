#ifndef HDR_layEditorServiceBase
#define HDR_layEditorServiceBase

#include "layEditable.h"
#include "layViewObject.h"
#include "layPlugin.h"
#include "layViewService.h"

namespace lay
{

/**
 *  @brief A generic base class for an editor service
 *
 *  This class offers common services such as a mouse cursor.
 */
class EditorServiceBase
  : public lay::ViewService,
    public lay::Editable,
    public lay::Plugin
{
public: 
  /**
   *  @brief Constructor
   */
  EditorServiceBase (lay::LayoutView *view);

  /**
   *  @brief Destructor
   */
  ~EditorServiceBase ();

  /**
   *  @brief Obtain the lay::ViewService interface
   */
  lay::ViewService *view_service_interface ()
  {
    return this;
  }

  /**
   *  @brief Obtain the lay::Editable interface
   */
  lay::Editable *editable_interface ()
  {
    return this;
  }

  /**
   *  @brief Adds a mouse cursor to the given point
   */
  void add_mouse_cursor (const geo::DPoint &pt, bool emphasize = false);

  /**
   *  @brief Adds an edge marker for the given edge
   */
  void add_edge_marker (const geo::DEdge &e, bool emphasize);

  /**
   *  @brief Resets the mouse cursor
   */
  void clear_mouse_cursors ();

  /**
   *  @brief Provides a nice mouse tracking cursor from the given snap details
   */
  void mouse_cursor_from_snap_details (const lay::PointSnapToObjectResult &snap_details);

  /**
   *  @brief Gets the tracking cursor color
   */
  QColor tracking_cursor_color () const
  {
    return m_cursor_color;
  }

  /**
   *  @brief Gets a value indicating whether the tracking cursor is enabled
   */
  bool tracking_cursor_enabled () const
  {
    return m_cursor_enabled;
  }

protected:
  virtual bool configure (const std::string &name, const std::string &value);
  virtual void deactivated ();

private:
  //  The marker representing the mouse cursor
  std::vector<lay::ViewObject *> m_mouse_cursor_markers;
  QColor m_cursor_color;
  bool m_cursor_enabled;
};

}

#endif

