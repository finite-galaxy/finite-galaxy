// FrameTimer.h

#ifndef FRAME_TIMER_H_
#define FRAME_TIMER_H_

#include <chrono>



// Class to be used for enforcing a certain frame rate. This ensures that the
// frames are not drawn faster than a certain speed, but if the calculations or
// the graphics cannot keep up it will allow things to go slower for a few frames without trying to "catch up" by making the subsequent frame faster.
class FrameTimer {
public:
  // Create a timer that is just responsible for measuring the time that
  // elapses until Time() is called.
  FrameTimer();
  // Create a frame timer that will space frames out at exactly the given FPS,
  // _unless_ a frame takes too long by at least the given lag, in which case
  // the next frame happens immediately but no "catch-up" is done.
  explicit FrameTimer(int fps, int maxLagMsec = 5);

  // Wait until the next frame should begin.
  void Wait();
  // Find out how long it has been since this timer was created, in seconds.
  double Time() const;

  // Change the frame rate (for viewing in slow motion).
  void SetFrameRate(int fps);


private:
  // Calculate when the next frame should begin.
  void Step();


private:
  std::chrono::steady_clock::time_point next;
  std::chrono::steady_clock::duration step;
  std::chrono::steady_clock::duration maxLag;
};



#endif
