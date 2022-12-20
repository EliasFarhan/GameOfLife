#include <SFML/Graphics.hpp>

constexpr int width = 512;
constexpr int height = 512;
constexpr std::size_t size = static_cast<std::size_t>(width) * height;

class Point
{
public:
    constexpr Point() = default;
    constexpr Point(int x, int y): x(x), y(y){}
    constexpr explicit Point(std::size_t index): x(static_cast<int>(index%width)), y(static_cast<int>(index/width))
    {
        
    }

    explicit constexpr operator std::size_t() const
    {
        return y * width + x;
    }

    constexpr Point operator+(const Point& p) const
    {
        return { p.x + x, p.y + y };
    }
    
    int x = 0;
    int y = 0;
};


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
        auto dt = clock.restart();
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for(std::size_t i = 0; i < size; ++i)
        {
            const auto pos = Point(i);
            const auto cell = previousMap[i];
            int count = 0;
            for(int dx = -1; dx <= 1; ++dx)
            {
                for(int dy = -1; dy <= 1; ++dy)
                {
                    if(dx == 0 && dy == 0)
                        continue;
                    auto delta = Point(dx, dy);
                    if (pos.x + dx < 0)
                    {
                        delta.x += width;
                    }
                    if (pos.y + dy < 0)
                    {
                        delta.y += height;
                    }
                    if (pos.x + dx >= width)
                    {
                        delta.x -= width;
                    }
                    if (pos.y + dy >= height)
                    {
                        delta.y -= height;
                    }
                    
                    const auto neighbor = pos + delta;
                    if(previousMap[static_cast<std::size_t>(neighbor)])
                    {
                        ++count;
                    }
                }
            }
            auto& nextCell = currentMap[static_cast<std::size_t>(pos)];

            if(!cell)
            {
                if(count == 3)
                {
                    nextCell = 1;
                }
                else
                {
                    nextCell = 0;
                }
            }
            else
            {
                if(count < 2 || count > 3)
                {
                    nextCell = 0;
                }
                else
                {
                    nextCell = 1;
                }
            }

            colors[i] = nextCell ? sf::Color::White : sf::Color::Black;

        }
        std::swap(currentMap, previousMap);

        image.create(width, height, reinterpret_cast<sf::Uint8*>(colors.data()));
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        
        window.clear(sf::Color::Black);
        window.draw(sprite);

        window.display();
    }

    return 0;
}