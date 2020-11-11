// ShipInfoDisplay.h

#ifndef SHIP_INFO_DISPLAY_H_
#define SHIP_INFO_DISPLAY_H_

#include "ItemInfoDisplay.h"

#include <string>
#include <vector>

class Depreciation;
class Point;
class Ship;



// Class representing three panels of information about a given ship. One shows the
// ship's description, the second summarizes its attributes, and the third lists
// all outfits currently installed in the ship. This is used for the shipyard, for
// showing changes to your ship as you add upgrades, for scanning other ships, etc.
class ShipInfoDisplay : public ItemInfoDisplay {
public:
  ShipInfoDisplay() = default;
  ShipInfoDisplay(const Ship &ship, const Depreciation &depreciation, int day);

  // Call this every time the ship changes.
  void Update(const Ship &ship, const Depreciation &depreciation, int day);

  // Provided by ItemInfoDisplay:
  // int PanelWidth();
  // int MaximumHeight() const;
  // int DescriptionHeight() const;
  // int AttributesHeight() const;
  int OutfitsHeight() const;
  int SaleHeight() const;

  // Provided by ItemInfoDisplay:
  // void DrawDescription(const Point &topLeft) const;
  virtual void DrawAttributes(const Point &topLeft) const override;
  void DrawOutfits(const Point &topLeft) const;
  void DrawSale(const Point &topLeft) const;


private:
  void UpdateAttributes(const Ship &ship, const Depreciation &depreciation, int day);
  void UpdateOutfits(const Ship &ship, const Depreciation &depreciation, int day);


private:
  std::vector<std::string> tableLabels;
  std::vector<std::string> fuelTable;
  std::vector<std::string> energyTable;
  std::vector<std::string> heatTable;

  std::vector<std::string> outfitLabels;
  std::vector<std::string> outfitValues;
  int outfitsHeight = 0;

  std::vector<std::string> saleLabels;
  std::vector<std::string> saleValues;
  int saleHeight = 0;
};



#endif
