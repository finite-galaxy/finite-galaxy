// SpriteSet.cpp

#include "SpriteSet.h"

#include "Sprite.h"

#include <map>

using namespace std;

namespace {
	map<string, Sprite> sprites;
}



const Sprite *SpriteSet::Get(const string &name)
{
	return Modify(name);
}



Sprite *SpriteSet::Modify(const string &name)
{
	auto it = sprites.find(name);
	if(it == sprites.end())
		it = sprites.insert(make_pair(name, Sprite(name))).first;
	return &it->second;
}
