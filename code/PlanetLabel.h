// PlanetLabel.h

#ifndef PLANET_LABEL_H_
#define PLANET_LABEL_H_

#include "Colour.h"
#include "Point.h"

#include <string>

class StellarObject;
class System;



class PlanetLabel {
public:
  PlanetLabel(const Point &position, const StellarObject &object, const System *system, double zoom);

  void Draw() const;


private:
  Point position;
  double radius = 0.;
  std::string name;
  std::string government;
  Colour colour;
  int hostility = 0;
  int direction = 0;
};



#endif
