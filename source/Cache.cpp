// Cache.cpp

#include "Cache.h"

#include <set>

using namespace std;

namespace {
  // This is ensure that the return value has been initialized when it uses
  // for a initialization of any static variables.
  set<CacheBase*> &GetCacheObjectSet()
  {
    // Don't destruct this object because it must be alive until all static variables are destructed.
    static set<CacheBase*> *CacheObjectSet(new set<CacheBase*>);
    return *CacheObjectSet;
  }
}



CacheBase::CacheBase()
  : stepCount(0), updateInterval(3600)
{
}



void CacheBase::SetUpdateInterval(size_t newInterval)
{
  updateInterval = newInterval;
  if(stepCount >= updateInterval)
  {
    NextGeneration();
    stepCount = 0;
  }
}



void CacheBase::Step()
{
  set<CacheBase*> &objects = GetCacheObjectSet();
  for(auto &it : objects)
    it->StepThis();
}



void CacheBase::StepThis()
{
  ++stepCount;
  if(stepCount >= updateInterval)
  {
    NextGeneration();
    stepCount = 0;
  }
}


void CacheBase::RegisterCacheObject(CacheBase *cacheObject)
{
  GetCacheObjectSet().insert(cacheObject);
}



void CacheBase::UnregisterCacheObject(CacheBase *cacheObject)
{
  GetCacheObjectSet().erase(cacheObject);
}

