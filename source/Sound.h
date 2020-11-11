// Sound.h

#ifndef SOUND_H_
#define SOUND_H_

#include <string>



// This is a sound that can be played. The sound's file name will determine
// whether it is looping (ends in '~') or not.
class Sound {
public:
  bool Load(const std::string &path, const std::string &name);

  const std::string &Name() const;

  unsigned Buffer() const;
  bool IsLooping() const;


private:
  std::string name;
  unsigned buffer = 0;
  bool isLooped = false;
};



#endif
