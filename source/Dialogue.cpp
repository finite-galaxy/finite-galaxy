// Dialogue.cpp

#include "Dialogue.h"

#include "Colour.h"
#include "Command.h"
#include "Conversation.h"
#include "DataNode.h"
#include "FillShader.h"
#include "Font.h"
#include "FontSet.h"
#include "GameData.h"
#include "MapDetailPanel.h"
#include "PlayerInfo.h"
#include "Point.h"
#include "shift.h"
#include "Sprite.h"
#include "SpriteSet.h"
#include "SpriteShader.h"
#include "UI.h"

#include <cmath>
#include <functional>

using namespace std;

namespace {
  const int WIDTH = 280;

  // Map any conceivable numeric keypad keys to their ASCII values. Most of
  // these will presumably only exist on special programming keyboards.
  const map<SDL_Keycode, char> KEY_MAP = {
    {SDLK_KP_0, '0'},
    {SDLK_KP_1, '1'},
    {SDLK_KP_2, '2'},
    {SDLK_KP_3, '3'},
    {SDLK_KP_4, '4'},
    {SDLK_KP_5, '5'},
    {SDLK_KP_6, '6'},
    {SDLK_KP_7, '7'},
    {SDLK_KP_8, '8'},
    {SDLK_KP_9, '9'},
    {SDLK_KP_AMPERSAND, '&'},
    {SDLK_KP_AT, '@'},
    {SDLK_KP_A, 'a'},
    {SDLK_KP_B, 'b'},
    {SDLK_KP_C, 'c'},
    {SDLK_KP_D, 'd'},
    {SDLK_KP_E, 'e'},
    {SDLK_KP_F, 'f'},
    {SDLK_KP_COLON, ':'},
    {SDLK_KP_COMMA, ','},
    {SDLK_KP_DIVIDE, '/'},
    {SDLK_KP_EQUALS, '='},
    {SDLK_KP_EXCLAM, '!'},
    {SDLK_KP_GREATER, '>'},
    {SDLK_KP_HASH, '#'},
    {SDLK_KP_LEFTBRACE, '{'},
    {SDLK_KP_LEFTPAREN, '('},
    {SDLK_KP_LESS, '<'},
    {SDLK_KP_MINUS, '-'},
    {SDLK_KP_MULTIPLY, '*'},
    {SDLK_KP_PERCENT, '%'},
    {SDLK_KP_PERIOD, '.'},
    {SDLK_KP_PLUS, '+'},
    {SDLK_KP_POWER, '^'},
    {SDLK_KP_RIGHTBRACE, '}'},
    {SDLK_KP_RIGHTPAREN, ')'},
    {SDLK_KP_SPACE, ' '},
    {SDLK_KP_VERTICALBAR, '|'}
  };
}



// Dialogue that has no callback (information only). In this form, there is
// only an "ok" button, not a "cancel" button.
Dialogue::Dialogue(const string &text, Font::Truncate trunc)
{
  Init(text, false, false, trunc);
}



// Mission accept / decline dialogue.
Dialogue::Dialogue(const string &text, PlayerInfo &player, const System *system)
  : intFun(bind(&PlayerInfo::MissionCallback, &player, placeholders::_1)),
  system(system), player(&player)
{
  Init(text, true, true);
}



// Draw this panel.
void Dialogue::Draw()
{
  DrawBackdrop();

  const Sprite *top = SpriteSet::Get("interface/panel/message_top");
  const Sprite *middle = SpriteSet::Get("interface/panel/message_middle");
  const Sprite *bottom = SpriteSet::Get("interface/panel/message_bottom");
  const Sprite *ok = SpriteSet::Get("interface/button/90x30");
  const Sprite *cancel = SpriteSet::Get("interface/button/90x30");

  // Get the position of the top of this dialogue, and of the text and input.
  Point pos(0., (top->Height() + height * middle->Height() + bottom->Height()) * -.5f);
  Point textPos(WIDTH * -.5 + 10., pos.Y() + 20.);
  Point inputPos = Point(0., -20.) - pos;

  // Draw the top section of the dialogue box.
  pos.Y() += top->Height() * .5;
  SpriteShader::Draw(top, pos);
  pos.Y() += top->Height() * .5;

  // The middle section is duplicated depending on how long the text is.
  for(int i = 0; i < height; ++i)
  {
    pos.Y() += middle->Height() * .5;
    SpriteShader::Draw(middle, pos);
    pos.Y() += middle->Height() * .5;
  }

  // Draw the bottom section.
  const Font &font = FontSet::Get(18);
  pos.Y() += bottom->Height() * .5;
  SpriteShader::Draw(bottom, pos);
  pos.Y() += bottom->Height() * .5;

  // Draw the buttons, including optionally the cancel button.
  const Colour &bright = *GameData::Colours().Get("bright");
  const Colour &dim = *GameData::Colours().Get("medium");
  const Colour &back = *GameData::Colours().Get("faint");
  pos.Y() += 20;
  if(canCancel)
  {
    string cancelText = isMission ? "Decline" : "Cancel";
    cancelPos = pos + Point(-100., 0.);
    SpriteShader::Draw(cancel, cancelPos);
    Point labelPos(
      cancelPos.X() - .5 * font.Width(cancelText),
      cancelPos.Y() - .5 * font.Height());
    font.Draw(cancelText, labelPos, !okIsActive ? bright : dim);
  }
  string okText = isMission ? "Accept" : "OK";
  okPos = pos + Point(100., 0.);
  SpriteShader::Draw(ok, okPos);
  Point labelPos(
    okPos.X() - .5 * font.Width(okText),
    okPos.Y() - .5 * font.Height());
  font.Draw(okText, labelPos, okIsActive ? bright : dim);

  // Draw the text.
  text.Draw(textPos, dim);

  // Draw the input, if any.
  if(!isMission && (intFun || stringFun))
  {
    FillShader::Fill(inputPos, Point(WIDTH - 20., 20.), back);

    Point stringPos(
      inputPos.X() - (WIDTH - 20) * .5 + 10.,
      inputPos.Y() - .5 * font.Height());
    const Font::Layout layout{Font::TRUNC_FRONT, WIDTH - 30};
    const string validatedInput(Font::EscapeMarkupHasError(input));
    font.Draw(validatedInput, stringPos, bright, &layout);

    Point barPos(stringPos.X() + font.Width(validatedInput, &layout) + 2., inputPos.Y());
    FillShader::Fill(barPos, Point(1., 16.), dim);
  }
}



