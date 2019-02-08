// WrappedText.h

#ifndef WRAPPED_TEXT_H_
#define WRAPPED_TEXT_H_

#include "Font.h"
#include "Point.h"

#include <string>

class Colour;



 // Class for holding wrapped parameters.
class WrappedText {
public:
  WrappedText();
  explicit WrappedText(const Font &font);
  
  // Set the alignment mode.
  Font::Align Alignment() const;
	void SetAlignment(Font::Align align);

	// Set the truncate mode.
	Font::Truncate Truncate() const;
	void SetTruncate(Font::Truncate trunc);
  
  // Set the wrap width. This does not include any margins.
  int WrapWidth() const;
  void SetWrapWidth(int width);
  
  // Set the font to use. This will also set sensible defaults for the tab
  // width, line height, and paragraph break. You must specify the wrap width
  // and the alignment separately.
  void SetFont(const Font &font);
  
  // Set the height in pixels of one line of text within a paragraph.
  int LineHeight() const;
  void SetLineHeight(int height);
  
  // Set the extra spacing in pixels to be added in between paragraphs.
  int ParagraphBreak() const;
  void SetParagraphBreak(int height);
  
  // Wrap the given text. Use Draw() to draw it.
  void Wrap(const std::string &str);
  void Wrap(const char *str);
  
  // Get the height of the wrapped text.
  int Height() const;
  
  // Draw the text.
  void Draw(const Point &topLeft, const Colour &colour) const;
  
  

private:
  const Font *font;
  
  std::string text;
  Font::Layout layout;
};



#endif

