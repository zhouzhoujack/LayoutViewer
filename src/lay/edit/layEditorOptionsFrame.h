#ifndef HDR_layEditorOptionsFrame
#define HDR_layEditorOptionsFrame

#include <QtWidgets/QFrame>

namespace lay
{

class EditorOptionsPages;
class LayoutView;

class EditorOptionsFrame
  : public QFrame
{
public:
  EditorOptionsFrame (QWidget *parent);
  virtual ~EditorOptionsFrame ();

  void populate (LayoutView *view);

  EditorOptionsPages *pages_widget () const
  {
    return mp_pages;
  }

public:
  EditorOptionsPages *mp_pages;
};

}

#endif
