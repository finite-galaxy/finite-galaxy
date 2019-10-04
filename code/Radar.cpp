// Radar.cpp

#include "Radar.h"

#include "GameData.h"
#include "LineShader.h"
#include "PointerShader.h"
#include "RingShader.h"

#include <cmath>

using namespace std;

const int Radar::PLAYER = 0;
const int Radar::FRIENDLY = 1;
const int Radar::UNFRIENDLY = 2;
const int Radar::HOSTILE = 3;
const int Radar::INACTIVE = 4;
const int Radar::SPECIAL = 5;
const int Radar::ANOMALOUS = 6;
const int Radar::BLINK = 7;
const int Radar::VIEWPORT = 8;



void Radar::Clear()
{
  objects.clear();
  pointers.clear();
  lines.clear();
}



void Radar::SetCentre(const Point &centre)
{
  this->centre = centre;
}



// Add an object. If "inner" is 0 it is a dot; otherwise, it is a ring. The
// given position should be in world units (not shrunk to radar units).
void Radar::Add(int type, Point position, double outer, double inner)
{
  objects.emplace_back(GetColour(type).Opaque(), position - centre, outer, inner);
}



// Add a pointer, pointing in the direction of the given vector.
void Radar::AddPointer(int type, const Point &position)
{
  pointers.emplace_back(GetColour(type), position.Unit());
}



// Create a "corner" from a vertical and horizontal leg.
void Radar::AddViewportBoundary(const Point &vertex)
{
  Point start(vertex.X() - copysign(200., vertex.X()), vertex.Y());
  Point end(vertex.X(), vertex.Y() - copysign(200., vertex.Y()));

  // Add the horizontal leg, pointing from start to vertex.
  lines.emplace_back(GetColour(VIEWPORT), start, vertex - start);
  // Add the vertical leg, pointing from end to vertex.
  lines.emplace_back(GetColour(VIEWPORT), end, vertex - end);
}



// Draw the radar display at the given coordinates.
void Radar::Draw(const Point &centre, double scale, double radius, double pointerRadius) const
{
  // Draw any desired line vectors.
  for(const Line &line : lines)
  {
    Point start = line.base * scale;
    Point v = line.vector * scale;

    // At least one endpoint must be within the radar display.
    double startExcess = start.Length() - radius;
    double endExcess = (start + v).Length() - radius;
    if(startExcess > 0 && endExcess > 0)
      continue;
    else if(startExcess > 0)
    {
      // Move "start" along "v" until it is within the radius.
      start += startExcess * v.Unit();
      // Shorten "v" to keep the desired length.
      v -= startExcess * v.Unit();
    }
    else if(endExcess > 0)
      v -= endExcess * v.Unit();

    LineShader::Draw(start + centre, start + v + centre, 1.f, line.colour);
  }

  // Draw StellarObjects and ships.
  RingShader::Bind();
  for(const Object &object : objects)
  {
    Point position = object.position * scale;
    double length = position.Length();
    if(length > radius)
      position *= radius / length;
    position += centre;

    RingShader::Add(position, object.outer, object.inner, object.colour);
  }
  RingShader::Unbind();

  // Draw neighbouring system indicators.
  PointerShader::Bind();
  for(const Pointer &pointer : pointers)
    PointerShader::Add(centre, pointer.unit, 10.f, 10.f, pointerRadius, pointer.colour);
  PointerShader::Unbind();
}



const Colour &Radar::GetColour(int type)
{
  static const vector<Colour> colour = {
    *GameData::Colours().Get("radar player"),
    *GameData::Colours().Get("radar friendly"),
    *GameData::Colours().Get("radar unfriendly"),
    *GameData::Colours().Get("radar hostile"),
    *GameData::Colours().Get("radar inactive"),
    *GameData::Colours().Get("radar special"),
    *GameData::Colours().Get("radar anomalous"),
    *GameData::Colours().Get("radar blink"),
    *GameData::Colours().Get("radar viewport")
  };

  if(static_cast<size_t>(type) >= colour.size())
    type = INACTIVE;

  return colour[type];
}



Radar::Object::Object(const Colour &colour, const Point &pos, double out, double in)
  : colour(colour), position(pos), outer(out), inner(in)
{
}



Radar::Pointer::Pointer(const Colour &colour, const Point &unit)
  : colour(colour), unit(unit)
{
}



// Create a line starting from "base" with length and angle described by "vector."
Radar::Line::Line(const Colour &colour, const Point &base, const Point &vector)
  : colour(colour), base(base), vector(vector)
{
}
