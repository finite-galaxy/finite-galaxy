// Messages.h

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <string>
#include <vector>



// Class representing messages that should be shown to the user. The messages
// gradually fade as the game steps forward, so each one must remember the game
// step when it came into being. If a new message is added that exactly matches
// an old one, the old version is removed before the new one is added; this is
// to keep repeated messages from filling up the whole screen.
class Messages {
public:
  class Entry {
  public:
    Entry() = default;
    Entry(int step, const std::string &message) : step(step), message(message) {}

    int step;
    std::string message;
  };

public:
  // Add a message to the list.
  static void Add(const std::string &message, bool isImportant = true);

  // Get the messages for the given game step. Any messages that are too old
  // will be culled out, and new ones that have just been added will have
  // their "step" set to the given value.
  static const std::vector<Entry> &Get(int step);

  // Reset the messages (i.e. because a new game was loaded).
  static void Reset();
};



#endif
