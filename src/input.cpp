#include "input.h"
#include "engine.h"

#include "SDL.h"

P<WindowManager> InputHandler::windowManager;
bool InputHandler::touch_screen = false;
sf::Transform InputHandler::mouse_transform;
PVector<InputEventHandler> InputHandler::input_event_handlers;
PVector<JoystickEventHandler> InputHandler::joystick_event_handlers;

bool InputHandler::keyboard_button_down[SDL_NUM_SCANCODES];
bool InputHandler::keyboard_button_pressed[SDL_NUM_SCANCODES];
bool InputHandler::keyboard_button_released[SDL_NUM_SCANCODES];
SDL_Keysym InputHandler::last_key_press;

sf::Vector2f InputHandler::mouse_position;
float InputHandler::mouse_wheel_delta;
bool InputHandler::mouse_button_down[MOUSE_BUTTON_COUNT];
bool InputHandler::mouse_button_pressed[MOUSE_BUTTON_COUNT];
bool InputHandler::mouse_button_released[MOUSE_BUTTON_COUNT];

float InputHandler::joystick_axis_pos[JOYSTICK_COUNT][sf::Joystick::AxisCount];
float InputHandler::joystick_axis_changed[JOYSTICK_COUNT][sf::Joystick::AxisCount];
bool InputHandler::joystick_button_down[JOYSTICK_COUNT][sf::Joystick::ButtonCount];
bool InputHandler::joystick_button_changed[JOYSTICK_COUNT][sf::Joystick::ButtonCount];

InputEventHandler::InputEventHandler()
{
    InputHandler::input_event_handlers.push_back(this);
}

InputEventHandler::~InputEventHandler()
{
}

JoystickEventHandler::JoystickEventHandler()
{
    InputHandler::joystick_event_handlers.push_back(this);
}

JoystickEventHandler::~JoystickEventHandler()
{
}

void InputHandler::initialize()
{
    memset(mouse_button_down, 0, sizeof(mouse_button_down));
    memset(keyboard_button_down, 0, sizeof(keyboard_button_down));
    memset(joystick_axis_pos, 0, sizeof(joystick_axis_pos));
#ifdef __ANDROID__
    touch_screen = true;
#endif
    last_key_press.scancode = SDL_SCANCODE_UNKNOWN;
}

void InputHandler::preEventsUpdate()
{
    if (!windowManager)
        windowManager = engine->getObject("windowManager");

    for(unsigned int n=0; n<SDL_NUM_SCANCODES; n++)
    {
        if (keyboard_button_pressed[n])
            keyboard_button_pressed[n] = false;
        else
            keyboard_button_released[n] = false;
    }
    for(unsigned int n=0; n<MOUSE_BUTTON_COUNT; n++)
    {
        if (mouse_button_pressed[n])
            mouse_button_pressed[n] = false;
        else
            mouse_button_released[n] = false;
    }
    for(unsigned int i=0; i<JOYSTICK_COUNT; i++)
    {
        for(unsigned int n=0; n<sf::Joystick::AxisCount; n++)
        {
            joystick_axis_changed[i][n] = false;
        }
        for(unsigned int n=0; n<sf::Joystick::ButtonCount; n++)
        {
            joystick_button_changed[i][n] = false;
        }
    }
    mouse_wheel_delta = 0;
}

void InputHandler::handleEvent(SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.scancode > SDL_SCANCODE_UNKNOWN && event.key.keysym.scancode < SDL_NUM_SCANCODES)
        {
            keyboard_button_down[event.key.keysym.scancode] = true;
            keyboard_button_pressed[event.key.keysym.scancode] = true;
        }
        last_key_press = event.key.keysym;
    }
	else if (event.type == SDL_KEYUP)
	{
        if (event.key.keysym.scancode > SDL_SCANCODE_UNKNOWN && event.key.keysym.scancode < SDL_NUM_SCANCODES)
        {
            keyboard_button_down[event.key.keysym.scancode] = false;
            keyboard_button_released[event.key.keysym.scancode] = true;
        }
	}
    else if (event.type == SDL_TEXTINPUT)
    {
        if (last_key_press.scancode != SDL_SCANCODE_UNKNOWN)
        {
            for (auto i = 0u; i < 32 && event.text.text[i]; ++i)
            {
                fireKeyEvent(last_key_press, event.text.text[i]);
            }
            
            last_key_press.scancode = SDL_SCANCODE_UNKNOWN;
        }
    }
    else if (event.type == SDL_MOUSEWHEEL)
        mouse_wheel_delta += event.wheel.y;
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        mouse_button_down[event.button.button] = true;
        mouse_button_pressed[event.button.button] = true;
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        mouse_button_down[event.button.button] = false;
        mouse_button_released[event.button.button] = true;
    }
    else if (event.type == SDL_JOYAXISMOTION)
    {
        // @note: SFML -> SDL
        // SFML maps the axis range within [-100,100].
        // SDL maps it in the range of a int16: [-32768,32767].
        constexpr std::tuple<float, float> sfmlAxis{ -100.f, 100.f };
        constexpr std::tuple<float, float> sdlAxis{ -32768.f, 32767.f };
        constexpr float sfmlRange = std::get<1>(sfmlAxis) - std::get<0>(sfmlAxis);
        constexpr float sdlRange = std::get<1>(sdlAxis) - std::get<0>(sdlAxis);
        const float position = std::get<0>(sfmlAxis) + (sfmlRange * event.jaxis.value) / sdlRange;
        static constexpr float scale_factor = 100.f / (100.f - joystick_axis_snap_to_0_range);

        float axis_pos = 0.f;
        
        if (position > joystick_axis_snap_to_0_range) {
            axis_pos = (position - joystick_axis_snap_to_0_range) * scale_factor;
        } else if (position < -joystick_axis_snap_to_0_range) {
            axis_pos = (position + joystick_axis_snap_to_0_range) * scale_factor;
        }

        // Clamp axis_pos within SFML range.
        axis_pos = std::min(std::max(-100.f, axis_pos), 100.f);

        if (joystick_axis_pos[event.jaxis.which][event.jaxis.axis] != axis_pos) {
            joystick_axis_changed[event.jaxis.which][event.jaxis.axis] = true;
        }
        
        joystick_axis_pos[event.jaxis.which][event.jaxis.axis] = axis_pos;
    }
    else if (event.type == SDL_JOYBUTTONDOWN)
    {
        joystick_button_down[event.jbutton.which][event.jbutton.button] = true;
        joystick_button_changed[event.jbutton.which][event.jbutton.button] = true;
    }
    else if (event.type == SDL_JOYBUTTONUP)
    {
        joystick_button_down[event.jbutton.which][event.jbutton.button] = false;
        joystick_button_changed[event.jbutton.which][event.jbutton.button] = true;
    }
    else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
    {
        for(unsigned int n=0; n<SDL_NUM_SCANCODES; n++)
        {
            keyboard_button_down[n] = false;
        }
    }
}

