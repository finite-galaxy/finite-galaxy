// Rectangle.cpp

#include "Rectangle.h"



// Construct a rectangle by specifying the two corners rather than the
// centre and the dimensions. The two corners need not be in any order.
Rectangle Rectangle::WithCorners(const Point &from, const Point &to)
{
  // Regardless of which corner is which, the centre is always their average
  // and the dimensions are the difference between them (which will be
  // converted to an absolute value in the Rectangle constructor).
  return Rectangle(.5 * (from + to), from - to);
}



// Construct a rectangle beginning at the given point and having the given
// dimensions (which are allowed to be negative).
Rectangle Rectangle::FromCorner(const Point &corner, const Point &dimensions)
{
  // The centre is always the corner plus half the dimensions, regardless of
  // which corner is given and what sign the dimensions have.
  return Rectangle(corner + .5 * dimensions, dimensions);
}



// Constructor, specifying the centre and the dimensions. Internally, make sure
// that the dimensions are always positive values.
Rectangle::Rectangle(const Point &centre, const Point &dimensions)
  : centre(centre), dimensions(abs(dimensions))
{
}



// Shift this rectangle by the given offset.
Rectangle Rectangle::operator+(const Point &offset) const
{
  return Rectangle(centre + offset, dimensions);
}



// Shift this rectangle by the given offset.
Rectangle &Rectangle::operator+=(const Point &offset)
{
  centre += offset;
  return *this;
}



// Shift this rectangle by the given offset.
Rectangle Rectangle::operator-(const Point &offset) const
{
  return Rectangle(centre - offset, dimensions);
}



// Shift this rectangle by the given offset.
Rectangle &Rectangle::operator-=(const Point &offset)
{
  centre -= offset;
  return *this;
}



// Get the centre of this rectangle.
Point Rectangle::Centre() const
{
  return centre;
}



// Get the dimensions, i.e. the full width and height.
Point Rectangle::Dimensions() const
{
  return dimensions;
}



// Get the width of the rectangle.
double Rectangle::Width() const
{
  return dimensions.X();
}



// Get the height of the rectangle.
double Rectangle::Height() const
{
  return dimensions.Y();
}



// Get the minimum X value.
double Rectangle::Left() const
{
  return centre.X() - .5 * dimensions.X();
}



// Get the minimum Y value.
double Rectangle::Top() const
{
  return centre.Y() - .5 * dimensions.Y();
}



// Get the maximum X value.
double Rectangle::Right() const
{
  return centre.X() + .5 * dimensions.X();
}



// Get the maximum Y value.
double Rectangle::Bottom() const
{
  return centre.Y() + .5 * dimensions.Y();
}



// Get the top left corner - that is, the minimum x and y.
Point Rectangle::TopLeft() const
{
  return centre - .5 * dimensions;
}



// Get the bottom right corner - that is, the maximum x and y.
Point Rectangle::BottomRight() const
{
  return centre + .5 * dimensions;
}



// Get the bottom right corner - that is, the minimum x and maximum y.
Point Rectangle::BottomLeft() const
{
  return centre + Point(.5 * dimensions.X(), .5 * dimensions.Y());
}



// Check if a point is inside this rectangle.
bool Rectangle::Contains(const Point &point) const
{
  // The point is within the rectangle if its distance to the centre is less
  // than half the dimensions.
  Point d = 2. * abs(point - centre);
  return (d.X() <= dimensions.X() && d.Y() <= dimensions.Y());
}



// Check if the given rectangle is inside this one. If one of its edges is
// touching the edge of this one, that still counts.
bool Rectangle::Contains(const Rectangle &other) const
{
  return Contains(other.TopLeft()) && Contains(other.BottomRight());
}
