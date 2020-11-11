// ShipyardPanel.h

#ifndef SHIPYARD_PANEL_H_
#define SHIPYARD_PANEL_H_

#include "ShopPanel.h"

#include "Sale.h"

#include <string>

class PlayerInfo;
class Point;
class Ship;



// Class representing the shipyard UI panel, which allows you to buy new ships
// or to sell any of the ones you own. For certain ships, you may need to have a
// certain licence to buy them, in which case the cost of the licence is added
// to the cost of the ship. (This is intended to be an annoyance, representing
// a government that is particularly repressive of independent pilots.)
class ShipyardPanel : public ShopPanel {
public:
  explicit ShipyardPanel(PlayerInfo &player);


protected:
  virtual int TileSize() const override;
  virtual int DrawPlayerShipInfo(const Point &point) override;
  virtual bool HasItem(const std::string &name) const override;
  virtual void DrawItem(const std::string &name, const Point &point, int scrollY) override;
  virtual int DividerOffset() const override;
  virtual int DetailWidth() const override;
  virtual int DrawDetails(const Point &centre) override;
  virtual bool CanBuy() const override;
  virtual void Buy(bool fromCargo = false) override;
  virtual void FailBuy() const override;
  virtual bool CanSell(bool toCargo = false) const override;
  virtual void Sell(bool toCargo = false) override;
  virtual bool CanSellMultiple() const override;


private:
  void BuyShip(const std::string &name);
  void SellShip();


private:
  int modifier;

  Sale<Ship> shipyard;
};


#endif
