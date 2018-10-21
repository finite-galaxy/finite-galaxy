// StartConditions.cpp

#include "StartConditions.h"

#include "DataNode.h"
#include "GameData.h"
#include "Planet.h"
#include "Ship.h"
#include "System.h"

using namespace std;



void StartConditions::Load(const DataNode &node)
{
  for(const DataNode &child : node)
  {
    if(child.Token(0) == "date" && child.Size() >= 4)
      date = Date(child.Value(1), child.Value(2), child.Value(3));
    else if(child.Token(0) == "system" && child.Size() >= 2)
      system = GameData::Systems().Get(child.Token(1));
    else if(child.Token(0) == "planet" && child.Size() >= 2)
      planet = GameData::Planets().Get(child.Token(1));
    else if(child.Token(0) == "account")
      accounts.Load(child);
    else if(child.Token(0) == "ship" && child.Size() >= 2)
      ships.emplace_back(child);
    else
      conditions.Add(child);
  }
}



// Finish loading the ship definitions.
void StartConditions::FinishLoading()
{
  for(Ship &ship : ships)
    ship.FinishLoading(true);
}



Date StartConditions::GetDate() const
{
  return date ? date : Date(16, 11, 3013);
}



const Planet *StartConditions::GetPlanet() const
{
  return planet ? planet : GameData::Planets().Get("New Boston");
}



const System *StartConditions::GetSystem() const
{
  return system ? system : GetPlanet() ? GetPlanet()->GetSystem() : GameData::Systems().Get("Rutilicus");
}



const Account &StartConditions::GetAccounts() const
{
  return accounts;
}



const ConditionSet &StartConditions::GetConditions() const
{
  return conditions;
}



const list<Ship> &StartConditions::Ships() const
{
  return ships;
}
