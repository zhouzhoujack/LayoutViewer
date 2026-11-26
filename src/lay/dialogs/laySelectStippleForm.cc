#include "laySelectStippleForm.h"
#include "detail/utilsString.h"
#include "detail/utilsInternational.h"
#include "ui_SelectStippleForm.h"

#include "tlString.h"

#include <QBitmap>

#include <algorithm>

namespace lay
{

// -----------------------------------------------------------------------------
//  SelectStippleForm implementation

SelectStippleForm::SelectStippleForm (QWidget *parent, const lay::DitherPattern &pattern, bool include_nil)
  : QDialog (parent), m_selected (-1), m_pattern (pattern), m_include_nil (include_nil)
{
  mp_ui = new Ui::SelectStippleForm ();

  mp_ui->setupUi (this);

  mp_ui->stipple_items->setUniformItemSizes (true);

  update ();

  connect (mp_ui->stipple_items, SIGNAL (currentItemChanged(QListWidgetItem*, QListWidgetItem*)), 
           this, SLOT (sel_changed (QListWidgetItem *, QListWidgetItem *)));
}

SelectStippleForm::~SelectStippleForm ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
SelectStippleForm::set_selected (int selected)
{
  if (selected != m_selected) {
    m_selected = selected;
    mp_ui->stipple_items->setCurrentRow (m_include_nil ? (selected < 0 ? 0 : selected + 1) : selected);
  }
}

namespace {
  struct display_order
  {
    bool operator () (lay::DitherPattern::iterator a, lay::DitherPattern::iterator b)
    {
      return a->order_index () < b->order_index ();
    }
  };
}

void 
SelectStippleForm::update ()
{
  mp_ui->stipple_items->clear ();

  if (m_include_nil) {
    new QListWidgetItem (QObject::tr ("None"), mp_ui->stipple_items);
  }

  std::vector <lay::DitherPattern::iterator> iters; 
  for (lay::DitherPattern::iterator i = m_pattern.begin_custom (); i != m_pattern.end (); ++i) {
    iters.push_back (i);
  }
  std::sort (iters.begin (), iters.end (), display_order ());

  //  fill the list of stipple items
  for (lay::DitherPattern::iterator i = m_pattern.begin (); i != m_pattern.begin_custom (); ++i) {
    std::string name (i->name ());
    if (name.empty ()) {
      name = geo::sprintf ("#%d", std::distance (m_pattern.begin (), i));
    }
    new QListWidgetItem (QIcon (i->get_bitmap ()), geo::to_qstring (name), mp_ui->stipple_items);
  }

  for (std::vector <lay::DitherPattern::iterator>::const_iterator i = iters.begin (); i != iters.end (); ++i) {
    if ((*i)->order_index () > 0) {
      std::string name ((*i)->name ());
      if (name.empty ()) {
        name = geo::sprintf ("custom #%d", (*i)->order_index ());
      }
      new QListWidgetItem (QIcon ((*i)->get_bitmap ()), geo::to_qstring (name), mp_ui->stipple_items);
    }
  }
}

void 
SelectStippleForm::sel_changed (QListWidgetItem *citem, QListWidgetItem *)
{
  int row = mp_ui->stipple_items->row (citem);
  if (m_include_nil) {
    --row;
  }

  if (row >= int (std::distance (m_pattern.begin (), m_pattern.begin_custom ()))) {
    for (lay::DitherPattern::iterator i = m_pattern.begin_custom (); i != m_pattern.end (); ++i) {
      if (int (i->order_index ()) - 1 + std::distance (m_pattern.begin (), m_pattern.begin_custom ()) == row) {
        m_selected = std::distance (m_pattern.begin (), i);
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

