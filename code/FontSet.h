// FontSet.h

#ifndef FONT_SET_H_
#define FONT_SET_H_

#include <string>

class Font;



// Class for getting the Font object for a given point size.
class FontSet {
public:
  static void Add(const std::string &fontsDir);
  static const Font &Get(int size);

  // Set the font description. "desc" is a comma separated list of font families.
  static void SetFontDescription(const std::string &desc);
  // Set the language for layouting.
  static void SetLanguage(const std::string &lang);
};



#endif

