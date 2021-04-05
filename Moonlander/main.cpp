#include "World.hpp"
#include <unistd.h>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1536,864),"moonlander");//,sf::Style::Fullscreen);//sf::VideoMode(1280, 720), "moonlander");

    Font font("Soft Elegance.ttf");
    Texture texture("3small2.jpg");
    Texture bgTexture("bg.jpg");
    Context C(font,texture,bgTexture,window);
    World W(C);

    sf::Event event;
    sf::Clock clock;
    sf::Time elapsed;

    float fpsUpdateTimer = 0.f;
    std::string fpsString;
    sf::Text fpsText;
    fpsText.setFont(font.getFont());
    fpsText.setCharacterSize(20);
    fpsText.setPosition(5.f,5.f);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setString("HELLO");

    RequestQueue actionQueue;

    while(window.isOpen())
    {
        elapsed = clock.restart();
        fpsUpdateTimer += elapsed.asSeconds();
        if (fpsUpdateTimer >= 1.0)
        {
            fpsString = std::to_string(1.0 / elapsed.asSeconds());
            fpsText.setString(fpsString);
            fpsUpdateTimer -= 1.0;
        }

        window.clear(sf::Color::Black);
        W.draw(window);
        window.setView(window.getDefaultView());
        window.draw(fpsText);
        window.display();

        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::W:
                        actionQueue.emplace(true,EngineID::Main);
                        break;
                    case sf::Keyboard::A:
                        actionQueue.emplace(true,EngineID::Left);
                        break;
                    case sf::Keyboard::D:
                        actionQueue.emplace(true,EngineID::Right);
                        break;
                    default:
                        break;
                }
            }
            if(event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::W:
                    actionQueue.emplace(false, EngineID::Main);
                    break;
                case sf::Keyboard::A:
                    actionQueue.emplace(false, EngineID::Left);
                    break;
                case sf::Keyboard::D:
                    actionQueue.emplace(false, EngineID::Right);
                    break;
                default:
                    break;
                }
            }
        }
        W.processRequests(actionQueue);
        W.update(elapsed.asSeconds());

        //usleep(10000);
    }
}
