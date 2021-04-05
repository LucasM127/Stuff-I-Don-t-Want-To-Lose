#include "SpatialGrid.hpp"

GridMap::GridMap(Vec2 sz, REAL cellSz)
    : m_sz(sz), m_cellSz(cellSz),
      m_gridSz( (uint(m_sz.x  / m_cellSz)), (uint(m_sz.y / m_cellSz)) )
{
    m_grid.resize(m_gridSz.x * m_gridSz.y, INVALID_ID);
}

bool GridMap::isValid(Vec2 p)
{
    return (p.x >= 0 && p.y >= 0 && p.x <= m_sz.x && p.y <= m_sz.y);
}

uint GridMap::at(uint i, uint j)
{
    return (i + m_gridSz.x * j);
}

uint GridMap::locate(Vec2 p)
{
    //bounds checking
    if(!isValid(p)) return INVALID_ID;
    uint i = uint(p.x / m_cellSz);
    uint j = uint(p.y / m_cellSz);
    return m_grid[i + m_gridSz.x * j];
}

uint GridMap::get(Vec2 p)
{
    return locate(p);
}

bool GridMap::set(Vec2 p, uint t)
{
    if(!isValid(p)) return false;
    uint i = uint(p.x / m_cellSz);
    uint j = uint(p.y / m_cellSz);
    m_grid[at(i,j)] = t;
    return true;
}
//BUGGY????
void GridMap::addNeighbours(Vec2 p, std::vector<uint> &neighbours)
{
    //if(!isValid(p)) return;
    uint i = uint(p.x / m_cellSz);
    uint j = uint(p.y / m_cellSz);
    for(uint x = i - 2; x < i + 3; ++x)
    //for (uint x = i > 1 ? i - 2 : i > 0 ? i - 1 : 0; x < (i == m_gridSz.x - 1 ? m_gridSz.x : i == m_gridSz.x - 2 ? m_gridSz.x : i + 3); ++x)
    //for (uint x = i > 0 ? i - 1 : 0; x < (i == m_gridSz.x - 1 ? m_gridSz.x : i + 2); ++x)
    {
        if(x >= m_gridSz.x) continue;
        //for (uint y = j > 1 ? j - 1 : j > 0 ? j - 1 : 0; y < (j == m_gridHeight - 1 ? m_gridHeight : j == m_gridHeight - 2 ? m_gridHeight : j + 3); ++y)
        //for (uint y = j > 0 ? j - 1 : 0; y < (j == m_gridSz.y - 1 ? m_gridSz.y : j + 2); ++y)
        for(uint y = j - 2; y < j + 3; ++y)
        {
            if(y >= m_gridSz.y) continue;
            if (x == i && y == j)
                continue;
//            if(i > m_gridSz.x || j > m_gridSz.y) continue;
            uint id = m_grid[at(i,j)];
            if(id != INVALID_ID)
                neighbours.push_back(id);
        }
    }
}