// DrawList.cpp

#include "DrawList.h"

#include "Body.h"
#include "Preferences.h"
#include "Screen.h"
#include "Sprite.h"
#include "SpriteSet.h"
#include "SpriteShader.h"

#include <cmath>

using namespace std;



// Clear the list.
void DrawList::Clear(int step, double zoom)
{
  items.clear();
  this->step = step;
  this->zoom = zoom;
  isHighDPI = (Screen::IsHighResolution() ? zoom > .5 : zoom > 1.);
}



void DrawList::SetCentre(const Point &centre, const Point &centreVelocity)
{
  this->centre = centre;
  this->centreVelocity = centreVelocity;
}



// Add an object based on the Body class.
bool DrawList::Add(const Body &body, double cloak)
{
  Point position = body.Position() - centre;
  Point blur = body.Velocity() - centreVelocity;
  if(Cull(body, position, blur) || cloak >= 1.)
    return false;

  Push(body, position, blur, cloak, 1., body.GetSwizzle());
  return true;
}



bool DrawList::Add(const Body &body, Point position)
{
  position -= centre;
  Point blur = body.Velocity() - centreVelocity;
  if(Cull(body, position, blur))
    return false;

  Push(body, position, blur, 0., 1., body.GetSwizzle());
  return true;
}



bool DrawList::AddUnblurred(const Body &body)
{
  Point position = body.Position() - centre;
  Point blur;
  if(Cull(body, position, blur))
    return false;

  Push(body, position, blur, 0., 1., body.GetSwizzle());
  return true;
}



bool DrawList::AddProjectile(const Body &body, const Point &adjustedVelocity, double clip)
{
  Point position = body.Position() + .5 * body.Velocity() - centre;
  Point blur = adjustedVelocity - centreVelocity;
  if(Cull(body, position, blur) || clip <= 0.)
    return false;

  Push(body, position, blur, 0., clip, body.GetSwizzle());
  return true;
}



bool DrawList::AddSwizzled(const Body &body, int swizzle)
{
  Point position = body.Position() - centre;
  Point blur = body.Velocity() - centreVelocity;
  if(Cull(body, position, blur))
    return false;

  Push(body, position, blur, 0., 1., swizzle);
  return true;
}



// Draw all the items in this list.
void DrawList::Draw() const
{
  SpriteShader::Bind();

  bool withBlur = Preferences::Has("Render motion blur");
  for(const SpriteShader::Item &item : items)
    SpriteShader::Add(item, withBlur);

  SpriteShader::Unbind();
}



bool DrawList::Cull(const Body &body, const Point &position, const Point &blur) const
{
  if(!body.HasSprite() || !body.Zoom())
    return true;

  Point unit = body.Facing().Unit();
  // Cull sprites that are completely off screen, to reduce the number of draw
  // calls that we issue (which may be the bottleneck on some systems).
  Point size(
    .5 * (fabs(unit.X() * body.Height()) + fabs(unit.Y() * body.Width()) + fabs(blur.X())),
    .5 * (fabs(unit.X() * body.Width()) + fabs(unit.Y() * body.Height()) + fabs(blur.Y())));
  Point topLeft = (position - size) * zoom;
  Point bottomRight = (position + size) * zoom;
  if(bottomRight.X() < Screen::Left() || bottomRight.Y() < Screen::Top())
    return true;
  if(topLeft.X() > Screen::Right() || topLeft.Y() > Screen::Bottom())
    return true;

  return false;
}



void DrawList::Push(const Body &body, Point pos, Point blur, double cloak, double clip, int swizzle)
{
  SpriteShader::Item item;

  item.texture = body.GetSprite()->Texture(isHighDPI);
  item.frame = body.GetFrame(step);
  item.frameCount = body.GetSprite()->Frames();

  // Get unit vectors in the direction of the object's width and height.
  double width = body.Width();
  double height = body.Height();
  Point unit = body.Facing().Unit();
  Point uw = unit * width;
  Point uh = unit * height;

  item.clip = clip;
  if(clip < 1.)
  {
    // "clip" is the fraction of its height that we're clipping the sprite
    // to. We still want it to start at the same spot, though.
    pos -= uh * ((1. - clip) * .5);
    uh *= clip;
  }
  item.position[0] = static_cast<float>(pos.X() * zoom);
  item.position[1] = static_cast<float>(pos.Y() * zoom);

  // (0, -1) means a zero-degree rotation (since negative Y is up).
  uw *= zoom;
  uh *= zoom;
  item.transform[0] = -uw.Y();
  item.transform[1] = uw.X();
  item.transform[2] = -uh.X();
  item.transform[3] = -uh.Y();

  // Calculate the blur vector, in texture coordinates.
  blur *= zoom;
  item.blur[0] = unit.Cross(blur) / (width * 4.);
  item.blur[1] = -unit.Dot(blur) / (height * 4.);

  item.alpha = 1. - cloak;
  item.swizzle = swizzle;

  items.push_back(item);
}
