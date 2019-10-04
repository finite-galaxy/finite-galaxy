// Messages.cpp

#include "Messages.h"

#include <mutex>

using namespace std;

namespace {
  mutex incomingMutex;

  vector<pair<string, bool>> incoming;
  vector<Messages::Entry> list;
}



// Add a message to the list.
void Messages::Add(const string &message, bool isImportant)
{
  lock_guard<mutex> lock(incomingMutex);
  incoming.emplace_back(message, isImportant);
}



// Get the messages for the given game step. Any messages that are too old
// will be culled out, and new ones that have just been added will have
// their "step" set to the given value.
const vector<Messages::Entry> &Messages::Get(int step)
{
  lock_guard<mutex> lock(incomingMutex);

  // Load the incoming messages.
  for(const pair<string, bool> &item : incoming)
  {
    const string &message = item.first;
    bool isImportant = item.second;

    // If this message is not important and it is already being shown in the
    // list, ignore it.
    if(!isImportant)
    {
      bool skip = false;
      for(const Messages::Entry &entry : list)
        skip |= (entry.message == message);
      if(skip)
        continue;
    }

    // For each incoming message, if it exactly matches an existing message,
    // replace that one with this new one.
    auto it = list.begin();
    while(it != list.end())
    {
      // Each time a new message comes in, "age" all the existing ones to
      // limit how many of them appear at once.
      it->step -= 60;
      // Also erase messages that have reached the end of their lifetime.
      if((isImportant && it->message == message) || it->step < step - 1000)
        it = list.erase(it);
      else
        ++it;
    }
    list.emplace_back(step, message);
  }
  incoming.clear();
  return list;
}



// Reset the messages (i.e. because a new game was loaded).
void Messages::Reset()
{
  lock_guard<mutex> lock(incomingMutex);
  incoming.clear();
  list.clear();
}
