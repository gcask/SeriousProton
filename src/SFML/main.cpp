#include "SDL.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <SFML/System/FileInputStream.hpp>
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

	sf::RectangleShape rectangle;
	rectangle.setSize({ window.getSize().x / 4.f, window.getSize().y / 4.f });
	rectangle.setOrigin({ 0, rectangle.getSize().y / 2.f });
	rectangle.setPosition({ window.getSize().x / 2.f, window.getSize().y / 2.f });
	//rectangle.setOrigin(window.getSize().x / 2.f, window.getSize().y / 2.f);

	sf::CircleShape circle(50.f);
	auto semiRed = sf::Color::Red;
	semiRed.a = 127;
	circle.setFillColor(semiRed);
	circle.setOutlineThickness(2.f);
	circle.setPosition({ window.getSize().x - 100.f, 0.f });

	// Font+Text
	sf::FileInputStream ttfFile;
	SDL_assert(ttfFile.open("Roboto-Regular.ttf"));
	sf::Font roboto;
	SDL_assert(roboto.loadFromStream(ttfFile));
	sf::Font hachi;
	SDL_assert(ttfFile.open("HachiMaruPop-Regular.ttf"));
	SDL_assert(hachi.loadFromStream(ttfFile));

	std::array<sf::Text, 4> texts{
		sf::Text{"Hello,.projection SFML!#@+=/<>)(12345", roboto},
		sf::Text{"Hello,.projection SFML!#@+=/<>)(12345", roboto, 16},
		sf::Text{"Hello,.projection SFML!#@+=/<>)(12345", hachi},
		sf::Text{"Hello,.projection SFML!#@+=/<>)(12345\nThis is a new\tline", hachi, 16}
	};
	{
		auto currentY = roboto.getLineSpacing(30);
		texts[1].setPosition(0, currentY);
		currentY += roboto.getLineSpacing(16);
		texts[2].setPosition(0, currentY);
		currentY += hachi.getLineSpacing(30);
		texts[3].setPosition(0, currentY);
	}

	for (auto& text : texts)
	{
		text.setColor(sf::Color{ 0, 0, 0, 128 });
	}

	// draw a rectangle around each text.
	std::array<sf::RectangleShape, 4> fontBoxes;
	for (auto i = 0u; i < fontBoxes.size(); ++i)
	{
		auto bounds = texts[i].getLocalBounds();
		fontBoxes[i].setSize(sf::Vector2f{ bounds.width, bounds.height });
		fontBoxes[i].setFillColor(sf::Color::White);
		fontBoxes[i].setOutlineColor(sf::Color::Red);
		fontBoxes[i].setOutlineThickness(1.f);
	}

	{
		auto currentY = roboto.getLineSpacing(30);
		fontBoxes[1].setPosition(0, currentY);
		currentY += roboto.getLineSpacing(16);
		fontBoxes[2].setPosition(0, currentY);
		currentY += hachi.getLineSpacing(30);
		fontBoxes[3].setPosition(0, currentY);
	}

	SDL_Event event;
	bool keepGoing = true;
	auto angle = 0.f;
	auto scale = 1.f;
	auto scaleDelta = 0.1f;
	while (window.isOpen())
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				window.close();
		}
		window.clear(sf::Color::Black);
		for (auto i = 0; i < texts.size(); ++i)
		{
			window.draw(fontBoxes[i]);
			window.draw(texts[i]);
		}
		window.draw(rectangle);
		window.draw(vertices);
		window.draw(circle);
		angle += 1;
		scale += scaleDelta;
		if (scale > 2.f || scale < 1.f)
			scaleDelta *= -1;
		if (angle > 360.f)
			angle = 0.f;
		window.display();
	}
	SDL_Quit();
	return 0;
}