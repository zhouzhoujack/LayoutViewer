#ifndef HDR_layViewObject
#define HDR_layViewObject


#include <list>
#include "detail/utilsObjectCollection.h"
#include "detail/utilsObject.h"
#include "detail/utilsVariant.h"
#include <string>
#include <vector>
#include <map>

#include <QPoint>
#include <QByteArray>
#include <QColor>
#include <QtWidgets/QWidget>

#include "tlObjectCollection.h"
#include "tlVariant.h"
#include "detail/geoTrans.h"
#include "geoBox.h"
#include "layViewOp.h"
#include "layCursor.h"
#include "layBitmapRenderer.h"
#include "layViewObjectCanvas.h"

class QMouseEvent;
class QImage;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QMimeData;

namespace geo
{
  class Library;
  class Layout;
}

namespace lay {

class Viewport;
class ViewObjectWidget;
class ViewObjectCanvas;
class CanvasPlane;
class Bitmap;
class ViewService;

const char *drag_drop_mime_type ();

/**
 *  @brief A helper class required to store the drag/drop data
 *
 *  Drag/drop data is basically a collection of key/value pairs. 
 *  A category string is provided to identify the kind of data.
 */

class DragDropDataBase
{
public:
  /**
   *  @brief Default constructor
   */
  DragDropDataBase () { }

  /**
   *  @brief Dtor
   */
  virtual ~DragDropDataBase () { } 

  /**
   *  @brief Serializes itself to an QByteArray
   */
  virtual QByteArray serialized () const = 0;

  /**
   *  @brief Try deserialization from an QByteArray
   *
   *  Returns false, if deserialization failed.
   */
  virtual bool deserialize (const QByteArray &ba) = 0;

