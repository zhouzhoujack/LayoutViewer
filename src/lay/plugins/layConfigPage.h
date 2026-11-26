#pragma once

#include <QFrame>

namespace lay
{
  
class Dispatcher;


class ConfigPage 
  : public QFrame
{
public:
  ConfigPage (QWidget *parent) 
    : QFrame (parent)
  {
    // .. nothing else ..
  }


  virtual void setup (Dispatcher * /*root*/)
  {
    //  the default implementation does nothing.
  }

  virtual void commit (Dispatcher * /*root*/)
  {
    //  the default implementation does nothing.
  }

};

}
