// FontSet.h

#ifndef FONT_SET_H_
#define FONT_SET_H_

#include <string>

class Font;



// Class for getting the Font object for a given point size. Each font must be
// based on a glyph image; right now only point sizes 14 and 18 exist.
class FontSet {
public:
  static void Add(const std::string &path, int size);
  static const Font &Get(int size);
};



#endif
