// Dialogue.h

#ifndef DIALOG_H_
#define DIALOG_H_

#include "Font.h"
#include "Panel.h"
#include "Point.h"
#include "WrappedText.h"

#include <functional>
#include <string>

class DataNode;
class PlayerInfo;
class System;



// A dialogue box displays a given message to the player. The box will expand to
// fit the message, and may also include a text input field. The box may have
// only an "ok" button, or may also have a "cancel" button. If this dialogue is
// introducing a mission, the buttons are instead "accept" and "decline". A
// callback function can be given to receive the player's response.
class Dialogue : public Panel {
public:
  // Dialogue that has no callback (information only). In this form, there is
  // only an "ok" button, not a "cancel" button.
  explicit Dialogue(const std::string &text, Font::Truncate trunc = Font::TRUNC_NONE);
  // Mission accept / decline dialogue.
  Dialogue(const std::string &text, PlayerInfo &player, const System *system = nullptr);
  virtual ~Dialogue() = default;

  // Three different kinds of dialogues can be constructed: requesting numerical
  // input, requesting text input, or not requesting any input at all. In any
  // case, the callback is called only if the user selects "ok", not "cancel."
template <class T>
  Dialogue(T *t, void (T::*fun)(int), const std::string &text, Font::Truncate trunc = Font::TRUNC_NONE);
template <class T>
  Dialogue(T *t, void (T::*fun)(int), const std::string &text, int initialValue, Font::Truncate trunc = Font::TRUNC_NONE);

template <class T>
  Dialogue(T *t, void (T::*fun)(const std::string &), const std::string &text, Font::Truncate trunc = Font::TRUNC_NONE);

template <class T>
  Dialogue(T *t, void (T::*fun)(), const std::string &text, Font::Truncate trunc = Font::TRUNC_NONE);

  // Draw this panel.
  virtual void Draw() override;

  // Static method used to convert a DataNode into formatted Dialog text.
  static void ParseTextNode(const DataNode &node, size_t startingIndex, std::string &text);



protected:
  // The use can click "ok" or "cancel", or use the tab key to toggle which
  // button is highlighted and the enter key to select it.
  virtual bool KeyDown(SDL_Keycode key, Uint16 mod, const Command &command, bool isNewPress) override;
  virtual bool Click(int x, int y, int clicks) override;


private:
  // Common code from all three constructors:
  void Init(const std::string &message, bool canCancel = true, bool isMission = false, Font::Truncate trunc = Font::TRUNC_NONE);
  void DoCallback() const;


protected:
  WrappedText text;
  int height;

  std::function<void(int)> intFun;
  std::function<void(const std::string &)> stringFun;
  std::function<void()> voidFun;

  bool canCancel;
  bool okIsActive;
  bool isMission;

  std::string input;

  Point okPos;
  Point cancelPos;

  const System *system = nullptr;
  PlayerInfo *player = nullptr;
};



template <class T>
Dialogue::Dialogue(T *t, void (T::*fun)(int), const std::string &text, Font::Truncate trunc)
  : intFun(std::bind(fun, t, std::placeholders::_1))
{
  Init(text, true, false, trunc);
}



template <class T>
Dialogue::Dialogue(T *t, void (T::*fun)(int), const std::string &text, int initialValue, Font::Truncate trunc)
  : intFun(std::bind(fun, t, std::placeholders::_1))
{
  Init(text, true, false, trunc);
  input = std::to_string(initialValue);
}



template <class T>
Dialogue::Dialogue(T *t, void (T::*fun)(const std::string &), const std::string &text, Font::Truncate trunc)
  : stringFun(std::bind(fun, t, std::placeholders::_1))
{
  Init(text, true, false, trunc);
}



template <class T>
Dialogue::Dialogue(T *t, void (T::*fun)(), const std::string &text, Font::Truncate trunc)
  : voidFun(std::bind(fun, t))
{
  Init(text, true, false, trunc);
}



#endif
