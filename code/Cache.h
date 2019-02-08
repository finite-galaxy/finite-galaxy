// Cache.h

#ifndef CACHE_H_
#define CACHE_H_

#include <functional>
#include <list>
#include <unordered_map>
#include <utility>

#if __GNUC__ == 4 && __GNUC_MINOR__ == 8
// GCC 4.8 reports a strange error, no type named std::unordered_map<>::iterator.
#define GCC48_WORKAROUND
#endif



// This is the common timer class for the Cache class.
class CacheBase {
public:
  CacheBase();

  // Set an interval to change the generations.
  // An auto expired cache may recycle a value after changing 2 generations.
  // The newInterval's unit is a number of steps.
  void SetUpdateInterval(size_t newInterval);

  // Notify to progress a frame time.
  // This function will call StepThis() for all instances of Cache.
  static void Step();

protected:
  // Register an instance of CacheBase when constructing it.
  static void RegisterCacheObject(CacheBase *cacheObject);
  // Unregister an instance of CacheBase when destructing it.
  static void UnregisterCacheObject(CacheBase *cacheObject);

private:
  // Called from Step().
  void StepThis();
  // Notify to progress a generation.
  virtual void NextGeneration() = 0;

  size_t stepCount;
  size_t updateInterval;
};



// Do nothing at recycle of a cache element.
template<class T>
class DoNothingAtRecycle {
public:
  void operator()(T& data) const
  {}
};



// This is a generic cache class which has some template parameters same as unordered_map.
// Key is a type of the key values.
// T is a type of the mapped values.
// Hash is a type of the unary function that returns the hash value of a key value.
// If autoExpired is true, its instance may recycle a value after changing 2 generations. It means that any data keep for one generation at least.
// If autoExpired is false, the cache keeps all data unless Expire() has been called.
// AtRecycle is a function class. The function is called before recycling a value.
template<class Key, class T, bool autoExpired = false, class Hash = std::hash<Key>, class AtRecycle = DoNothingAtRecycle<T>>
class Cache : public CacheBase {
public:
  Cache();
  virtual ~Cache() noexcept;
  // Don't copy this instance.
  Cache(const Cache &a) = delete;
  Cache &operator=(const Cache &a) = delete;
  // But it may move.
  Cache(Cache &&a) noexcept;
  Cache &operator=(Cache &&a) noexcept;

  // Set a value and return it owned by this cache.
  // Don't set the key that exists in the cache.
  // This function set the useCount to 1.
  // Note: The newData is rvalue reference. You may use std::move() or construct a temporary object.
  const T &Set(const Key &key, T &&newData);
  // Same as Set(), but it does not try to recycle a value in this instance.
  const T &New(const Key &key, T &&newData);
  // Use a value mapped the key.
  // If a return value.second is true, then the return value.first is the mapped value.
  // Otherwise, this cache has no value mapped the key.
  std::pair<const T*, bool> Use(const Key &key);
  // Recycle an expired value on a caller side. This instance does not call the AtRecycle.
  // If a return value.second is false, this cache has no expired data.
  std::pair<T, bool> Recycle();
  // Decrement the useCount and expire the value mapped the key if useCount == 0.
  // This function has no effect for an auto expired cache.
  void Expire(const Key &key);
  // Clear all data in this cache. This function calls AtRecycle for all values.
  void Clear();

private:
  struct ContainerElement;
  typedef std::list<ContainerElement> ContainerType;
  typedef typename ContainerType::iterator ContainerPtrType;
  typedef std::unordered_map<Key, ContainerPtrType, Hash> DirectoryType;
  // The container contains actual data.
  // It is an ordered list based on the update time.
  ContainerType container;
  // The directory is a hash table, which maps the key to an iterator of the container.
  DirectoryType directory;

  struct ContainerElement {
    T data;
    size_t useCount;
#if !defined(GCC48_WORKAROUND)
    typename DirectoryType::iterator index;

    ContainerElement(const T &d, size_t c, const typename DirectoryType::iterator &i)
      : data(d), useCount(c), index(i)
    {}
#define INDEX(d, e) ((e).index)
#define EMPLACE_FRONT(directory, key, data) emplace_front(data, 1, (directory).end())
#else
    Key key;

    ContainerElement(const T &d, size_t c, const Key &k)
      : data(d), useCount(c), key(k)
    {}
#define INDEX(d, e) ((d).find((e).key))
#define EMPLACE_FRONT(directory, key, data) emplace_front(data, 1, key)
#endif
  };

