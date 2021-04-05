#include "SpatialHashMap.hpp"
//#include <cmath>
#define LOG_DEBUG_STDOUT

using namespace SFUTIL;

void convertAABB(const SFUTIL::AABBf aabb, sf::VertexArray &V)
{
    V.setPrimitiveType(sf::Lines);
    sf::Vertex v;
    v.color = sf::Color::Red;
    v.position = {aabb.x_min, aabb.y_min};
    V.append(v);
    v.position = {aabb.x_min, aabb.y_max};
    V.append(v);
    v.position = {aabb.x_min, aabb.y_max};
    V.append(v);
    v.position = {aabb.x_max, aabb.y_max};
    V.append(v);
    v.position = {aabb.x_max, aabb.y_max};
    V.append(v);
    v.position = {aabb.x_max, aabb.y_min};
    V.append(v);
    v.position = {aabb.x_max, aabb.y_min};
    V.append(v);
    v.position = {aabb.x_min, aabb.y_min};
    V.append(v);
}

void SpatialHash::resize(AABBf bounds)
{
    m_bounds = bounds;
    m_boxDim = 500.f;//50.f;

    m_width = ceilf(bounds.width()/m_boxDim);
    m_height = ceilf(bounds.height()/m_boxDim);
    m_map.resize(m_width * m_height);

    m_boxes.setPrimitiveType(sf::Lines);
    for(uint i=0; i<m_width; ++i)
    {
        for(uint j = 0; j<m_height; ++j)
        {
            sf::Vertex V;
            V.color = sf::Color(128,128,128,128);
            V.position = {m_boxDim * i, m_boxDim * j};
            m_boxes.append(V);
            V.position = {m_boxDim * (i + 1), m_boxDim * j};
            m_boxes.append(V);
            V.position = {m_boxDim * i, m_boxDim * j};
            m_boxes.append(V);
            V.position = {m_boxDim * i, m_boxDim * (j + 1)};
            m_boxes.append(V);
        }
    }
}

void SpatialHash::insert(sf::Vector2f &a, sf::Vector2f &b)//LSegf &lseg)
{//ray Cast ???
    //find the AABB that it goes inside ???
    AABBf aabb(a,b);
    //from left inclusive but not right inclusive... -> use floor function
    uint lowerX = floorf(aabb.x_min / m_boxDim);
    uint upperX = floorf(aabb.x_max / m_boxDim);
    uint lowerY = floorf(aabb.y_min / m_boxDim);
    uint upperY = floorf(aabb.y_max / m_boxDim);

    if(upperY >= m_height) upperY = m_height - 1;
    if(upperX >= m_width) upperX = m_width - 1;

    Box B;
    B.aabb = aabb;
    B.lseg.a = a;
    B.lseg.b = b;
    B.wasTested = false;

    for (uint x = lowerX; x <= upperX; x++)
    {
        for(uint y = lowerY; y <= upperY; y++)
        {
            m_map[y * m_width + x].boxes.push_back(B);
        }
    }
}

void SpatialHash::render(sf::RenderTarget &target)
{
    target.draw(m_boxes);
    target.draw(m_activeVAs);
}
//insert the 'object' here
const LSegf* SpatialHash::testHit(Lander &lander)//SFUTIL::AABBf aabb)
{
//assert hits worldBoundsAABB
    const AABBf &aabb = lander.getAABB();
    if(!m_bounds.intersects(aabb)) return nullptr;

    m_activeVAs.clear();
    int lowerX = floorf(aabb.x_min / m_boxDim);
    int upperX = floorf(aabb.x_max / m_boxDim);
    int lowerY = floorf(aabb.y_min / m_boxDim);
    int upperY = floorf(aabb.y_max / m_boxDim);

    //assert bounds...
    if(upperY < 0 || upperX < 0) return nullptr;
    if(lowerY >= m_height || lowerX >= m_width) return nullptr;
    if(upperY >= m_height) upperY = m_height - 1;
    if(upperX >= m_width) upperX = m_width - 1;
    if(lowerX < 0) lowerX = 0;
    if(lowerY < 0) lowerY = 0;

    for (int x = lowerX; x <= upperX; x++)
    {
        for (int y = lowerY; y <= upperY; y++)
        {
            for (auto &box : m_map[y * m_width + x].boxes)
            {
                //if(box.wasTested) continue;
                //box.wasTested = true;//how to reset???
                convertAABB(box.aabb, m_activeVAs);

                if(box.aabb.intersects(aabb))
                {
                    if (SFUTIL::polygonIntersectLine(lander.getVertices(),box.lseg.a,box.lseg.b))
                    {
                        //draw the line in red
                        sf::Vertex V;
                        V.color = sf::Color::Green;
                        V.position = box.lseg.a;
                        m_activeVAs.append(V);
                        V.position = box.lseg.b;
                        m_activeVAs.append(V);
                        return &box.lseg;
                    }
                }
            }
        }
    }

    return nullptr;
}