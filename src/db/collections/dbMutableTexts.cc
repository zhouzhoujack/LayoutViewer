#include "dbMutableTexts.h"

namespace db
{

// -------------------------------------------------------------------------------------------------------------
//  MutableTexts implementation

MutableTexts::MutableTexts ()
  : AsIfFlatTexts ()
{
  //  .. nothing yet ..
}

MutableTexts::MutableTexts (const MutableTexts &other)
  : AsIfFlatTexts (other)
{
  //  .. nothing yet ..
}

MutableTexts::~MutableTexts ()
{
  //  .. nothing yet ..
}

void
MutableTexts::insert (const db::Shape &shape)
{
  if (shape.is_text ()) {
    db::Text text;
    shape.text (text);
    insert (text);
  }
}

}

