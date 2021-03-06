// GameEvent.h

#ifndef GAME_EVENT_H_
#define GAME_EVENT_H_

#include "ConditionSet.h"
#include "DataNode.h"
#include "Date.h"

#include <list>
#include <string>
#include <vector>

class DataWriter;
class Planet;
class PlayerInfo;
class System;



// This represents something that happens that changes the universe. For
// example, a system may be taken over by a different government, or a new type
// of ship or weapon may become available for purchase. Events that do not
// specify a date on which they occur will happen in response to missions. An
// event always sets the "event: <name>" condition when it occurs, which allows
// you to use the mission framework to specify a message that can be shown to
// the player the next time they land  on a planet after that event happens.
class GameEvent {
public:
  GameEvent() = default;
  // Construct and Load() at the same time.
  GameEvent(const DataNode &node);

  void Load(const DataNode &node);
  void Save(DataWriter &out) const;
  const std::string &Name() const;

  const Date &GetDate() const;
  void SetDate(const Date &date);

  void Apply(PlayerInfo &player);

  const std::list<DataNode> &Changes() const;


private:
  Date date;
  std::string name;
  ConditionSet conditionsToApply;
  std::list<DataNode> changes;
  std::vector<const System *> systemsToVisit;
  std::vector<const Planet *> planetsToVisit;
  std::vector<const System *> systemsToUnvisit;
  std::vector<const Planet *> planetsToUnvisit;
};



#endif
