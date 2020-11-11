// GameEvent.h

#ifndef GAMEWINDOW_H_
#define GAMEWINDOW_H_

#include <string>

// This class is a collection of global functions for handling SDL_Windows.
class GameWindow
{
public:
  static bool Init();
  static void Quit();

  // Paint the next frame in the main window.
  static void Step();

  // Ensure the proper icon is set on the main window.
  static void SetIcon();

  // Handle resize events of the main window.
  static void AdjustViewport();

  // Last known windowed-mode width & height.
  static int Width();
  static int Height();
  // 
  static bool IsMaximized();
  static bool IsFullscreen();
  static void ToggleFullscreen();  

  // Check if the initialized window system supports OpenGL texture_swizzle.
  static bool HasSwizzle();

  // Print the error message in the terminal, error file, and message box.
  // Checks for video system errors and records those as well.
  static void ExitWithError(const std::string& message);
};



#endif
