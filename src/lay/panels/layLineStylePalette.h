#ifndef HDR_layLineStylePalette
#define HDR_layLineStylePalette


#include <vector>
#include <string>

namespace lay
{

class LineStylePalette
{
public:

  /**
   *  @brief Default constructor
   *
   *  This initializes the palette with the standard styles.
   */
  LineStylePalette ();

  /**
   *  @brief Constructor from the data 
   *
   *  @param styles The styles as a vector
   */
  LineStylePalette (const std::vector<unsigned int> &styles);

  /**
   *  @brief Copy constructor
   */
  LineStylePalette (const LineStylePalette &d);

  /**
   *  @brief Assignment operator
   */
  LineStylePalette operator= (const LineStylePalette &d);

  /**
   *  @brief Equality operator
   */
  bool operator== (const LineStylePalette &d) const;

  /**
   *  @brief Inequality operator
   */
  bool operator!= (const LineStylePalette &d) const
  {
    return ! operator== (d);
  }

  /**
   *  @brief Change a specific style
   */
  void set_style (unsigned int n, unsigned int s);

  /**
   *  @brief Clear the style list
   */
  void clear_styles ();

  /** 
   *  @brief Retrieve the style by index
   */
  unsigned int style_by_index (unsigned int n) const;

  /**
   *  @brief Retrieve the number of styles in the palette
   *
   *  Warning: it is not guaranteed that this number is non-zero.
   */
  unsigned int styles () const;

  /** 
   *  @brief Conversion to a string 
   */
  std::string to_string () const;

  /**
   *  @brief Conversion from a string
   *
   *  This method will throw an exception if the string does not have a valid format
   *  like the one returned by the to_string method.
   */
  void from_string (const std::string &s);

  /**
   *  @brief Deliver the default palette
   */
  static LineStylePalette default_palette (); 

private:
  std::vector <unsigned int> m_styles;

};

}

#endif

