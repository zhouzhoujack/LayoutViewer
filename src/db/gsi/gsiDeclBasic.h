#ifndef _HDR_gsiDeclBasic
#define _HDR_gsiDeclBasic

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "tlVariant.h"

namespace gsi
{

/**
 *  @brief Provides a basic implementation for a "boxed" plain value using a Variant as the basic type
 */
class GSI_PUBLIC Value
{
public:
  /**
   *  @brief Constructor: create a "nil" object
   */
  Value () { }

  /**
   *  @brief Constructor: create an object with a value
   */
  Value (const tl::Variant &v) : m_v (v) { }

  /**
   *  @brief Obtain the value 
   */
  const tl::Variant &value () const 
  { 
    return m_v;
  }

  /**
   *  @brief Obtain the value (non-const)
   */
  tl::Variant &value () 
  { 
    return m_v;
  }

  /**
   *  @brief Set the value 
   */
  void set_value (const tl::Variant &v)
  { 
    m_v = v;
  }

  /**
   *   @brief Converts the value to a string
   */
  std::string to_string () const 
  { 
    return m_v.to_string ();
  }

protected:
  tl::Variant m_v;
};

}

#endif


