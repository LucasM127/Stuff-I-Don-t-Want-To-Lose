#include "PolyLoader.hpp"

#include <fstream>
#include <iostream>

void PolyLoader::loadPoly(const std::string &filename, std::vector<sf::Vector2f> &P)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        std::cout<<"Unable to open "<<filename<<std::endl;
        return;
    }

    float f;
    sf::Vector2f v;
    file>>f;
    while(true)
    {
        v.x = f;
        file>>f;
        v.y = f;
        file>>f;
        P.push_back(v);
        if (file.peek() == EOF)//not sure why this way only. but whatever...
            break;
    }
}