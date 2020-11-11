// News.h

#ifndef NEWS_H_
#define NEWS_H_

#include "LocationFilter.h"
#include "Phrase.h"

#include <string>
#include <vector>

class DataNode;
class Planet;
class Sprite;



// This represents a person you can "talk to" in the spaceport to get some local
// news. One specification can contain many possible portraits and messages.
class News {
public:
  void Load(const DataNode &node);

  // Check if this news item is available on the given planet.
  bool Matches(const Planet *planet) const;

  // Get the speaker's name.
  std::string Name() const;
  // Pick a portrait at random out of the possible options.
  const Sprite *Portrait() const;
  // Get the speaker's message, chosen randomly.
  std::string Message() const;


private:
  LocationFilter location;

  Phrase names;
  std::vector<const Sprite *> portraits;
  Phrase messages;
};

#endif
