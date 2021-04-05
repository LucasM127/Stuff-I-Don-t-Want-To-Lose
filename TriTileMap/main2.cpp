#include "TriGridMap.hpp"
#include <iostream>
#include <unistd.h>

class RotLine : public sf::Transformable, public sf::Drawable
{
public:
    RotLine();
    void update(float dt);
    sf::Vector2f getp1(){return getTransform().transformPoint(lineToTest[0].position);}
    sf::Vector2f getp2(){return getTransform().transformPoint(lineToTest[1].position);}
private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(lineToTest,2,sf::Lines,states);
    }
    sf::Vertex lineToTest[2];
};

RotLine::RotLine()
{
    lineToTest[0].position = {float(rand()%1000) / 100.f, float(rand()%1000) / 100.f};
	lineToTest[1].position = {float(rand()%1000) / 100.f, float(rand()%1000) / 100.f};
	lineToTest[0].color = sf::Color::Red;
	lineToTest[1].color = sf::Color::Black;
    setOrigin(5,5);
}

void RotLine::update(float dt)
{
    rotate(30.f * dt);
}

int main()
{
    srand(time(NULL));
	GridMap map(17,10);
    //map.setBounds(2.f,8.f,2.f,7.9f);
    //map.setBounds(2.f,10.f,2.f,10.f);//x is wrong!
    //offset ???? no need to use them...

	sf::Font font;
	std::string fontFile = "Soft Elegance.ttf";
	if (!font.loadFromFile(fontFile))
	{
		std::cout << "Unable to load " << fontFile << std::endl;
		return 0;
	}

	sf::Text UI;
	UI.setCharacterSize(20);
	UI.setFillColor(sf::Color::Black);
	UI.setFont(font);
	UI.setPosition(10, 10);
	sf::Text UI2;
	UI2.setCharacterSize(20);
	UI2.setFillColor(sf::Color::Black);
	UI2.setFont(font);
	UI2.setPosition(10, 30);

    TileShape tile;
    tile.setColor(sf::Color(140,150,220,128));
    TileShape mapTile;
    sf::Texture T;
    if(T.loadFromFile("3small.jpg"))
        mapTile.setTexture(T);
    else std::cout<<"Unable to load texture"<<std::endl;
    T.setRepeated(true);
    

	std::string mouseCoordString;
	sf::RenderWindow window(sf::VideoMode(640,640),"TriangleMap");
	sf::View view;
	view.setCenter(5.f,5.f);
	view.setSize(640,640);//12.f,-12.f);

    window.setView(view);

    Coordinate C;
    Coordinate dummy;
    sf::Vector2f mousePos;

    //rotate it around 5,5
    sf::Vertex lineToTest[2];
    lineToTest[0].position = {float(rand()%1000) / 100.f, float(rand()%1000) / 100.f};
	lineToTest[1].position = {float(rand()%1000) / 100.f, float(rand()%1000) / 100.f};
	lineToTest[0].color = sf::Color::Red;
	lineToTest[1].color = sf::Color::Black;
    RotLine line;

    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Red);
    rect.setOutlineThickness(5.f);

    sf::Vector2f mins, maxs;
    map.getBounds(mins,maxs);
    sf::Vector2f sz = map.getSize();

    rect.setSize(sz);
    rect.setPosition({0,0});
tile.setScale(2.f,2.f);
    sf::Event event;
    sf::Clock clock;
    float dt;
    while(window.isOpen())
	{
        dt = clock.restart().asSeconds();
		window.clear(sf::Color::White);
        //map.drawGridLines(window);
        map.display(window,mapTile);
        tile.draw(window);
        //window.draw(tile);
        window.draw(rect);
        //window.draw(line);
        //window.draw(lineToTest,2,sf::Lines);
        window.setView(window.getDefaultView());
		window.draw(UI);
		window.draw(UI2);
		window.setView(view);
        window.display();

        //window.waitEvent(event);
        while(window.pollEvent(event))
		{
        if(event.type == sf::Event::KeyPressed) window.close();
        if(event.type == sf::Event::Closed) window.close();
		if (event.type == sf::Event::MouseButtonPressed)
		{
            view.setSize(sz.x/2,sz.y/2);
			map.toggleTile(C);
        }//hmmmmmm....
		if (event.type == sf::Event::MouseMoved)
		{
			//window.setView(view);
			//update the UI
			mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            map.convertPosToCoordinate(mousePos,C);

			mouseCoordString.resize(0);
			mouseCoordString += "A: ";
			mouseCoordString += std::to_string(C.a);
			mouseCoordString += " B: ";
			mouseCoordString += std::to_string(C.b);
			mouseCoordString += " C: ";
			mouseCoordString += std::to_string(C.c);
            if(map.isValidCoordinate(C))
                mouseCoordString += " valid";
            else
                mouseCoordString += " invalid";

			UI.setString(mouseCoordString);

            mouseCoordString.resize(0);
            mouseCoordString += "X: ";
            mouseCoordString += std::to_string(mousePos.x);
            mouseCoordString += " Y: ";
            mouseCoordString += std::to_string(mousePos.y);
            UI2.setString(mouseCoordString);

			//set our appropiate triangle to position...
			tile.moveToCoordinate(C,2.f);
		}
        }
        


        usleep(10000);
	}

	return 0;
}
/* dump
        //line.update(dt);
        {
            sf::Transform T;
    T.translate(5.f,5.f);
    T.rotate(15.f*dt);//15.f);//90.f
    T.translate(-5.f,-5.f);
    lineToTest[0].position = T.transformPoint(lineToTest[0].position);
    lineToTest[1].position = T.transformPoint(lineToTest[1].position);
        }
        map.clear();
//        map.checkLine(line.getp1(),line.getp2(),dummy);
        //map.checkLine(lineToTest[0].position,lineToTest[1].position,dummy);
        map.getBounds(lineToTest,2);
*/