  /**
   *  @brief Create a QMimeData object from the object
   */
  QMimeData *to_mime_data () const;
};

/**
 *  @brief Drag/drop data for a cell
 */

class CellDragDropData
  : public DragDropDataBase
{
public:
  /**
   *  @brief Default ctor
   */
  CellDragDropData ()
    : mp_layout (0), mp_library (0), m_cell_index (0), m_is_pcell (false)
  {
    //  .. nothing yet ..
  }

  /**
   *  @brief Specifies drag & drop of a cell
   *
   *  @param layout the layout where the cell lives in 
   *  @param cell_index The index of the cell
   */
  CellDragDropData (const geo::Layout *layout, const geo::Library *library, geo::cell_index_type cell_or_pcell_index, bool is_pcell, const std::vector<geo::Variant> &pcell_params = std::vector<geo::Variant> ())
    : mp_layout (layout), mp_library (library), m_cell_index (cell_or_pcell_index), m_is_pcell (is_pcell), m_pcell_params (pcell_params)
  {
    //  .. nothing yet ..
  }

  /**
   *  @brief Gets the layout object where the cell lives in
   */
  const geo::Layout *layout () const
  {
    return mp_layout;
  }

  /**
   *  @brief Gets the layout object where the cell lives in
   */
  const geo::Library *library () const
  {
    return mp_library;
  }

  /**
   *  @brief PCell parameters
   */
  const std::vector<geo::Variant> &pcell_params () const
  {
    return m_pcell_params;
  }

  /**
   *  @brief Gets the index of the cell
   */
  geo::cell_index_type cell_index () const
  {
    return m_cell_index;
  }

  /**
   *  @brief Gets a value indicating whether the cell is a pcell
   */
  bool is_pcell () const
  {
    return m_is_pcell;
  }

  /**
   *  @brief Serializes itself to an QByteArray
   */
  virtual QByteArray serialized () const;

  /**
   *  @brief Try deserialization from an QByteArray
   *
   *  Returns false, if deserialization failed.
   */
  bool deserialize (const QByteArray &ba);

private:
  const geo::Layout *mp_layout;
  const geo::Library *mp_library;
  geo::cell_index_type m_cell_index;
  bool m_is_pcell;
  std::vector<geo::Variant> m_pcell_params;
};


/**
 *  @brief The background view object 
 *
 *  A background view object is an object that is painted onto the
 *  canvas background and is contained by the view object widget. 
 *  This is an interface that should/must be implemented
 *  by the super class.
 */

class BackgroundViewObject
  : virtual public geo::RefCounted
{
public: 
  /**
   *  @brief The constructor
   *
   *  @param widget The widget object that the object is shown on.
   *  @param _static True, if the object is in frozen mode initially
   */
  BackgroundViewObject (ViewObjectWidget *widget = 0);

  /**
   *  @brief The destructor
   */
  virtual ~BackgroundViewObject ();

  /**
   *  @brief Render the object on the background 
   *
   *  This method is supposed to repaint the background part of the object 
   *  on the QImage provided in the canvas.
   */
  virtual void render_bg (const Viewport &vp, ViewObjectCanvas &canvas) = 0;

  /**
   *  @brief Accessor to the widget object pointer
   */
  ViewObjectWidget *widget () const 
  {  
    return const_cast<ViewObjectWidget *> (mp_widget.get());
  }

  /**
   *  @brief Set the visibility state of the view object
   *  
   *  Invisible objects are not drawn
   */
  void visible (bool vis);
     
  /**
   *  @brief Tell the visibility state of the view object
   */
  bool is_visible () const
  {
    return m_visible;
  }
     
  /**
   *  @brief This method tells the widget to update the object on next repaint
   *  
   *  If the object is static, a "touch" is issued on the widget. Otherwise
   *  just a update is issued, resulting in a repaint
   *  This is the preferred method to tell that an object needs repainting.
   *  It can be called multiple times without performance penalty.
   */
  void redraw ();

  /**
   *  @brief Z-Order property
   *
   *  This property controls in which order the background objects are drawn: 
   *  those with a lower z-order value are drawn first. Thus, ones with the 
   *  higher value overwrite them.
   */
  int z_order () const
  {
    return m_z_order;
  }

  /**
   *  @brief Z-Order write accessor
   */
  void z_order (int z);

private:
  friend class ViewObjectWidget;

  BackgroundViewObject (const BackgroundViewObject &d);
  BackgroundViewObject &operator= (const BackgroundViewObject &d);

  geo::weak_ptr<ViewObjectWidget> mp_widget;
  bool m_visible;
  int m_z_order;
};

/**
 *  @brief The view object 
 *
 *  A view object is an object that is painted onto the
 *  canvas and is contained by the view object widget. 
 *  View objects can be static or non-static. Changing a
 *  view object in the non-static case is somewhat more 
 *  efficient and is recommended for dynamic objects.
 *  Static mode is entered with "freeze", non-static mode
 *  with "thaw".
 *  This is an interface that should/must be implemented
 *  by the super class.
 */

class ViewObject
  : public geo::RefCounted
{
public: 
  /**
   *  @brief The constructor
   *
   *  @param widget The widget object that the object is shown on.
   *  @param _static True, if the object is in frozen mode initially
   */
  ViewObject (ViewObjectWidget *widget = 0, bool _static = true);

  /**
   *  @brief The destructor
   */
  virtual ~ViewObject ();

  /**
   *  @brief Render the object on the planes provided by the canvas.
   *
   *  This method is supposed to repaint the object on the plane
   *  objects that the canvas can provide upon request with the plane
   *  method.
   */
  virtual void render (const Viewport &vp, ViewObjectCanvas &canvas) = 0;

  /**
   *  @brief Accessor to the widget object pointer
   */
  ViewObjectWidget *widget () const 
  {  
    return const_cast<ViewObjectWidget *> (mp_widget.get());
  }

  /**
   *  @brief Gets a value indicating whether the marker can be dismissed (made invisible)
   *
   *  Markers with this flag set to true can be hidden by using ViewObjectCanvas::show_markers.
   */
  bool get_dismissable () const
  {
    return m_dismissable;
  }

  /**
   *  @brief Sets a value indicating whether the marker can be dismissed (made invisible)
   *
   *  See \\get_dismissable for details.
   */
  void set_dismissable (bool f);

  /**
   *  @brief Set the visibility state of the view object
   *  
   *  Invisible objects are not drawn
   */
  void visible (bool vis);
     
  /**
   *  @brief Tell the visibility state of the view object
   */
  bool is_visible () const
  {
    return m_visible;
  }
     
  /**
   *  @brief This method tells the widget to update the object on next repaint
   *  
   *  If the object is static, a "touch" is issued on the widget. Otherwise
   *  just a update is issued, resulting in a repaint
   *  This is the preferred method to tell that an object needs repainting.
   *  It can be called multiple times without performance penalty.
   */
  void redraw ();

  /**
   *  @brief thaw this object
   *
   *  This is a convenience function that avoids having to store a widget pointer
   */
  void thaw (); 

  /**
   *  @brief freeze this object
   *
   *  This is a convenience function that avoids having to store a widget pointer
   */
  void freeze ();

private:
  friend class ViewObjectWidget;

  ViewObject (const ViewObject &d);
  ViewObject &operator= (const ViewObject &d);

  geo::weak_ptr<ViewObjectWidget> mp_widget;
  bool m_static;
  bool m_visible;
  bool m_dismissable;
};

/**
 *  @brief Describes the button state (supposed to be ored)
 */
enum ButtonState {
  ShiftButton = 1,
  ControlButton = 2,
  AltButton = 4,
  LeftButton = 8,
  MidButton = 16,
  RightButton = 32
};

/**
 *  @brief The view object container
 *
 *  The container holds the view objects and 
 *  manages the the mouse event distribution and 
 *  painting.
 */

class ViewObjectWidget 
  : public QWidget,
    public geo::RefCounted
{
public:
  typedef geo::weak_collection<ViewObject>::iterator object_iterator;
  typedef geo::weak_collection<BackgroundViewObject>::iterator background_object_iterator;
  typedef std::list<ViewService *>::const_iterator service_iterator;
  typedef std::list<ViewService *>::const_iterator mouse_receivers_iterator;

  /**
   *  @brief ctor
   */
  ViewObjectWidget (QWidget *view, const char *name);

  /**
   *  @brief dtor
   */
  ~ViewObjectWidget ();

  /**
   *  @brief Cancel all drag operations
   */
  void drag_cancel ();

  /**
   *  @brief CanvasPlane rendering 
   *
   *  This method is supposed to render the object on the planes provided
   *  by the planes() method. 
   *
   *  @param vp The viewport 
   *  @param st True, if only static (frozen) objects are to be redrawn
   */
  void do_render (const Viewport &vp, ViewObjectCanvas &canvas, bool st);

  /**
   *  @brief Background rendering
   *
   *  Objects on the background must provide a paint method that paints
   *  them on the canvas directly.
   *  
   *  @param vp The viewport 
   *  @param canvas The canvas where to paint on
   */
  void do_render_bg (const Viewport &vp, ViewObjectCanvas &canvas);

  /**
   *  @brief Query if any "static" object needs to be redrawn
   *
   *  To reset this flag, call do_render with "st" set to true.
   */
  bool needs_update_static () const
  {
    return m_needs_update_static;
  }

  /**
   *  @brief Query if the background needs to be redrawn
   *
   *  To reset this flag, call do_render_bg 
   */
  bool needs_update_bg () const
  {
    return m_needs_update_bg;
  }

  /**
   *  @brief Puts the object into "static" mode
   */
  void freeze (ViewObject *obj);

  /**
   *  @brief Puts the object into "non-static" mode
   */
  void thaw (ViewObject *obj);

  /**
   *  @brief Marks the widget for update
   *
   *  Beside issuing an "update" request, the widget is also
   *  marked for "static-update", which shall redraw all static
   *  objects also.
   */
  void touch ();

  /**
   *  @brief Marks the widget background for update
   *
   *  Beside issuing an "update" request, the widget is also
   *  marked for "bg-update", which shall redraw including the background.
   */
  void touch_bg ();

  /**
   *  @brief Grab the mouse for the object
   *
   *  This will put the object into the list of objects interested
   *  into mouse events with high priority (prio = true on mouse_* events).
   *  Objects with dragging semantics (press-drag-release) may request
   *  "absolute" mouse grab. This way, all mouse events are passed to the
   *  object. The grab should then in any case be released on mouse button
   *  release.
   */
  void grab_mouse (ViewService *obj, bool abs);

  /**
   *  @brief Remove the object from the list of objects that grabbed the mouse
   *
   *  This will also remove any absolute grabs if there is no other object
   *  that needs this.
   */
  void ungrab_mouse (ViewService *obj);

  /**
   *  @brief Set the mouse cursor
   *
   *  This method can be called from a mouse event handler to set the cursor for the view.
   *  It is only effective if called from a mouse event handler.
   */
  void set_cursor (lay::Cursor::cursor_shape cursor);

  /**
   *  @brief Determine the active service
   */
  lay::ViewService *active_service () const
  {
    return const_cast<lay::ViewService *> (mp_active_service);
  }

  /**
   *  @brief Activate a service
   *
   *  The active service will be the first to receive mouse events  
   *  with prio==true, unless the mouse is grabbed and the event is
   *  taken by the grabber. 
   *  Passing 0 deactivates all services. The service currently active
   *  will get a "deactivated" signal.
   */
  void activate (lay::ViewService *service);

  /**
   *  @brief Services iterator: begin
   */
  service_iterator begin_services () 
  {
    return m_services.begin ();
  }

  /**
   *  @brief Services iterator: end
   */
  service_iterator end_services () 
  {
    return m_services.end ();
  }

  /**
   *  @brief Background objects iterator: begin
   */
  background_object_iterator begin_background_objects () 
  {
    return m_background_objects.begin ();
  }

  /**
   *  @brief Background objects iterator: end
   */
  background_object_iterator end_background_objects () 
  {
    return m_background_objects.end ();
  }

  /**
   *  @brief Objects iterator: begin
   */
  object_iterator begin_objects () 
  {
    return m_objects.begin ();
  }

  /**
   *  @brief Objects iterator: end
   */
  object_iterator end_objects () 
  {
    return m_objects.end ();
  }

  /** 
   *  @brief Remaining leave event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void leave_event () { }

  /** 
   *  @brief Remaining enter event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void enter_event () { }

  /** 
   *  @brief Remaining key event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void key_event (unsigned int /*key*/, unsigned int /*buttons*/) { }

