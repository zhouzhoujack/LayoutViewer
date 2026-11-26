#include "layCursor.h"

#include <QCursor>

namespace lay
{

// -----------------------------------------------------------------------------
//  Cursor implementation

QCursor 
Cursor::qcursor (cursor_shape cursor)
{
  switch (int (cursor)) {
  case 0:
    return QCursor (Qt::ArrowCursor);
  case 1:
    return QCursor (Qt::UpArrowCursor);
  case 2:
    return QCursor (Qt::CrossCursor);
  case 3:
    return QCursor (Qt::WaitCursor);
  case 4:
    return QCursor (Qt::IBeamCursor);
  case 5:
    return QCursor (Qt::SizeVerCursor);
  case 6:
    return QCursor (Qt::SizeHorCursor);
  case 7:
    return QCursor (Qt::SizeBDiagCursor);
  case 8:
    return QCursor (Qt::SizeFDiagCursor);
  case 9:
    return QCursor (Qt::SizeAllCursor);
  case 10:
    return QCursor (Qt::BlankCursor);
  case 11:
    return QCursor (Qt::SplitVCursor);
  case 12:
    return QCursor (Qt::SplitHCursor);
  case 13:
    return QCursor (Qt::PointingHandCursor);
  case 14:
    return QCursor (Qt::ForbiddenCursor);
  case 15:
    return QCursor (Qt::WhatsThisCursor);
  case 16:
    return QCursor (Qt::BusyCursor);
  case 17:
    return QCursor (Qt::OpenHandCursor);
  case 18:
    return QCursor (Qt::ClosedHandCursor);
  default:
    return QCursor (Qt::BlankCursor);
  };
}

}

