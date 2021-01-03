#include "PlayerController.h"

PlayerController::PlayerController(int nr)
{
    switch(nr)
    {
    case 0:
    default:
        keyBind[0] = SDL_SCANCODE_LEFT;
        keyBind[1] = SDL_SCANCODE_RIGHT;
        keyBind[2] = SDL_SCANCODE_UP;
        keyBind[3] = SDL_SCANCODE_DOWN;
        
        keyBind[4] = SDL_SCANCODE_SPACE;
        keyBind[5] = SDL_SCANCODE_Z;
        keyBind[6] = SDL_SCANCODE_X;
        keyBind[7] = SDL_SCANCODE_C;
        keyBind[8] = SDL_SCANCODE_V;
        keyBind[9] = SDL_SCANCODE_B;
        break;
    case 1:
        keyBind[0] = SDL_SCANCODE_A;
        keyBind[1] = SDL_SCANCODE_D;
        keyBind[2] = SDL_SCANCODE_W;
        keyBind[3] = SDL_SCANCODE_S;
        
        keyBind[4] = SDL_SCANCODE_Q;
        keyBind[5] = SDL_SCANCODE_E;
        keyBind[6] = SDL_SCANCODE_R;
        keyBind[7] = SDL_SCANCODE_F;
        keyBind[8] = SDL_SCANCODE_T;
        keyBind[9] = SDL_SCANCODE_G;
        break;
    }
}