  /**
   *  @brief The drag enter event
   *
   *  If something is dragged into the view, this event is triggered.
   *  p will be the position of the event.
   *
   *  The implementation should return true, if the drop is accepted.
   */
  virtual bool drag_enter_event (const geo::DPoint & /*p*/, const DragDropDataBase * /*data*/) { return false; }

  /**
   *  @brief The drag move event
   */
  virtual bool drag_move_event (const geo::DPoint & /*p*/, const DragDropDataBase * /*data*/) { return false; }

  /**
   *  @brief The drag leave event
   */
  virtual void drag_leave_event () { }

  /**
   *  @brief The drop event
   */
  virtual bool drop_event (const geo::DPoint & /*p*/, const DragDropDataBase * /*data*/) { return false; }

  /** 
   *  @brief Remaining mouse double click event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void mouse_double_click_event (const geo::DPoint & /*p*/, unsigned int /*buttons*/) { }

  /** 
   *  @brief Remaining single mouse click event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void mouse_click_event (const geo::DPoint & /*p*/, unsigned int /*buttons*/) { }

  /** 
   *  @brief Remaining mouse press event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void mouse_press_event (const geo::DPoint & /*p*/, unsigned int /*buttons*/) { }

  /** 
   *  @brief Remaining mouse release event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void mouse_release_event (const geo::DPoint & /*p*/, unsigned int /*buttons*/) { }

  /** 
   *  @brief Remaining mouse move event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void mouse_move_event (const geo::DPoint & /*p*/, unsigned int /*buttons*/) { }

  /** 
   *  @brief Remaining wheel event handler
   *
   *  This event handler is called if no ViewObject requested handling
   *  of this event.
   */
  virtual void wheel_event (int /*delta*/, bool /*horizontal*/, const geo::DPoint & /*p*/, unsigned int /*buttons*/) { }

  /**
   *  @brief Get the transformation for mouse events
   */
  const geo::DCplxTrans &mouse_event_trans () const
  {
    return m_trans;
  }

  /** 
   *  @brief Deliver an iterator for the mouse receivers (begin)
   */
  mouse_receivers_iterator begin_mouse_receivers () const
  {
    return m_grabbed.begin ();
  }

  /** 
   *  @brief Deliver an iterator for the mouse receivers (end)
   */
  mouse_receivers_iterator end_mouse_receivers () const
  {
    return m_grabbed.end ();
  }

  /**
   *  @brief Get the viewport for mouse events
   */
  geo::DBox mouse_event_viewport () const;

  /**
   *  @brief Set the default cursor
   *
   *  The default cursor is shown when no mouse event sets the cursor.
   */
  void set_default_cursor (lay::Cursor::cursor_shape cursor);

  /**
   *  @brief Sets a value indicating whether dismissable view objects shall be drawn or not
   *
   *  Markers with dismissable = false are always drawn. The default value is "false".
   */
  void set_dismiss_view_objects (bool dismissed);

  /**
   *  @brief Gets a value indicating whether dismissable markers shall be drawn or not
   */
  bool dismiss_view_objects () const
  {
    return m_view_objects_dismissed;
  }

  /**
   *  @brief Gets the current mouse position
   */
  QPoint mouse_position () const
  {
    return m_mouse_pos;
  }

  /**
   *  @brief Gets the current mouse position in micrometer units
   */
  geo::DPoint mouse_position_um () const
  {
    return pixel_to_um (m_mouse_pos);
  }

  /**
   *  @brief Translates a screen coordinate in micrometer coordinates
   */
  geo::DPoint pixel_to_um (const QPoint &pt) const;

  /**
   *  @brief Gets a flag indicating whether the mouse is inside the window
   */
  bool mouse_in_window () const
  {
    return m_mouse_inside;
  }

