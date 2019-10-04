// Trade.cpp

#include "Trade.h"

#include "DataNode.h"

#include <algorithm>

using namespace std;



void Trade::Load(const DataNode &node)
{
  for(const DataNode &child : node)
  {
    if(child.Token(0) == "commodity" && child.Size() >= 2)
    {
      bool isSpecial = (child.Size() < 4);
      vector<Commodity> &list = (isSpecial ? specialCommodities : commodities);
      auto it = list.begin();
      for( ; it != list.end(); ++it)
        if(it->name == child.Token(1))
          break;
      if(it == list.end())
        it = list.insert(it, Commodity());
  
      it->name = child.Token(1);
      if(!isSpecial)
      {
        it->low = child.Value(2);
        it->high = child.Value(3);
      }
      for(const DataNode &grand : child)
        it->items.push_back(grand.Token(0));
    }
    else if(child.Token(0) == "clear")
      commodities.clear();
    else
      child.PrintTrace("Skipping unrecognized attribute:");
  }
}



const vector<Trade::Commodity> &Trade::Commodities() const
{
  return commodities;
}



const vector<Trade::Commodity> &Trade::SpecialCommodities() const
{
  return specialCommodities;
}
