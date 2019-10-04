// Trade.h

#ifndef TRADE_H_
#define TRADE_H_

#include <string>
#include <vector>

class DataNode;



// Class representing all the commodities that are available to trade. Each
// commodity has a certain normal price range, and can also specify specific
// items that are a kind of that commodity, so that a mission can have you
// deliver, say, "eggs" or "frozen meat" instead of generic "food".
class Trade {
public:
  class Commodity {
  public:
    std::string name;
    int low = 0;
    int high = 0;
    std::vector<std::string> items;
  };


public:
  void Load(const DataNode &node);

  const std::vector<Commodity> &Commodities() const;
  const std::vector<Commodity> &SpecialCommodities() const;


private:
  std::vector<Commodity> commodities;
  std::vector<Commodity> specialCommodities;
};



#endif
