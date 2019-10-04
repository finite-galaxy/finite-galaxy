// Table.cpp

#include "Table.h"

#include "FillShader.h"
#include "Font.h"
#include "FontSet.h"
#include "Format.h"
#include "Rectangle.h"

using namespace std;



Table::Table()
{
  Clear();
}



// Set the column positions. If no columns are set, the Table will draw a
// list (one column of text, left aligned).
void Table::Clear()
{
  columns.clear();

  font = &FontSet::Get(18);
  rowSize = Point(0., 20.);
  centre = Point(0., font->Height() / 2);
  lineSize = Point(0., 1.);
  lineOff = Point(0., font->Height() + 1);

  point = Point();
  it = columns.begin();
  colour = Colour(1.f, 0.f);
}



void Table::AddColumn(int x, Align align)
{
  columns.emplace_back(x, align == LEFT ? 0. : align == RIGHT ? -1. : -.5);

  // This may invalidate iterators, so:
  it = columns.begin();
}



// Set the font size. Default is 14 pixels.
void Table::SetFontSize(int size)
{
  font = &FontSet::Get(size);
  lineOff.Y() = font->Height() + 1;
  centre.Y() = font->Height() / 2;
}



// Set the row height. Default is 20 pixels.
void Table::SetRowHeight(int height)
{
  rowSize.Y() = height;
}



// Set the width of the highlight area. If the underline has not been set,
// this will also set the width of the underline.
void Table::SetHighlight(int startX, int endX)
{
  rowSize.X() = endX - startX;
  centre.X() = (endX + startX) / 2;

  if(!lineSize.X())
  {
    lineSize.X() = rowSize.X();
    lineOff.X() = centre.X();
  }
}



// Set the X range of the underline. If the highlight has not been set, this
// will also set the width of the highlight.
void Table::SetUnderline(int startX, int endX)
{
  lineSize.X() = endX - startX;
  lineOff.X() = (endX + startX) / 2;

  if(!rowSize.X())
  {
    rowSize.X() = lineSize.X();
    centre.X() = lineOff.X();
  }
}



// Begin drawing at the given position. Each time text is drawn, it fills a
// new column until all columns have been filled. Then, the Y position is
// increased based on the row height, and a new row begins.
void Table::DrawAt(const Point &point) const
{
  this->point = point + Point(0., (rowSize.Y() - font->Height()) / 2);
  it = columns.begin();
}



// Set the colour for drawing text and underlines.
void Table::SetColour(const Colour &colour) const
{
  this->colour = colour;
}



// Advance to the next field without drawing anything.
void Table::Advance(int fields) const
{
  while(fields-- > 0)
  {
    if(columns.empty() || ++it == columns.end())
    {
      it = columns.begin();
      point.Y() += rowSize.Y();
    }
  }
}



// Draw a single text field, and move on to the next one.
void Table::Draw(const string &text, const Font::Layout *layout) const
{
  Draw(text, colour, layout);
}



// If a colour is given, this field is drawn using that colour, 
// but the previously set colour will be used for future fields.
void Table::Draw(const string &text, const Colour &colour, const Font::Layout *layout) const
{
  if(font)
  {
    Point pos = point;
    if(it != columns.end())
      pos += Point(it->offset + it->align * font->Width(text, layout), 0.);
    font->Draw(text, pos, colour, layout);
  }

  Advance();
}



void Table::Draw(double value) const
{
  Draw(value, colour);
}



void Table::Draw(double value, const Colour &colour) const
{
  Draw(Format::Number(value), colour);
}



// Draw an underline under the text for the current row.
void Table::DrawUnderline() const
{
  DrawUnderline(colour);
}



void Table::DrawUnderline(const Colour &colour) const
{
  FillShader::Fill(point + lineOff - Point(0., 2.), lineSize, colour);
}



// Highlight the current row.
void Table::DrawHighlight() const
{
  DrawHighlight(colour);
}



void Table::DrawHighlight(const Colour &colour) const
{
  FillShader::Fill(GetCentrePoint(), GetRowSize(), colour);
}



// Shift the draw position down by the given amount. This usually should not
// be called in the middle of a row, or the fields will not line up.
void Table::DrawGap(int y) const
{
  point.Y() += y;
}



// Get the point that should be passed to DrawAt() to start the next row at
// the given location.
Point Table::GetPoint()
{
  return point - Point(0., (rowSize.Y() - font->Height()) / 2);
}



// Get the centre and size of the current row. This can be used to define
// what screen region constitutes a mouse click on this particular row.
Point Table::GetCentrePoint() const
{
  return point + centre;
}



Point Table::GetRowSize() const
{
  return rowSize;
}



Rectangle Table::GetRowBounds() const
{
  return Rectangle(GetCentrePoint(), GetRowSize());
}



Table::Column::Column()
  : offset(0.), align(0.)
{
}



Table::Column::Column(double offset, double align)
  : offset(offset), align(align)
{
}
