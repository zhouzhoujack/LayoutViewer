#include "laySelectLineStyleForm.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "ui_SelectLineStyleForm.h"

#include "tlString.h"

#include <QBitmap>

#include <algorithm>

namespace lay
{

// -----------------------------------------------------------------------------
//  SelectLineStyleForm implementation

SelectLineStyleForm::SelectLineStyleForm (QWidget *parent, const lay::LineStyles &styles, bool include_nil)
  : QDialog (parent), m_selected (-1), m_styles (styles), m_include_nil (include_nil)
{
  mp_ui = new Ui::SelectLineStyleForm ();

  mp_ui->setupUi (this);

  mp_ui->style_items->setUniformItemSizes (true);

  update ();

  connect (mp_ui->style_items, SIGNAL (currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
           this, SLOT (sel_changed (QListWidgetItem *, QListWidgetItem *)));
}

SelectLineStyleForm::~SelectLineStyleForm ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
SelectLineStyleForm::set_selected (int selected)
{
  if (selected != m_selected) {
    m_selected = selected;
    mp_ui->style_items->setCurrentRow (m_include_nil ? (selected < 0 ? 0 : selected + 1) : selected);
  }
}

namespace {
  struct display_order
  {
    bool operator () (lay::LineStyles::iterator a, lay::LineStyles::iterator b)
    {
      return a->order_index () < b->order_index ();
    }
  };
}

void 
SelectLineStyleForm::update ()
{
  mp_ui->style_items->clear ();

  if (m_include_nil) {
    new QListWidgetItem (QObject::tr ("None"), mp_ui->style_items);
  }

  std::vector <lay::LineStyles::iterator> iters;
  for (lay::LineStyles::iterator i = m_styles.begin_custom (); i != m_styles.end (); ++i) {
    iters.push_back (i);
  }
  std::sort (iters.begin (), iters.end (), display_order ());

  //  fill the list of stipple items
  for (lay::LineStyles::iterator i = m_styles.begin (); i != m_styles.begin_custom (); ++i) {
    std::string name (i->name ());
    if (name.empty ()) {
      name = geo::sprintf ("#%d", std::distance (m_styles.begin (), i));
    }
    new QListWidgetItem (QIcon (i->get_bitmap (36, 26)), geo::to_qstring (name), mp_ui->style_items);
  }

  for (std::vector <lay::LineStyles::iterator>::const_iterator i = iters.begin (); i != iters.end (); ++i) {
    if ((*i)->order_index () > 0) {
      std::string name ((*i)->name ());
      if (name.empty ()) {
        name = geo::sprintf ("custom #%d", (*i)->order_index ());
      }
      new QListWidgetItem (QIcon ((*i)->get_bitmap (36, 26)), geo::to_qstring (name), mp_ui->style_items);
    }
  }
}

void 
SelectLineStyleForm::sel_changed (QListWidgetItem *citem, QListWidgetItem *)
{
  int row = mp_ui->style_items->row (citem);
  if (m_include_nil) {
    --row;
  }

  if (row >= int (std::distance (m_styles.begin (), m_styles.begin_custom ()))) {
    for (lay::LineStyles::iterator i = m_styles.begin_custom (); i != m_styles.end (); ++i) {
      if (int (i->order_index ()) - 1 + std::distance (m_styles.begin (), m_styles.begin_custom ()) == row) {
        m_selected = std::distance (m_styles.begin (), i);
        return;
      }
    }
  } else if (row >= 0) {
    m_selected = row;
  } else {
    m_selected = -1;
  }
}

}

