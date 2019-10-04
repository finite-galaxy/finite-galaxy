// MapOutfitterPanel.h

#ifndef MAP_OUTFITTER_PANEL_H_
#define MAP_OUTFITTER_PANEL_H_

#include "MapSalesPanel.h"

#include "OutfitInfoDisplay.h"

#include <vector>

class Outfit;
class PlayerInfo;
class Sprite;



// A panel that displays the galaxy star map, along with a side panel showing
// all outfits that are for sale in known systems. You can click on one of them
// to see which systems it is available in.
class MapOutfitterPanel : public MapSalesPanel {
public:
  explicit MapOutfitterPanel(PlayerInfo &player);
  explicit MapOutfitterPanel(const MapPanel &panel, bool onlyHere = false);


protected:
  virtual const Sprite *SelectedSprite() const override;
  virtual const Sprite *CompareSprite() const override;
  virtual const ItemInfoDisplay &SelectedInfo() const override;
  virtual const ItemInfoDisplay &CompareInfo() const override;
  virtual const std::string &KeyLabel(int index) const override;

  virtual void Select(int index) override;
  virtual void Compare(int index) override;
  virtual double SystemValue(const System *system) const override;
  virtual int FindItem(const std::string &text) const override;

  virtual void DrawItems() override;


private:
  void Init();


private:
  std::map<std::string, std::vector<const Outfit *>> catalogue;
  std::vector<const Outfit *> list;

  const Outfit *selected = nullptr;
  const Outfit *compare = nullptr;

  OutfitInfoDisplay selectedInfo;
  OutfitInfoDisplay compareInfo;
};



#endif
