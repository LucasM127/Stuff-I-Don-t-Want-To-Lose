#ifndef WORLD_HPP
#define WORLD_HPP

#include "Resources.hpp"
#include "Request.hpp"

#include "Entity.hpp"
#include "../TriTileMap/TriGridMap.hpp"
#include "SpatialHashMap.hpp"

enum GameState
{
    Playing,
    Paused,
    Won,
    Lost,
    OutOfBounds
};

typedef std::queue<LanderAction> RequestQueue;

class World
{
public:
    World(const Context &C);//a file to load???
    ~World(){}
    void processRequests(RequestQueue &requests);
    void update(float dt);
    void draw(sf::RenderTarget &target);
private:
/*
    GridMap m_gridMap;
    TileShape m_tileShape;
    
    sf::Sprite m_bgSprite;
    PointLight m_light;
    //hud stuff???
*/
//hmm
std::vector<sf::Vector2f> MapPolygon;
sf::VertexArray LightFromLander;
sf::RenderTexture rendertexture;
    const sf::Texture *m_bgTexture;
    sf::Sprite m_bgSprite;
    sf::VertexArray m_bgShape;

    SpatialHash m_bgCollidables;

    Lander m_lander;

    sf::Vector2f m_gravity;
    sf::Vector2f m_size;
    sf::View m_view;
};

#endif //WORLD_HPP