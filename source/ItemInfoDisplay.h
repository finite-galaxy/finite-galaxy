// ItemInfoDisplay.h

#ifndef ITEM_INFO_DISPLAY_H_
#define ITEM_INFO_DISPLAY_H_

#include "Point.h"
#include "WrappedText.h"

#include <string>
#include <vector>

class Table;



// Class representing three panels of information about a given item. One shows
// a text description, one shows the item's attributes, and a third may be
// different depending on what kind of item it is (a ship or an outfit).
class ItemInfoDisplay {
public:
  ItemInfoDisplay();

  // Get the panel width.
  static int PanelWidth();
  // Get the height of each of the panels.
  int MaximumHeight() const;
  int DescriptionHeight() const;
  int AttributesHeight() const;

  // Draw each of the panels.
  void DrawDescription(const Point &topLeft) const;
  virtual void DrawAttributes(const Point &topLeft) const;
  void DrawTooltips() const;

  // Update the location where the mouse is hovering.
  void Hover(const Point &point);
  void ClearHover();


protected:
  void UpdateDescription(const std::string &text, const std::vector<std::string> &licences, bool isShip);
  Point Draw(Point point, const std::vector<std::string> &labels, const std::vector<std::string> &values) const;
  void CheckHover(const Table &table, const std::string &label) const;


protected:
  static const int WIDTH = 250;

  WrappedText description;
  int descriptionHeight = 0;

  std::vector<std::string> attributeLabels;
  std::vector<std::string> attributeValues;
  int attributesHeight = 0;

  int maximumHeight = 0;

  // For tooltips:
  Point hoverPoint;
  mutable std::string hover;
  mutable int hoverCount = 0;
  bool hasHover = false;
  mutable WrappedText hoverText;
};



#endif

