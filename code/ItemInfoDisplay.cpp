// ItemInfoDisplay.cpp

#include "ItemInfoDisplay.h"

#include "Colour.h"
#include "FillShader.h"
#include "FontSet.h"
#include "GameData.h"
#include "Screen.h"
#include "Table.h"

#include <algorithm>
#include <cmath>

using namespace std;

namespace {
  const int HOVER_TIME = 60;
}



ItemInfoDisplay::ItemInfoDisplay()
{
  description.SetAlignment(Font::JUSTIFIED);
  description.SetWrapWidth(WIDTH - 20);
  description.SetFont(FontSet::Get(18));

  hoverText.SetAlignment(Font::JUSTIFIED);
  hoverText.SetWrapWidth(WIDTH - 20);
  hoverText.SetFont(FontSet::Get(18));
}



// Get the panel width.
int ItemInfoDisplay::PanelWidth()
{
  return WIDTH;
}



// Get the height of each of the three panels.
int ItemInfoDisplay::MaximumHeight() const
{
  return maximumHeight;
}



int ItemInfoDisplay::DescriptionHeight() const
{
  return descriptionHeight;
}



int ItemInfoDisplay::AttributesHeight() const
{
  return attributesHeight;
}



// Draw each of the panels.
void ItemInfoDisplay::DrawDescription(const Point &topLeft) const
{
  description.Draw(topLeft + Point(10., 12.), *GameData::Colours().Get("medium"));
}



void ItemInfoDisplay::DrawAttributes(const Point &topLeft) const
{
  Draw(topLeft, attributeLabels, attributeValues);
}



void ItemInfoDisplay::DrawTooltips() const
{
  if(!hoverCount || hoverCount-- < HOVER_TIME || !hoverText.Height())
    return;

  Point textSize(hoverText.WrapWidth(), hoverText.Height() - hoverText.ParagraphBreak());
  Point boxSize = textSize + Point(20., 20.);

  Point topLeft = hoverPoint;
  if(topLeft.X() + boxSize.X() > Screen::Right())
    topLeft.X() -= boxSize.X();
  if(topLeft.Y() + boxSize.Y() > Screen::Bottom())
    topLeft.Y() -= boxSize.Y();

  FillShader::Fill(topLeft + .5 * boxSize, boxSize, *GameData::Colours().Get("tooltip background"));
  hoverText.Draw(topLeft + Point(10., 10.), *GameData::Colours().Get("medium"));
}



// Update the location where the mouse is hovering.
void ItemInfoDisplay::Hover(const Point &point)
{
  hoverPoint = point;
  hasHover = true;
}



void ItemInfoDisplay::ClearHover()
{
  hasHover = false;
}



void ItemInfoDisplay::UpdateDescription(const string &text, const vector<string> &licences, bool isShip)
{
  if(licences.empty())
    description.Wrap(text);
  else
  {
    static const string NOUN[2] = {"outfit", "ship"};
    string fullText = text + "\tTo purchase this " + NOUN[isShip] + " you must have ";
    for(unsigned i = 0; i < licences.size(); ++i)
    {
      bool isVoweled = false;
      for(const char &c : "aeiou")
        if(*licences[i].begin() == c || *licences[i].begin() == toupper(c))
          isVoweled = true;
      if(i)
      {
        if(licences.size() > 2)
          fullText += ", ";
        else
          fullText += " ";
      }
      if(i && i == licences.size() - 1)
        fullText += "and ";
      fullText += (isVoweled ? "an " : "a ") + licences[i] + " Licence";

    }
    fullText += ".\n";
    description.Wrap(fullText);
  }

  // Pad by 10 pixels on the top and bottom.
  descriptionHeight = description.Height() + 20;
}



Point ItemInfoDisplay::Draw(Point point, const vector<string> &labels, const vector<string> &values) const
{
  // Add ten pixels of padding at the top.
  point.Y() += 10.;

  // Get standard colours to draw with.
  const Colour &labelColour = *GameData::Colours().Get("medium");
  const Colour &valueColour = *GameData::Colours().Get("bright");

  Table table;
  // Use 10-pixel margins on both sides.
  table.AddColumn(10, Table::LEFT);
  table.AddColumn(WIDTH - 10, Table::RIGHT);
  table.SetHighlight(0, WIDTH);
  table.DrawAt(point);

  for(unsigned i = 0; i < labels.size() && i < values.size(); ++i)
  {
    if(labels[i].empty())
    {
      table.DrawGap(10);
      continue;
    }

    CheckHover(table, labels[i]);
    table.Draw(labels[i], values[i].empty() ? valueColour : labelColour);
    table.Draw(values[i], valueColour);
  }
  return table.GetPoint();
}



void ItemInfoDisplay::CheckHover(const Table &table, const string &label) const
{
  if(!hasHover)
    return;

  Point distance = hoverPoint - table.GetCentrePoint();
  Point radius = .5 * table.GetRowSize();
  if(abs(distance.X()) < radius.X() && abs(distance.Y()) < radius.Y())
  {
    hoverCount += 2 * (label == hover);
    hover = label;
    if(hoverCount >= HOVER_TIME)
    {
      hoverCount = HOVER_TIME;
      hoverText.Wrap(GameData::Tooltip(label));
    }
  }
}
