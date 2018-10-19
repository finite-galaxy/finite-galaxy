// FontSet.cpp

#include "FontSet.h"

#include "Font.h"

#include <map>

using namespace std;

namespace {
	map<int, Font> fonts;
}



void FontSet::Add(const string &path, int size)
{
	if(!fonts.count(size))
		fonts[size].Load(path);
}



const Font &FontSet::Get(int size)
{
	return fonts[size];
}
