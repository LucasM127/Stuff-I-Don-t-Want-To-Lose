#include <SFML/Graphics.hpp>
#include "../SFML_Functions/SFML_Functions.hpp"

typedef Point Vec2;

int main()
{
    sf::RenderWindow window(sf::VideoMode(800,600), "TestLine");

    sf::Font font;
    if(!font.loadFromFile("Soft Elegance.ttf")) return -1;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::Black);
    text.setPosition(10.f,10.f);

    sf::VertexArray line;
    line.setPrimitiveType(sf::Lines);

    Point l0(100.f,100.f);
    Point l1(300.f,200.f);
    line.append( { l0, sf::Color::Black});
    line.append( { l1, sf::Color::Black});

    sf::Event event;
    sf::Vector2f mousePos;
    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        window.draw(line);
        window.draw(text);
        window.display();

        window.waitEvent(event);
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::MouseMoved)
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            const Vec2 p = mousePos;
            const Vec2 L = l1 - l0;
            const Vec2 P = p - l0;
            float dot = SFUTIL::dot(L, L);
            float range = L.x * P.x + L.y * P.y;
            range /= dot;
            
            float cross = SFUTIL::cross(P, L);
            float length = sqrtf(dot);
            float distance = cross / length;

            std::string string = "Range: ";
            string.append( std::to_string(range));
            string.append("   and distance: ");
            string.append( std::to_string(distance));

            text.setString(string);
        }
    }
    
    return 0;
}