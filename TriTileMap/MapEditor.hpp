#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

#include "TriGridMap.hpp"

class Editor
{
public:
    Editor();
    void select();
    void edit();
    ~Editor(){}
private:
    GridMap m_map;
    TileShape m_tileShape;
    sf::RenderWindow m_window;
    sf::View m_view;
    sf::Font m_font;
    std::string fileName;
};

#endif //MAPEDITOR_HPP