// Format and add the text from the given node to the given string.
void Dialogue::ParseTextNode(const DataNode &node, size_t startingIndex, string &text)
{
  for(int i = startingIndex; i < node.Size(); ++i)
  {
    if(!text.empty())
      text += "\n\t";
    text += node.Token(i);
  }
  for(const DataNode &child : node)
    for(int i = 0; i < child.Size(); ++i)
    {
      if(!text.empty())
        text += "\n\t";
      text += child.Token(i);
    }
}



bool Dialogue::KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress)
{
  auto it = KEY_MAP.find(key);
  bool isCloseRequest = key == SDLK_ESCAPE || (key == 'w' && (mod & (KMOD_CTRL | KMOD_GUI)));
  if((it != KEY_MAP.end() || (key >= ' ' && key <= '~')) && !isMission && (intFun || stringFun) && !isCloseRequest)
  {
    int ascii = (it != KEY_MAP.end()) ? it->second : key;
    char c = ((mod & KMOD_SHIFT) ? SHIFT[ascii] : ascii);
    // Caps lock should shift letters, but not any other keys.
    if((mod & KMOD_CAPS) && c >= 'a' && c <= 'z')
      c += 'A' - 'a';

    if(stringFun)
      input += c;
    // Integer input should not allow leading zeros.
    else if(intFun && c == '0' && !input.empty())
      input += c;
    else if(intFun && c >= '1' && c <= '9')
      input += c;
  }
  else if((key == SDLK_DELETE || key == SDLK_BACKSPACE) && !input.empty())
    input.erase(input.length() - 1);
  else if(key == SDLK_TAB && canCancel)
    okIsActive = !okIsActive;
  else if(key == SDLK_LEFT)
    okIsActive = !canCancel;
  else if(key == SDLK_RIGHT)
    okIsActive = true;
  else if(key == SDLK_RETURN || key == SDLK_KP_ENTER || isCloseRequest
      || (isMission && (key == 'a' || key == 'd')))
  {
    // Shortcuts for "accept" and "decline."
    if(key == 'a' || (!canCancel && isCloseRequest))
      okIsActive = true;
    if(key == 'd' || (canCancel && isCloseRequest))
      okIsActive = false;
    if(okIsActive || isMission)
      DoCallback();

    GetUI()->Pop(this);
  }
  else if((key == 'm' || command.Has(Command::MAP)) && system && player)
    GetUI()->Push(new MapDetailPanel(*player, system));
  else
    return false;

  return true;
}



bool Dialogue::Click(int x, int y, int clicks)
{
  Point clickPos(x, y);

  Point ok = clickPos - okPos;
  if(fabs(ok.X()) < 40. && fabs(ok.Y()) < 20.)
  {
    okIsActive = true;
    return DoKey(SDLK_RETURN);
  }

  if(canCancel)
  {
    Point cancel = clickPos - cancelPos;
    if(fabs(cancel.X()) < 40. && fabs(cancel.Y()) < 20.)
    {
      okIsActive = false;
      return DoKey(SDLK_RETURN);
    }
  }

  return true;
}



// Common code from all three constructors:
void Dialogue::Init(const string &message, bool canCancel, bool isMission, Font::Truncate trunc)
{
  this->isMission = isMission;
  this->canCancel = canCancel;
  okIsActive = true;

  text.SetAlignment(Font::JUSTIFIED);
  text.SetWrapWidth(WIDTH - 20);
  text.SetTruncate(trunc);
  text.SetFont(FontSet::Get(18));

  text.Wrap(message);

  // The dialogue with no extenders is 80 pixels tall. 10 pixels at the top and
  // bottom are "padding," but text.Height() over-reports the height by about
  // 5 pixels because it includes its own padding at the bottom. If there is a
  // text input, we need another 20 pixels for it and 10 pixels padding.
  height = 10 + (text.Height() - 5) + 10 + 30 * (!isMission && (intFun || stringFun));
  // Determine how many 40-pixel extension panels we need.
  if(height <= 80)
    height = 0;
  else
    height = (height - 40) / 40;
}



void Dialogue::DoCallback() const
{
  if(isMission)
  {
    if(intFun)
      intFun(okIsActive ? Conversation::ACCEPT : Conversation::DECLINE);

    return;
  }

  if(intFun)
  {
    // Only call the callback if the input can be converted to an int.
    // Otherwise treat this as if the player clicked "cancel."
    try {
      intFun(stoi(input));
    }
    catch(...)
    {
    }
  }

  if(stringFun)
    stringFun(Font::EscapeMarkupHasError(input));

  if(voidFun)
    voidFun();
}