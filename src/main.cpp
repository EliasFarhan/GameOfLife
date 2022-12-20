#include <SFML/Graphics.hpp>

#include "optick.h"
#include "constant.h"





int main()
{
    sf::RenderWindow window(sf::VideoMode(width, height), "Game Of Life");
    //window.setFramerateLimit(5);
    sf::Clock clock;
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            if(std::rand() % 2 == 0)
            {
                cell = 1;
            }
        }
    }
    std::vector<unsigned char> currentMap(size, 0);
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    std::vector<sf::Color> colors(size);
    
    while (window.isOpen())
    {
        OPTICK_FRAME("GameFrame");
        auto dt = clock.restart();
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        {
            OPTICK_EVENT("CPU Computation");
            //#pragma omp parallel for
            for (int i = 0; i < size; ++i)
            {
                const auto pos = Point(i);
                auto& nextCell = currentMap[static_cast<std::size_t>(pos)];
                nextCell = Compute(previousMap, pos);

                colors[i] = nextCell ? sf::Color::Black : sf::Color::White;

            }
        }
        std::swap(currentMap, previousMap);
        {
            OPTICK_EVENT("Texture Creation");
            image.create(width, height, reinterpret_cast<sf::Uint8*>(colors.data()));
            texture.loadFromImage(image);
            sprite.setTexture(texture);
        }
        window.clear(sf::Color::Black);
        window.draw(sprite);

        window.display();
    }

    return 0;
}