// Preferences.h

#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <string>



class Preferences {
public:
  static void Load();
  static void Save();

  static bool Has(const std::string &name);
  static void Set(const std::string &name, bool on = true);

  // Toogle the ammo usage preferences, cycling between "never," "frugally,"
  // and "always."
  static void ToggleAmmoUsage();
  static std::string AmmoUsage();

  // Scroll speed preference.
  static int ScrollSpeed();
  static void SetScrollSpeed(int speed);

  // View zoom.
  static double ViewZoom();
  static bool ZoomViewIn();
  static bool ZoomViewOut();

  // Refuel Preferences
  static double GetMaxPrice();
  static void SetMaxPrice(double price);
};



#endif
