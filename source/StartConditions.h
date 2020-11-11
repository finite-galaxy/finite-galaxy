// StartConditions.h

#ifndef START_CONDITIONS_H_
#define START_CONDITIONS_H_

#include "Account.h"
#include "ConditionSet.h"
#include "Date.h"

#include <list>

class DataNode;
class Planet;
class Ship;
class System;



class StartConditions {
public:
  void Load(const DataNode &node);
  // Finish loading the ship definitions.
  void FinishLoading();

  Date GetDate() const;
  const Planet *GetPlanet() const;
  const System *GetSystem() const;
  const Account &GetAccounts() const;
  const ConditionSet &GetConditions() const;
  const std::list<Ship> &Ships() const;


private:
  Date date;
  const Planet *planet = nullptr;
  const System *system = nullptr;
  Account accounts;
  ConditionSet conditions;
  std::list<Ship> ships;
};



#endif
