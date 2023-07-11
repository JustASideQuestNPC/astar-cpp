#include <SFML/Graphics.hpp>
#include "pixel_renderer.h"

int main()
{
    auto window = sf::RenderWindow{ { 720u, 480u }, "CMake SFML Project" };
    window.setFramerateLimit(60);
    
    sf::Clock clock;

    // canvas size is 180x120
    PixelRenderer canvas{{720, 480}, 4};

    sf::CircleShape pixelHex(30, 6);
    pixelHex.setOrigin(30, 30);
    pixelHex.setPosition(45, 60);
    pixelHex.setOutlineColor(sf::Color{0xFFFFFFFF});
    pixelHex.setOutlineThickness(3);
    pixelHex.setFillColor(sf::Color{0x00000000});

    sf::CircleShape smoothHex(30*4, 6);
    smoothHex.setOrigin(30*4, 30*4);
    smoothHex.setPosition(540, 240);
    smoothHex.setOutlineColor(sf::Color(0xFFFFFFFF));
    smoothHex.setOutlineThickness(14);
    smoothHex.setFillColor(sf::Color(0x00000000));
    
    float angle;
    float degreesPerSecond{45};

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        
        angle += clock.restart().asSeconds() * degreesPerSecond;

        window.clear();
        canvas.clear();
        
        pixelHex.setRotation(angle);
        canvas.draw(pixelHex);
        canvas.display(window);

        smoothHex.setRotation(angle);
        window.draw(smoothHex);
        window.display();
    }
}