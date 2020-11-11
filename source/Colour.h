// Colour.h

#ifndef COLOUR_H_
#define COLOUR_H_



// Class representing an RGBA colour (for use by OpenGL). The specified colours
// should be in premultiplied alpha mode. For example, a white colour which is
// 50% translucent would be {.5, .5, .5} with an alpha of .5. This allows you to
// also specify colours that use additive blending. For example, if the alpha is
// zero the colour's components will be added to whatever is underneath them.
class Colour {
public:
  // Constructor for shades of grey, opaque unless an alpha is also given.
  explicit Colour(float i = 1.f, float a = 1.f);
  // Constructor for colours, opaque unless an alpha is also given.
  Colour(float r, float g, float b, float a = 1.f);

  // Set this colour to the given RGBA values.
  void Load(double r, double g, double b, double a);
  // Get the colour as a float vector, suitable for use by OpenGL.
  const float *Get() const;

  // Get this colour, but entirely opaque. That is, this is the colour you would
  // get if drawing this colour on top of opaque black.
  Colour Opaque() const;
  // Assuming that this colour is completely opaque, get the same colour with
  // the given transparency.
  Colour Transparent(float alpha) const;
  // Assuming that this is an opaque colour, get its additive equivalent with
  // the given fraction of its full brightness.
  Colour Additive(float alpha) const;


private:
  // Store the colour as a float vector for easy interfacing with OpenGL.
  float colour[4];
};



#endif
