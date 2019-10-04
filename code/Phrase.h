// Phrase.h

#ifndef PHRASE_H_
#define PHRASE_H_

#include <string>
#include <vector>

class DataNode;



// Class representing a set of rules for generating text strings from words.
class Phrase {
public:
  void Load(const DataNode &node);

  const std::string &Name() const;
  std::string Get() const;


private:
  bool ReferencesPhrase(const Phrase *phrase) const;


private:
  class Part {
  public:
    std::vector<std::string> words;
    std::vector<const Phrase *> phrases;
  };


private:
  std::string name;
  std::vector<std::vector<Part>> parts;
};



#endif
