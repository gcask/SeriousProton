#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "Updatable.h"
#include "windowManager.h"
class PlayerController: public virtual PObject
{
public:
    static const int buttonCount = 6;
    SDL_Scancode keyBind[4 + buttonCount];

    PlayerController(int nr);
    virtual ~PlayerController() {}
#if 0 // sdl compat: unused, disable.
    bool left() { return SDL_SCANCODE_isKeyPressed(keyBind[0]); }
    bool right() { return SDL_SCANCODE_isKeyPressed(keyBind[1]); }
    bool up() { return SDL_SCANCODE_isKeyPressed(keyBind[2]); }
    bool down() { return SDL_SCANCODE_isKeyPressed(keyBind[3]); }
    bool button(int idx) { return SDL_SCANCODE_isKeyPressed(keyBind[4 + idx]); }
#endif
};

#endif // PLAYERCONTROLLER_H
