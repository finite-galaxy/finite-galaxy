// MapPanel.h

#ifndef MAP_PANEL_H_
#define MAP_PANEL_H_

#include "Panel.h"

#include "Colour.h"
#include "DistanceMap.h"
#include "Point.h"
#include "WrappedText.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

class Angle;
class Government;
class Mission;
class Planet;
class PlayerInfo;
class System;



// This class provides the base class for both the "map details" panel and the
// missions panel, and handles drawing of the underlying starmap and colouring
// the systems based on a selected criterion. It also handles finding and
// drawing routes in between systems.
class MapPanel : public Panel {
public:
  // Enumeration for how the systems should be coloured:
  static const int SHOW_SHIPYARD = -1;
  static const int SHOW_OUTFITTER = -2;
  static const int SHOW_VISITED = -3;
  static const int SHOW_SPECIAL = -4;
  static const int SHOW_GOVERNMENT = -5;
  static const int SHOW_REPUTATION = -6;

  static const float OUTER;
  static const float INNER;
  static const float LINK_WIDTH;
  static const float LINK_OFFSET;


public:
  explicit MapPanel(PlayerInfo &player, int commodity = SHOW_REPUTATION, const System *special = nullptr);

  virtual void Step() override;
  virtual void Draw() override;

  void DrawButtons(const std::string &condition);
  static void DrawMiniMap(const PlayerInfo &player, float alpha, const System *const jump[2], int step);


protected:
  // Only override the ones you need; the default action is to return false.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;
  virtual bool Hover(int x, int y) override;
  virtual bool Drag(double dx, double dy) override;
  virtual bool Scroll(double dx, double dy) override;

  // Get the colour mapping for various system attributes.
  static Colour MapColour(double value);
  static Colour ReputationColour(double reputation, bool canLand, bool hasDominated);
  static Colour GovernmentColour(const Government *government);
  static Colour UninhabitedColour();
  static Colour UnexploredColour();

  virtual double SystemValue(const System *system) const;

  void Select(const System *system);
  void Find(const std::string &name);

  double Zoom() const;

  // Check whether the NPC and waypoint conditions of the given mission have
  // been satisfied.
  bool IsSatisfied(const Mission &mission) const;
  static bool IsSatisfied(const PlayerInfo &player, const Mission &mission);

  // Function for the "find" dialogues:
  static int Search(const std::string &str, const std::string &sub);


protected:
  PlayerInfo &player;

  DistanceMap distance;

  const System *playerSystem;
  const System *selectedSystem;
  const Planet *selectedPlanet = nullptr;
  // A system associated with a dialogue or conversation.
  const System *specialSystem;

  Point centre;
  Point recentreVector;
  int recentring = 0;
  int commodity;
  int step = 0;
  std::string buttonCondition;

  // Distance from the screen centre to the nearest owned system,
  // for use in determining which governments are in the legend.
  std::map<const Government *, double> closeGovernments;
  // Systems in which your escorts are located.
  std::map<const System *, std::pair<int, int>> escortSystems;
  // Centre the view on the given system (may actually be slightly offset
  // to account for panels on the screen).
  void CentreOnSystem(const System *system, bool immediate = false);

  // Cache the map layout, so it doesn't have to be re-calculated every frame.
  // The cache must be updated when the colouring mode changes.
  void UpdateCache();

  // For tooltips:
  int hoverCount = 0;
  const System *hoverSystem = nullptr;
  std::string tooltip;
  WrappedText hoverText;


private:
  void DrawTravelPlan();
  // Indicate which other systems have player escorts.
  void DrawEscorts();
  void DrawWormholes();
  void DrawLinks();
  // Draw systems in accordance to the set commodity colour scheme.
  void DrawSystems();
  void DrawNames();
  void DrawMissions();
  void DrawTooltips();
  void DrawPointer(const System *system, Angle &angle, const Colour &colour, bool bigger = false);
  static void DrawPointer(Point position, Angle &angle, const Colour &colour, bool drawBack = true, bool bigger = false);


private:
  // This is the colouring mode currently used in the cache.
  int cachedCommodity = -10;

  class Node {
  public:
    Node(const Point &position, const Colour &colour, const std::string &name, const Colour &nameColour, const Government *government)
      : position(position), colour(colour), name(name), nameColour(nameColour), government(government) {}

    Point position;
    Colour colour;
    std::string name;
    Colour nameColour;
    const Government *government;
  };
  std::vector<Node> nodes;

  class Link {
  public:
    Link(const Point &start, const Point &end, const Colour &colour)
      : start(start), end(end), colour(colour) {}

    Point start;
    Point end;
    Colour colour;
  };
  std::vector<Link> links;
};



#endif
