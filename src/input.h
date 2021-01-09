#ifndef INPUT_H
#define INPUT_H
#include "SDL_events.h"
#include <SFML/Graphics/Transform.hpp>

#include "sfml2sdl.h"
#include "windowManager.h"
#include "Updatable.h"

class InputEventHandler: public virtual PObject
{
public:
    InputEventHandler();
    virtual ~InputEventHandler();
    
    virtual void handleKeyPress(SDL_Keysym key, int unicode) = 0;
protected:
private:
};

class JoystickEventHandler: public virtual PObject
{
public:
    JoystickEventHandler();
    virtual ~JoystickEventHandler();
    
    virtual void handleJoystickAxis(unsigned int joystick, sf::Joystick::Axis axis, float position) = 0;
    virtual void handleJoystickButton(unsigned int joystick, unsigned int button, bool state) = 0;
protected:
private:
};

class InputHandler
{
public:
    static constexpr size_t MOUSE_BUTTON_COUNT = 8; // arbitrary, SDL does not provide a button count.
    static constexpr size_t JOYSTICK_COUNT = 8; // sfml -> sdl: SDL allows querying at runtime, for any number of joysticks, sfml limits to 8.
    static bool touch_screen;
    static sf::Transform mouse_transform;

    static PVector<InputEventHandler> input_event_handlers;
    static PVector<JoystickEventHandler> joystick_event_handlers;

    static bool keyboardIsDown(const SDL_Scancode& key) { return keyboard_button_down[key]; }
    static bool keyboardIsPressed(const SDL_Scancode& key) { return keyboard_button_pressed[key]; }
    static bool keyboardIsReleased(const SDL_Scancode& key) { return !keyboard_button_pressed[key] && keyboard_button_released[key]; }

    static sf::Vector2f getMousePos() { return mouse_position; }
    static void setMousePos(sf::Vector2f position);
    static bool mouseIsDown(uint32_t button) { return mouse_button_down[button]; }
    static bool mouseIsPressed(uint32_t button) { return mouse_button_pressed[button]; }
    static bool mouseIsReleased(uint32_t button) { return !mouse_button_pressed[button] && mouse_button_released[button]; }
    static float getMouseWheelDelta() { return mouse_wheel_delta; }
    
    static sf::Vector2f getJoysticXYPos() { return sf::Vector2f(joystick_axis_pos[0][sf::Joystick::X], joystick_axis_pos[0][sf::Joystick::Y]); }
    static float        getJoysticZPos()  { return joystick_axis_pos[0][sf::Joystick::Z]; }
    static float        getJoysticRPos()  { return joystick_axis_pos[0][sf::Joystick::R]; }

    static float getJoysticAxisPos(unsigned int joystickId, sf::Joystick::Axis axis){ return joystick_axis_pos[joystickId][axis];}
    static bool getJoysticButtonState(unsigned int joystickId, unsigned int button){ return joystick_button_down[joystickId][button];}

private:
    static P<WindowManager> windowManager;

    static sf::Vector2f mouse_position;
    static float mouse_wheel_delta;
    static bool keyboard_button_down[SDL_NUM_SCANCODES];
    static bool keyboard_button_pressed[SDL_NUM_SCANCODES];
    static bool keyboard_button_released[SDL_NUM_SCANCODES];
    static SDL_Keysym last_key_press;

    static bool mouse_button_down[MOUSE_BUTTON_COUNT];
    static bool mouse_button_pressed[MOUSE_BUTTON_COUNT];
    static bool mouse_button_released[MOUSE_BUTTON_COUNT];
    
    static float joystick_axis_pos[JOYSTICK_COUNT][sf::Joystick::AxisCount];
    static float joystick_axis_changed[JOYSTICK_COUNT][sf::Joystick::AxisCount];
    static bool joystick_button_down[JOYSTICK_COUNT][sf::Joystick::ButtonCount];
    static bool joystick_button_changed[JOYSTICK_COUNT][sf::Joystick::ButtonCount];
    constexpr static float joystick_axis_snap_to_0_range = 5;

    static void initialize();
    static void preEventsUpdate();
    static void postEventsUpdate();
    static void handleEvent(SDL_Event& event);
    
    static void fireKeyEvent(const SDL_Keysym &key, int32_t unicode);
    static sf::Vector2f realWindowPosToVirtual(sf::Vector2i position);
    static sf::Vector2i virtualWindowPosToReal(sf::Vector2f position);

    friend class Engine;
};


#endif//INPUT_H
