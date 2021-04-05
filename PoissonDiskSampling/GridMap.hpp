#ifndef GRID2D_SFML_HPP
#define GRID2D_SFML_HPP

#include <SFML/System/Vector2.hpp>
#include <vector>
typedef unsigned int uint;

typedef float REAL;

typedef sf::Vector2<REAL> Vec2;

const uint INVALID_ID = -1;

struct Coord
{
    Coord(unsigned int a = 0, unsigned int b = 0) : i(a), j(b) {}
    Coord operator + (const Coord &C) {return Coord(i + C.i, j + C.j);}
    bool operator != (const Coord &C) {return i != C.i || j != C.j;}
    unsigned int i, j;
};

class GridMap
{
public:
    GridMap(uint width, uint height, REAL cellSz) : m_width(width), m_height(height), m_cellSz(cellSz)
    {
        m_data.resize(m_width * m_height, INVALID_ID);
    }
    bool isValid(const Coord C) const {return (C.i < m_width && C.j < m_height);}
    inline const uint width()  const { return m_width; }
    inline const uint height() const { return m_height; }
    inline Coord coord(const Vec2 p){ return Coord(p.x/m_cellSz, p.y/m_cellSz);}
    inline uint at(const Coord C) const {return C.i + C.j * m_width;}
    inline bool set(const uint x, const Coord C)
    {
        if(!isValid(C)) return false;
        m_data[at(C)] = x;
        return true;
    }
    inline bool get(uint &data, const Coord C)
    {
        if(!isValid(C)) return false;
        data = m_data[at(C)];
        return true;
    }
    const std::vector<uint> &data(){return m_data;}
    const REAL cellSz(){return m_cellSz;}
private:
    uint m_width, m_height;
    REAL m_cellSz;
    std::vector<uint> m_data;
};

#endif //GRID2D_SFML_HPP