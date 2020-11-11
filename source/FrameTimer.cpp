// FrameTimer.cpp

#include "FrameTimer.h"

#include <thread>

using namespace std;



// Create a timer that is just responsible for measuring the time that
// elapses until Time() is called.
FrameTimer::FrameTimer()
{
  next = chrono::steady_clock::now();
}



// Create a frame timer that will space frames out at exactly the given FPS,
// _unless_ a frame takes too long by at least the given lag, in which case
// the next frame happens immediately but no "catch-up" is done.
FrameTimer::FrameTimer(int fps, int maxLagMsec)
  : step(chrono::nanoseconds(1000'000'000 / fps)),
  maxLag(chrono::milliseconds(maxLagMsec))
{
  next = chrono::steady_clock::now();
  Step();
}



// Wait until the next frame should begin.
void FrameTimer::Wait()
{
  // Note: in theory this could get interrupted by a signal handler, although
  // it's unlikely the program will receive any signals that do not terminate
  // it and that it does not ignore. But, the worst that would happen in that
  // case is that this particular frame will end too quickly, and then it will
  // go back to normal for the next one.
  chrono::steady_clock::time_point now = chrono::steady_clock::now();
  if(now < next)
  {
    // This should never happen with a true steady clock, but make sure that
    // the sleep time is never longer than one frame.
    if(now + step + maxLag < next)
      next = now + step;

    this_thread::sleep_until(next);
    now = chrono::steady_clock::now();
  }
  // If the lag is too high, don't try to do catch-up.
  if(now - next > maxLag)
    next = now;

  Step();
}



// Find out how long it has been since this timer was created, in seconds.
double FrameTimer::Time() const
{
  chrono::steady_clock::time_point now = chrono::steady_clock::now();
  return chrono::duration_cast<chrono::nanoseconds>(now - next).count() * .000'000'001;
}



// Change the frame rate (for viewing in slow motion).
void FrameTimer::SetFrameRate(int fps)
{
  step = chrono::nanoseconds(1000'000'000 / fps);
}



// Calculate when the next frame should begin.
void FrameTimer::Step()
{
  next += step;
}
