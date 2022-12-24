#include <random>
#include <SFML/Graphics.hpp>

#include "optick.h"
#include "constant.h"
#include "conway.h"


int main()
{
    sf::RenderWindow window(sf::VideoMode(width, height), "Game Of Life");
    //window.setFramerateLimit(5);
    sf::Clock clock;
    std::vector<unsigned char> previousMap(size, 0);
    {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> distrib(0, 1);

        std::srand(std::time(nullptr));
        for (auto& cell : previousMap)
        {
            cell = distrib(gen);
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
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        {
            OPTICK_EVENT("CPU Computation");
            //#pragma omp parallel for
            for (int i = 0; i < static_cast<int>(size); i++)
            {
                const auto pos = Point(static_cast<std::size_t>(i));
                auto& nextCell = currentMap[static_cast<std::size_t>(pos)];
                nextCell = Compute2(previousMap, pos);
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