protected:
  /**
   *  @brief Qt focus event handler
   */
  bool focusNextPrevChild (bool next);

  /**
   *  @brief Qt keyboard event handler
   */
  void keyPressEvent (QKeyEvent *e);

  /**
   *  @brief Qt mouse move event handler
   */
  void mouseMoveEvent (QMouseEvent *e);

  /**
   *  @brief Qt mouse leave event handler
   */
  void leaveEvent (QEvent *e);

  /**
   *  @brief Qt mouse enter event handler
   */
  void enterEvent (QEvent *e);

  /**
   *  @brief Qt mouse button press event handler
   */
  void mousePressEvent (QMouseEvent *e);

  /**
   *  @brief Qt mouse button double-click event handler
   */
  void mouseDoubleClickEvent (QMouseEvent *e);

  /**
   *  @brief Qt mouse button release event handler
   */
  void mouseReleaseEvent (QMouseEvent *e);

  /**
   *  @brief Qt drag enter event handler
   */
  void dragEnterEvent (QDragEnterEvent *event);

  /**
   *  @brief Qt drag leave event handler
   */
  void dragLeaveEvent (QDragLeaveEvent *event);

  /**
   *  @brief Qt drag enter event handler
   */
  void dragMoveEvent (QDragMoveEvent *event);

  /**
   *  @brief Qt drop event handler
   */
  void dropEvent (QDropEvent *event);

  /**
   *  @brief Qt mouse wheel event handler
   */
  void wheelEvent (QWheelEvent *e);

  /**
   *  @brief Set the transformation for mouse events
   */
  void mouse_event_trans (const geo::DCplxTrans &trans);