void InputHandler::postEventsUpdate()
{
    input_event_handlers.update();
    joystick_event_handlers.update();

    if (last_key_press.scancode != SDL_SCANCODE_UNKNOWN)
    {
        InputHandler::fireKeyEvent(last_key_press, -1);
        last_key_press.scancode = SDL_SCANCODE_UNKNOWN;
    }

#ifdef __ANDROID__
    if (sf::Touch::isDown(0))
    {
        mouse_position = realWindowPosToVirtual(sf::Touch::getPosition(0));
        if (!mouse_button_down[SDL_BUTTON_LEFT])
            mouse_button_pressed[SDL_BUTTON_LEFT] = true;
        mouse_button_down[SDL_BUTTON_LEFT] = true;
    }else{
        if (mouse_button_down[SDL_BUTTON_LEFT])
            mouse_button_released[SDL_BUTTON_LEFT] = true;
        mouse_button_down[SDL_BUTTON_LEFT] = false;
    }
#else
    mouse_position = realWindowPosToVirtual(sf::Mouse::getPosition(windowManager->window));
#endif
    mouse_position = mouse_transform.transformPoint(mouse_position);
    
    if (touch_screen)
    {
        bool any_button_down = false;
        for(unsigned int n=0; n<MOUSE_BUTTON_COUNT; n++)
            if (mouse_button_down[n] || mouse_button_released[n])
                any_button_down = true;
        if (!any_button_down)
        {
            mouse_position = sf::Vector2f(-1, -1);
        }
    }
    for(unsigned int i=0; i<JOYSTICK_COUNT; i++)
    {
        for(unsigned int n=0; n<sf::Joystick::AxisCount; n++)
        {
            if(joystick_axis_changed[i][n])
            {
                foreach(JoystickEventHandler, e, joystick_event_handlers)
                {
                    e->handleJoystickAxis(i, (sf::Joystick::Axis) n, joystick_axis_pos[i][n]);
                }
            }
        }
        for(unsigned int n=0; n<sf::Joystick::ButtonCount; n++)
        {
            if(joystick_button_changed[i][n])
            {
                foreach(JoystickEventHandler, e, joystick_event_handlers)
                {
                    e->handleJoystickButton(i, n, joystick_button_down[i][n]);
                }
            }
        }
    }
}

void InputHandler::setMousePos(sf::Vector2f position)
{
    if (!windowManager)
        windowManager = engine->getObject("windowManager");

    sf::Mouse::setPosition(virtualWindowPosToReal(position), windowManager->window);
    mouse_position = realWindowPosToVirtual(sf::Mouse::getPosition(windowManager->window));
}

void InputHandler::fireKeyEvent(const SDL_Keysym &key, int32_t unicode)
{
    foreach(InputEventHandler, e, input_event_handlers)
    {
        e->handleKeyPress(key, unicode);
    }
}

sf::Vector2f InputHandler::realWindowPosToVirtual(sf::Vector2i position)
{
    sf::FloatRect viewport = windowManager->window.getView().getViewport();
    sf::Vector2f pos = sf::Vector2f(position);
    
    pos.x -= viewport.left * float(windowManager->window.getSize().x);
    pos.y -= viewport.top * float(windowManager->window.getSize().y);
    pos.x *= float(windowManager->virtualSize.x) / float(windowManager->window.getSize().x) / viewport.width;
    pos.y *= float(windowManager->virtualSize.y) / float(windowManager->window.getSize().y) / viewport.height;
    return pos;
}

sf::Vector2i InputHandler::virtualWindowPosToReal(sf::Vector2f position)
{
    sf::FloatRect viewport = windowManager->window.getView().getViewport();

    position.x /= float(windowManager->virtualSize.x) / float(windowManager->window.getSize().x) / viewport.width;
    position.y /= float(windowManager->virtualSize.y) / float(windowManager->window.getSize().y) / viewport.height;
    
    position.x += viewport.left * float(windowManager->window.getSize().x);
    position.y += viewport.top * float(windowManager->window.getSize().y);
    return sf::Vector2i(position);
}
