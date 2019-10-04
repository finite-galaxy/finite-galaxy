// Dictionary.h

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <string>
#include <utility>
#include <vector>



// This class stores a mapping from character string keys to values, in a way
// that prioritizes fast lookup time at the expense of longer construction time
// compared to an STL map. That makes it suitable for ship attributes, which are
// changed much less frequently than they are queried.
class Dictionary : private std::vector<std::pair<const char *, double>> {
public:
  // Access a key for modifying it:
  double &operator[](const char *key);
  double &operator[](const std::string &key);
  // Get the value of a key, or 0 if it does not exist:
  double Get(const char *key) const;
  double Get(const std::string &key) const;

  // Expose certain functions from the underlying vector:
  using std::vector<std::pair<const char *, double>>::empty;
  using std::vector<std::pair<const char *, double>>::begin;
  using std::vector<std::pair<const char *, double>>::end;
};



#endif
