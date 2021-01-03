#include "engine.h"
#include "SDL_clipboard.h"

string Clipboard::readClipboard()
{
    return SDL_GetClipboardText();
}

void Clipboard::setClipboard(string value)
{
    SDL_SetClipboardText(value.c_str());
}
