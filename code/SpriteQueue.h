// SpriteQueue.h

#ifndef SPRITE_QUEUE_H_
#define SPRITE_QUEUE_H_

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

class ImageBuffer;
class ImageSet;
class Mask;
class Sprite;



// Class for queuing up a list of sprites to be loaded from the disk, with a set of
// worker threads that begins loading them as soon as they are added.
class SpriteQueue {
public:
  SpriteQueue();
  ~SpriteQueue();

  // Add a sprite to load.
  void Add(const std::shared_ptr<ImageSet> &images);
  // Unload the texture for the given sprite (to free up memory).
  void Unload(const std::string &name);
  // Upload more iamges and find out our percent completion.
  double Progress();
  // Finish loading.
  void Finish();

  // Thread entry point.
  void operator()();


private:
  double DoLoad(std::unique_lock<std::mutex> &lock);


private:
  // These are the image sets that need to be loaded from disk.
  std::queue<std::shared_ptr<ImageSet>> toRead;
  std::mutex readMutex;
  std::condition_variable readCondition;
  int added = 0;

  // These image sets have been loaded from disk but have not been uplodaed.
  std::queue<std::shared_ptr<ImageSet>> toLoad;
  std::mutex loadMutex;
  std::condition_variable loadCondition;
  int completed = 0;

  // These sprites must be unloaded to reclaim GPU memory.
  std::queue<std::string> toUnload;

  // Worker threads for loading sprites from disk.
  std::vector<std::thread> threads;
};

#endif
