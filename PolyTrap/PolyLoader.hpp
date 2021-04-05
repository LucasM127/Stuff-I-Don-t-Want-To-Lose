#ifndef PolyLoader_HPP
#define PolyLoader_HPP

#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>

class PolyLoader
{
public:
    PolyLoader(){}
    void loadPoly(const std::string& file, std::vector<sf::Vector2f>& P);
private:
};

#endif