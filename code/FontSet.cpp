// FontSet.cpp

#include "FontSet.h"

#include "Files.h"
#include "Font.h"

#include <cstdlib>
#include <map>
#include <fontconfig/fontconfig.h>

using namespace std;

namespace {
  map<int, Font> fonts;

  string fontDescription("Linux Libertine Display");
  string fontDescriptionForLayout("Linux Libertine Display");
  string fontLanguage("en");
  char envBackend[] = "PANGOCAIRO_BACKEND=fc";
}



void FontSet::Add(const std::string &fontsDir)
{
  const FcChar8* fontsDirFc8 = reinterpret_cast<const FcChar8*>(fontsDir.c_str());
  const string cfgFile = fontsDir + "fonts.conf";
  const FcChar8* cfgFileFc8 = reinterpret_cast<const FcChar8*>(cfgFile.c_str());
  FcConfig *fcConfig = FcConfigGetCurrent();
  if(!FcConfigAppFontAddDir(fcConfig, fontsDirFc8))
    Files::LogError("Warning: Fail to load fonts in \"" + fontsDir + "\".");
  if(!FcConfigParseAndLoad(fcConfig, cfgFileFc8, FcFalse))
    Files::LogError("Warning: Parse error in \"" + cfgFile + "\".");
}



const Font &FontSet::Get(int size)
{
  if(!fonts.count(size))
  {
    putenv(envBackend);
    Font &font = fonts[size];
    font.SetFontDescription(fontDescription);
    font.SetLayoutReference(fontDescriptionForLayout);
    font.SetPixelSize(size);
    font.SetLanguage(fontLanguage);
  }
  return fonts[size];
}



void SetFontDescription(const std::string &desc)
{
  fontDescription = desc;
  for(auto &it : fonts)
    it.second.SetFontDescription(desc);
}



void SetReferenceForLayout(const std::string &desc)
{
  fontDescriptionForLayout = desc;
  for(auto &it : fonts)
    it.second.SetLayoutReference(desc);
}



void SetLanguage(const std::string &lang)
{
  fontLanguage = lang;
  for(auto &it : fonts)
    it.second.SetLanguage(lang);
}

