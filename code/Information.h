// Information.h

#ifndef INFORMATION_H_
#define INFORMATION_H_

#include "Color.h"
#include "Point.h"

#include <map>
#include <set>
#include <string>

class Sprite;



// Class representing information to be displayed in a user interface, independent
// of how that information is laid out or shown.
class Information {
public:
  void SetSprite(const std::string &name, const Sprite *sprite, const Point &unit = Point(0., -1.), float frame = 0.f);
  const Sprite *GetSprite(const std::string &name) const;
  const Point &GetSpriteUnit(const std::string &name) const;
  float GetSpriteFrame(const std::string &name) const;
  
  void SetString(const std::string &name, const std::string &value);
  const std::string &GetString(const std::string &name) const;
  
  void SetBar(const std::string &name, double value, double segments = 0.);
  double BarValue(const std::string &name) const;
  double BarSegments(const std::string &name) const;
  
  void SetCondition(const std::string &condition);
  bool HasCondition(const std::string &condition) const;
  
  void SetOutlineColor(const Color &color);
  const Color &GetOutlineColor() const;
  
  
private:
  std::map<std::string, const Sprite *> sprites;
  std::map<std::string, Point> spriteUnits;
  std::map<std::string, float> spriteFrames;
  std::map<std::string, std::string> strings;
  std::map<std::string, double> bars;
  std::map<std::string, double> barSegments;
  
  std::set<std::string> conditions;
  
  Color outlineColor;
};



#endif
