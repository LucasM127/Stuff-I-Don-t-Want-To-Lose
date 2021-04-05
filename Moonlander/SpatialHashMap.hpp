#ifndef SPATIALHASHMAP
#define SPATIALHASHMAP

#include "../SFML_Functions/SFML_Functions.hpp"
#include <SFML/Graphics.hpp>
#include "Entity.hpp"
//#include "TriangleShape.hpp"
//#include "Object.hpp"
//#include "entity.hpp" // something base derived... with transformed vertices of polygon to test against and an aabb

//something to render AABBs
void convertAABB(const SFUTIL::AABBf aabb, sf::VertexArray &V);

struct Box
{
    SFUTIL::AABBf aabb;
    SFUTIL::LSegf lseg;
    bool wasTested;
};

struct Bucket
{
    std::vector<Box> boxes;
};
//only thing is duplicate aabbs could fail in multiple boxes...
//do the quadtree approach for those and put in a 'parent' box
//so have a min size max size ... should work fine...
class SpatialHash
{
public:
    SpatialHash(){}
    void resize(SFUTIL::AABBf bounds);
    //void insert(SFUTIL::AABBf aabb);
    void insert(sf::Vector2f &a, sf::Vector2f &b);//SFUTIL::LSegf &lseg);
    const SFUTIL::LSegf* testHit(Lander &lander);
    void render(sf::RenderTarget &target);
private:
    SFUTIL::AABBf m_bounds;
    float m_boxDim;
    uint m_width;
    uint m_height;
    uint m_maxDepth;//2^n n deep
    std::vector<Bucket> m_map;
    sf::VertexArray m_activeVAs;
    sf::VertexArray m_boxes;
};

#endif //SPATIALHASHMAP