  // begin() <= expire <= readyToRecycle <= end()
  // In the range [expired end()), all instances of ContainerElement is expired.
  ContainerPtrType expired;
  // In the range [readyToRecycle end()), all instances of ContainerElement may be recycled.
  ContainerPtrType readyToRecycle;

  void AdjustPointerWhenErase(ContainerPtrType &it)
  {
    if(readyToRecycle == it)
      ++readyToRecycle;
    if(expired == it)
      ++expired;
  }

  virtual void NextGeneration()
  {
    readyToRecycle = expired;
    if(autoExpired)
      expired = container.begin();
  }
};



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
Cache<Key, T, autoExpired, Hash, AtRecycle>::Cache()
  : container(), directory(), expired(container.end()), readyToRecycle(container.end())
{
  RegisterCacheObject(this);
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
Cache<Key, T, autoExpired, Hash, AtRecycle>::~Cache() noexcept
{
  Clear();
  UnregisterCacheObject(this);
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
Cache<Key, T, autoExpired, Hash, AtRecycle>::Cache(Cache<Key, T, autoExpired, Hash, AtRecycle> &&a) noexcept
  : Cache()
{
  *this = std::move(a);
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
Cache<Key, T, autoExpired, Hash, AtRecycle>
&Cache<Key, T, autoExpired, Hash, AtRecycle>::operator=(Cache<Key, T, autoExpired, Hash, AtRecycle> &&a) noexcept
{
  if(this == &a)
    return *this;
  Clear();
  const bool noExpired = a.expired == a.container.end();
  const bool noReadyToRecycle = a.readyToRecycle == a.container.end();
  container = std::move(a.container);
  directory = std::move(a.directory);
  if(noExpired)
    // move() doesn't keep an end() iterator.
    expired = container.end();
  else
    expired = a.expired;
  if(noReadyToRecycle)
    readyToRecycle = container.end();
  else
    readyToRecycle = a.readyToRecycle;
  return *this;
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
const T &Cache<Key, T, autoExpired, Hash, AtRecycle>::Set(const Key &key, T &&newData)
{
  if(readyToRecycle == container.end())
    container.EMPLACE_FRONT(directory, key, newData);
  else
  {
    auto it = --container.end();
    AtRecycle()(it->data);
    AdjustPointerWhenErase(it);
    it->data = std::move(newData);
    if(!autoExpired)
      it->useCount = 1;
    directory.erase(INDEX(directory, *it));
    container.splice(container.begin(), container, it);
  }
  const auto p = directory.emplace(key, container.begin());
  INDEX(directory, container.front()) = p.first;
  return container.front().data;
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
const T &Cache<Key, T, autoExpired, Hash, AtRecycle>::New(const Key &key, T &&newData)
{
  container.EMPLACE_FRONT(directory, key, newData);
  const auto p = directory.emplace(key, container.begin());
  INDEX(directory, container.front()) = p.first;
  return container.front().data;
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
std::pair<const T*, bool> Cache<Key, T, autoExpired, Hash, AtRecycle>::Use(const Key &key)
{
  auto index = directory.find(key);
  if(index == directory.end())
    return std::make_pair(nullptr, false);
  else
  {
    auto &it = index->second;
    AdjustPointerWhenErase(it);
    container.splice(container.begin(), container, it);
    if(!autoExpired)
      it->useCount++;
    return std::make_pair(&it->data, true);
  }
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
std::pair<T, bool> Cache<Key, T, autoExpired, Hash, AtRecycle>::Recycle()
{
  if(readyToRecycle == container.end())
    return std::make_pair(T(), false);
  else
  {
    auto it = --container.end();
    AdjustPointerWhenErase(it);
    const T result = it->data;
    directory.erase(INDEX(directory, *it));
    container.pop_back();
    return std::make_pair(result, true);
  }
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
void Cache<Key, T, autoExpired, Hash, AtRecycle>::Expire(const Key &key)
{
  if(autoExpired)
    return;
  auto index = directory.find(key);
  if(index != directory.end())
  {
    auto &it = index->second;
    if(it->useCount > 0)
      --it->useCount;
    if(it->useCount == 0)
    {
      AdjustPointerWhenErase(it);
      container.splice(expired, container, it);
      --expired;
    }
  }
}



template<class Key, class T, bool autoExpired, class Hash, class AtRecycle>
void Cache<Key, T, autoExpired, Hash, AtRecycle>::Clear()
{
  for(auto &value : container)
    AtRecycle()(value.data);
  container.clear();
  directory.clear();
  expired = container.end();
  readyToRecycle = container.end();
}



#ifdef GCC48_WORKAROUND
#undef GCC48_WORKAROUND
#endif
#undef INDEX
#undef EMPLACE_FRONT

#endif