private:
  friend class lay::ViewObject;
  friend class lay::ViewService;
  friend class lay::BackgroundViewObject;

  geo::weak_collection<lay::ViewObject> m_objects;
  geo::weak_collection<lay::BackgroundViewObject> m_background_objects;
  std::list<lay::ViewService *> m_services;
  std::list<ViewService *> m_grabbed;
  bool m_view_objects_dismissed;
  bool m_needs_update_static;
  bool m_needs_update_bg;
  lay::ViewService *mp_active_service;
  geo::DCplxTrans m_trans;
  QPoint m_mouse_pos;
  QPoint m_mouse_pressed;
  bool m_mouse_pressed_state;
  unsigned int m_mouse_buttons;
  bool m_in_mouse_move;
  bool m_mouse_inside;
  lay::Cursor::cursor_shape m_cursor, m_default_cursor;

  void ensure_entered ();
  void do_mouse_move ();
  void begin_mouse_event (lay::Cursor::cursor_shape cursor = lay::Cursor::keep);
  void end_mouse_event ();
  void objects_changed ();

  /**
   *  @brief Register a service
   *
   *  This method is called by the service constructor and registers the service in the widget.
   */
  void register_service (lay::ViewService *);

  /**
   *  @brief Register a service
   *
   *  This method is called by the service destructor and unregisters the service in the widget.
   */
  void unregister_service (lay::ViewService *svc);
};

} // namespace lay

#endif

