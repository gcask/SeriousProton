#include "SDL.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "GL/glew.h"
int main(int, char* [])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	sf::RenderWindow window;
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::ContextSettings context_settings(24, 8, false, 2, 0);
	window.create(sf::VideoMode(desktop.width / 2, desktop.height / 2, 32), "SFML/SDL Test", sf::Style::Default, context_settings);
	window.setFramerateLimit(30);
	sf::VertexArray vertices(sf::Triangles, 3);
	vertices[0].color = sf::Color::Red;
	vertices[1].color = sf::Color::Blue;
	vertices[2].color = sf::Color::Green;

	vertices[0].position = sf::Vector2f(10.0f, 10.f);
	vertices[1].position = sf::Vector2f(100.f, 10.f);
	vertices[2].position = sf::Vector2f(100.f, 100.f);

	//rectangle.setSize({ window.getSize().x / 4.f, window.getSize().y / 4.f });
	//rectangle.setOrigin(window.getSize().x / 2.f, window.getSize().y / 2.f);
	SDL_Event event;
	bool keepGoing = true;
	while (window.isOpen())
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				window.close();
		}
		window.clear(sf::Color::Black);
		window.draw(vertices);
		//window.draw(rectangle);
		window.display();
	}
	SDL_Quit();
	return 0;
}