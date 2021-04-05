#include "ConeSprayer.hpp"
#include <unistd.h>

int main()
{
    sf::RenderWindow window(sf::VideoMode(640,640),"particles");
    sf::View view;
    view.setCenter(0.f,0.f);
    view.setSize(100.f,100.f);
    window.setView(view);

    sf::Transform T;
    //T.scale(5.f,5.f);

    sf::Clock clock;
    sf::Time time;

    //ParticleSystem PS(200);
    ConeSprayer PS(60.f);
    bool b = false;
    sf::Vector2f pos(-20.f,-20.f);
    PS.setPos(pos);
    PS.setAngle(45.f);
    PS.setScale(11.f);
    
    sf::Event event;
    while(window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                b = !b;
                PS.activate(b);
            }
        }

        //PS.update(time);
        PS.update(time.asSeconds());
        //T.rotate(20.f * time.asSeconds());

        window.clear(sf::Color(64, 64, 64));
        window.draw(PS);
        window.display();

        time = clock.getElapsedTime();
        clock.restart();
        usleep(1000);
    }
    return 0;   
}