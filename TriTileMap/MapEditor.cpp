#include "MapEditor.hpp"

#include <fstream>
#include <iostream>

Editor::Editor()
{
    if(!m_font.loadFromFile("Soft Elegance.ttf")) throw;
    select();
}

void Editor::select()
{
    std::cout<<"Enter FileName to open or q to quit:"<<std::endl;
    std::cin>>fileName;

    if(fileName=="q") return;

    std::fstream file(fileName);

    if (!m_map.loadFromFile(fileName))
    {
        std::cout<<"Error opening "<<fileName<<std::endl;
        std::cout<<"Creating new file..."<<std::endl;
        std::cout<<"Enter width, height, scale"<<std::endl;
        unsigned int width, height, scale;
        std::cin>>width>>height>>scale;
        m_map.setup(width,height,scale);
    }

    //now create our renderwindow...
    sf::Vector2f min, max;
    m_map.getBounds(min, max);
    sf::Vector2f sz = m_map.getSize();
    //sz.y *= -1.f;
    m_view.setSize(sz.x, -1.f * sz.y);
    m_view.setCenter(sz/2.f);

    //ratio??
    unsigned int winWidth, winHeight;
    winWidth = sz.x;
    winHeight = sz.y;
    if(sz.x > sz.y)//wider...
    {
        winWidth = 900;
        winHeight = winWidth * (sz.y/sz.x);
    }
    else
    {
        winHeight = 900;
        winWidth = winHeight * (sz.x/sz.y);
    }
    m_window.create(sf::VideoMode(winWidth, winHeight),fileName);
    m_window.setView(m_view);

    edit();
}

void floodFill(GridMap &map, Coordinate C, const bool value)
{
    if(!map.isValidCoordinate(C) || map.checkTile(C) == value) return;
    map.toggleTile(C);
    if(C.isUp())
    {
        Coordinate A = C;
        A.a--;
        floodFill(map,A,value);
        Coordinate B = C;
        B.b--;
        floodFill(map,B,value);
        Coordinate C_ = C;
        C_.c++;
        floodFill(map,C_,value);
    }
    else
    {
        Coordinate A = C;
        A.a++;
        floodFill(map, A, value);
        Coordinate B = C;
        B.b++;
        floodFill(map, B, value);
        Coordinate C_ = C;
        C_.c--;
        floodFill(map, C_, value);
    }

    return;
}

void Editor::edit()
{
    sf::Text UI;
    UI.setCharacterSize(20);
    UI.setFillColor(sf::Color::Black);
    UI.setFont(m_font);
    UI.setPosition(10, 10);
    sf::Text UI2;
    UI2.setCharacterSize(20);
    UI2.setFillColor(sf::Color::Black);
    UI2.setFont(m_font);
    UI2.setPosition(10, 30);

    std::string mouseCoordString;
    sf::Vector2f mousePos;
    Coordinate C;

    TileShape selectorTile;
    selectorTile.setColor(sf::Color(160,160,160,128));
    selectorTile.scale(m_map.getScale(),m_map.getScale());

    bool drawGridLines = true;
    bool amInNewC = false;

    sf::Event event;
    while(m_window.isOpen())
    {
        m_window.clear(sf::Color::White);
        //m_window.setView(m_view);
        m_map.display(m_window, m_tileShape);
        selectorTile.draw(m_window);
        if(drawGridLines) m_map.drawGridLines(m_window);
        m_window.setView(m_window.getDefaultView());
        m_window.draw(UI);
        m_window.draw(UI2);
        m_window.display();
        m_window.setView(m_view);

        m_window.waitEvent(event);
        switch (event.type)
        {
            case sf::Event::KeyPressed:
            {
                if(event.key.code == sf::Keyboard::G)
                {
                    drawGridLines = !drawGridLines;
                    break;
                }
                if(event.key.code == sf::Keyboard::Q) return;
            }
            case sf::Event::Closed:
                m_window.close();
                break;
            case sf::Event::MouseMoved:
            {
                mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

                Coordinate X;
                m_map.convertPosToCoordinate(mousePos, X);
                //if(X.a != C.a && X.b != C.b && X.c != C.c && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                {
                    //std::cout<<"newTile"<<std::endl;
                    //amInNewC = true;
                }

                mouseCoordString.resize(0);
                mouseCoordString += "A: ";
                mouseCoordString += std::to_string(X.a);
                mouseCoordString += " B: ";
                mouseCoordString += std::to_string(X.b);
                mouseCoordString += " C: ";
                mouseCoordString += std::to_string(X.c);
                //C = X;

                //mouseCoordString.resize(0);
                mouseCoordString += "A: ";
                mouseCoordString += std::to_string(C.a);
                mouseCoordString += " B: ";
                mouseCoordString += std::to_string(C.b);
                mouseCoordString += " C: ";
                mouseCoordString += std::to_string(C.c);
                if(X.a != C.a && X.b != C.b && X.c != C.c) std::cout<<" crossed "<<std::endl;
                C=X;
                if (m_map.isValidCoordinate(C))
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

                selectorTile.moveToCoordinate(C,m_map.getScale());
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                if(event.mouseButton.button == sf::Mouse::Right)
                    floodFill(m_map,C,!m_map.checkTile(C));
                else //amInNewC = true;
                    m_map.toggleTile(C);
                break;
            }
            default:
                break;
        }
        if(amInNewC)
        {
            //m_map.toggleTile(C);
            //amInNewC = false;
        }
    }

    m_map.saveToFile(fileName);
    
    select();
}