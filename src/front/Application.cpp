#include "Application.h"
//#include <chrono>


Application::Application()
{
	sf::VideoMode currentMode = sf::VideoMode::getDesktopMode();
	
	window = std::make_unique<sf::RenderWindow>(currentMode, "EEMapBuilder", sf::Style::Fullscreen);
	window->setFramerateLimit(60u);

	sf::Vector2u uWindowSize = window->getSize();
	sf::Vector2f windowSize(static_cast<float>(uWindowSize.x), static_cast<float>(uWindowSize.y));

	editor = std::make_unique<EditorCore>(windowSize);
}

Application::~Application()
{
}

void Application::run()
{
	while (window->isOpen())
	{
		//const auto start = std::chrono::high_resolution_clock::now();
		update();
		render();
		//const auto end = std::chrono::high_resolution_clock::now();
		//const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		//std::cout << "frame duration: " << duration.count() << "mikro sekuntti\n";

		//double frameTimeInSeconds = duration.count() / 1e6;

		
		//double fps = 1.0 / frameTimeInSeconds;
		//std::cout << "Frame Time: " << duration << " microseconds\nFPS: " << fps << " pls usko, k�yt�n kuitenki v��r�� kaavaa t�h� :clueless:\n\n";

	}
}

void Application::updateSFMLEvents()
{
	editor->resetButtonInfo();
	while (window->pollEvent(sfEvent)) {
		switch (sfEvent.type)
		{
		case sf::Event::Closed:
			window->close();
			return;
		
		case sf::Event::KeyPressed:
			if (sfEvent.key.code == sf::Keyboard::Escape)
			{
				window->close();
				return;
			}
			break;
		default:
			break;
		}
		editor->events(sfEvent);
	}
}

void Application::update()
{
	const float deltaTime = deltaClock.restart().asSeconds();
	updateSFMLEvents();
	editor->update();
}

void Application::render()
{
	window->clear(sf::Color::Black);
	//states.currentState()->render(*window);
	editor->draw(*window);
	window->display();
}
