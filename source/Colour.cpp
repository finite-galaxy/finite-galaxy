// Colour.cpp

#include "Colour.h"



// Greyscale colour constructor.
Colour::Colour(float i, float a)
  : colour{i, i, i, a}
{
}



// Full colour constructor.
Colour::Colour(float r, float g, float b, float a)
  : colour{r, g, b, a}
{
}



// Set all four colour components to the given values.
void Colour::Load(double r, double g, double b, double a)
{
  colour[0] = static_cast<float>(r);
  colour[1] = static_cast<float>(g);
  colour[2] = static_cast<float>(b);
  colour[3] = static_cast<float>(a);
}



// Get a float vector representing this colour, for use by OpenGL.
const float *Colour::Get() const
{
  return colour;
}



// Get an opaque version of this colour.
Colour Colour::Opaque() const
{
  Colour opaque = *this;
  opaque.colour[3] = 1.f;
  return opaque;
}



// Assuming this colour is opaque, get a transparent version of it.
Colour Colour::Transparent(float alpha) const
{
  Colour result;
  for(int i = 0; i < 3; ++i)
    result.colour[i] = colour[i] * alpha;
  result.colour[3] = alpha;

  return result;
}



// Assuming this colour is opaque, get an additive version of it.
Colour Colour::Additive(float alpha) const
{
  Colour result = Transparent(alpha);
  result.colour[3] = 0.f;

  return result;
}
