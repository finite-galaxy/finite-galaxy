// Radar.h

#ifndef RADAR_H_
#define RADAR_H_

#include "Colour.h"
#include "Point.h"

#include <vector>



// Class representing all objects which should show up on radar. Each object is
// either a ring (planets) or a dot (ships) and has a colour representing its status
// (owned by player, friendly, unfriendly, hostile, inactive, or special).
class Radar {
public:
  static const int PLAYER;
  static const int FRIENDLY;
  static const int UNFRIENDLY;
  static const int HOSTILE;
  static const int INACTIVE;
  static const int SPECIAL;
  static const int ANOMALOUS;
  static const int BLINK;
  static const int VIEWPORT;


public:
  void Clear();
  void SetCentre(const Point &centre);

  // Add an object. If "inner" is 0 it is a dot; otherwise, it is a ring. The
  // given position should be in world units (not shrunk to radar units).
  void Add(int type, Point position, double outer, double inner = 0.);
  // Add a pointer, pointing in the direction of the given vector.
  void AddPointer(int type, const Point &position);
  // Add a viewport vertex indicating the extent of what can be seen on screen.
  void AddViewportBoundary(const Point &vertex);

  // Draw the radar display at the given coordinates.
  void Draw(const Point &centre, double scale, double radius, double pointerRadius) const;

  // Get the colour for the given status.
  static const Colour &GetColour(int type);


private:
  class Object {
  public:
    Object(const Colour &colour, const Point &pos, double out, double in);

    Colour colour;
    Point position;
    double outer;
    double inner;
  };

  class Pointer {
  public:
    Pointer(const Colour &colour, const Point &unit);

    Colour colour;
    Point unit;
  };

  class Line {
  public:
    Line(const Colour &colour, const Point &base, const Point &vector);

    Colour colour;
    Point base;
    Point vector;
  };

private:
  Point centre;
  std::vector<Object> objects;
  std::vector<Pointer> pointers;
  std::vector<Line> lines;
};



#endif
