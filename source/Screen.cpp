// Screen.cpp

#include "Screen.h"

#include <algorithm>

using namespace std;

namespace {
	int RAW_WIDTH = 0;
	int RAW_HEIGHT = 0;
	int WIDTH = 0;
	int HEIGHT = 0;
	int ZOOM = 100;
	bool HIGH_DPI = false;
}



void Screen::SetRaw(int width, int height)
{
	RAW_WIDTH = width;
	RAW_HEIGHT = height;
	SetZoom(ZOOM);
}



int Screen::Zoom()
{
	return ZOOM;
}



void Screen::SetZoom(int percent)
{
	ZOOM = max(100, min(200, percent));
	WIDTH = RAW_WIDTH * 100 / ZOOM;
	HEIGHT = RAW_HEIGHT * 100 / ZOOM;
}



// Specify that this is a high-DPI window.
void Screen::SetHighDPI(bool isHighDPI)
{
	HIGH_DPI = isHighDPI;
}



// This is true if the screen is high DPI, or if the zoom is above 100%.
bool Screen::IsHighResolution()
{
	return HIGH_DPI || (ZOOM > 100);
}



Point Screen::Dimensions()
{
	return Point(WIDTH, HEIGHT);
}



int Screen::Width()
{
	return WIDTH;
}



int Screen::Height()
{
	return HEIGHT;
}



int Screen::RawWidth()
{
	return RAW_WIDTH;
}



int Screen::RawHeight()
{
	return RAW_HEIGHT;
}




int Screen::Left()
{
	return WIDTH / -2;
}



int Screen::Top()
{
	return HEIGHT / -2;
}



int Screen::Right()
{
	return WIDTH / 2;
}



int Screen::Bottom()
{
	return HEIGHT / 2;
}



Point Screen::TopLeft()
{
	return Point(-.5 * WIDTH, -.5 * HEIGHT);
}



Point Screen::TopRight()
{
	return Point(.5 * WIDTH, -.5 * HEIGHT);
}



Point Screen::BottomLeft()
{
	return Point(-.5 * WIDTH, .5 * HEIGHT);
}



Point Screen::BottomRight()
{
	return Point(.5 * WIDTH, .5 * HEIGHT);
}
