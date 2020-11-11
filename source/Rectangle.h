// Rectangle.h

#ifndef RECTANGLE_H_
#define RECTANGLE_H_

#include "Point.h"



// Class representing a rectangle.
class Rectangle {
public:
  // Construct a rectangle by specifying the two corners rather than the
  // centre and the dimensions. The two corners need not be in any order.
  static Rectangle WithCorners(const Point &from, const Point &to);
  // Construct a rectangle beginning at the given point and having the given
  // dimensions (which are allowed to be negative).
  static Rectangle FromCorner(const Point &corner, const Point &dimensions);

  // Default constructor.
  Rectangle() = default;
  // Constructor, specifying the centre and the dimensions.
  Rectangle(const Point &centre, const Point &dimensions);
  // Copy constructor.
  Rectangle(const Rectangle &other) = default;

  // Assignment operator.
  Rectangle &operator=(const Rectangle &other) = default;

  // Translation operators.
  Rectangle operator+(const Point &offset) const;
  Rectangle &operator+=(const Point &offset);
  Rectangle operator-(const Point &offset) const;
  Rectangle &operator-=(const Point &offset);

  // Query the attributes of the rectangle.
  Point Centre() const;
  Point Dimensions() const;
  double Width() const;
  double Height() const;
  double Left() const;
  double Top() const;
  double Right() const;
  double Bottom() const;
  Point TopLeft() const;
  Point BottomRight() const;
  Point BottomLeft() const;

  // Check if a point is inside this rectangle.
  bool Contains(const Point &point) const;
  // Check if the given rectangle is inside this one. If one of its edges is
  // touching the edge of this one, that still counts.
  bool Contains(const Rectangle &other) const;


private:
  Point centre;
  Point dimensions;
};



#endif
