#include "World.hpp"

#include <exception>

World::World(const Context &C)
{
    m_bgTexture = &C.texture.getTexture();
    
    m_gravity = {0.f, -40.f};//20

    sf::Vector2u windowSize = C.target.getSize();
    float viewRatio = (float)windowSize.x / (float)windowSize.y;

    rendertexture.create(windowSize.x / 16, windowSize.y / 16);

LightFromLander.setPrimitiveType(sf::TrianglesFan);
    //std::vector<sf::Vector2f> MapPolygon;
    std::vector<sf::Vector2f> Triangles;
    loadPoly("TerrainTrapData", Triangles);//"trapData", Triangles);
    if (!loadPoly("TerrainPoints", MapPolygon))//"mapData", MapPolygon))
    {
        //
    }
    for(auto &point : MapPolygon)
    {
        //point.y -= 250;
        point *= 7.5f;//50.f;//scale it
    }
    for(auto &point : Triangles)
    {
        //point.y -= 250;
        point *= 7.5f;//50.f;
    }
    //SFUTIL::convertPolygonOutline(MapPolygon, m_bgShape);
    for(auto &point : Triangles)
    {
        sf::Vertex V;
        V.position = point;
        //V.color = sf::Color(rand()%255,rand()%255,rand()%255);//sf::Color(255,160,80);
        V.texCoords = point;
        m_bgShape.append(V);
    }
    m_bgShape.setPrimitiveType(sf::Triangles);

    SFUTIL::AABBf mapAABB;
    mapAABB.resize(&MapPolygon[0],MapPolygon.size());
    m_bgCollidables.resize(mapAABB);
    //insert the line segments...
    for(uint i = 0, next = MapPolygon.size()-1; i<MapPolygon.size(); ++i)
    {
        m_bgCollidables.insert(MapPolygon[i], MapPolygon[next]);
        next = i;
    }

    m_size.x = mapAABB.width();
    m_size.y = mapAABB.height();

    float mapRatio = m_size.x / m_size.y;

    //keep the ratios... proportional... ... have banding above or below, or just show a part... (camera)
    //for now do banding... fit it all in the window...
    sf::Vector2f viewSz;
    if(viewRatio > mapRatio)//(mapRatio > viewRatio)//the map is wider than the view, banding on top and bottom...
    {
        viewSz = {m_size.x, m_size.x / viewRatio};
    }
    else
    {
        viewSz = {m_size.y * viewRatio, m_size.y};
    }
    //viewSz.y *= -1.f;
    m_view.setSize(viewSz.x/2.f, -viewSz.y/2.f);
    m_view.zoom(3.f);
    m_view.setCenter(m_size/2.f);

    m_bgSprite.setTexture(C.bgTexture.getTexture());
    sf::Vector2u bgSz = C.bgTexture.getTexture().getSize();//sf::Vector2u
    //scale to fit view...
    sf::Vector2f bgScaleFactor(viewSz.x / (float)bgSz.x , viewSz.y / (float)bgSz.y);
    m_bgSprite.scale(bgScaleFactor);
    m_bgSprite.setPosition((m_size - viewSz) / 2.f);

    m_lander.setPosition(m_size.x/2,m_size.y - 50.f);
    m_lander.rotate(180.f);
    m_lander.scale(2.f,2.f);

    m_view.setCenter(m_lander.getPosition());
}

void World::update(float dt)
{
    m_view.setCenter(m_lander.getPosition());

    m_lander.applyForce(m_gravity*dt);
    m_lander.update(dt);
    /*
    //LOG("TEST");
    //update LightFromLander
    sf::Vector2f landerPos = m_lander.getPosition();//{1200,500};//m_lander.getPosition();
    //m_lander.getRotation();
    //sort around that position... increasing angle...
    std::vector<float> atans;
    for(uint i = 0; i<MapPolygon.size(); i++)
    {
        Point A = MapPolygon[i] - landerPos;
        atans.push_back(atan2(A.y,A.x));
        //LOG(atans.back());
    }

    auto sortByAngle = [&](const uint a, const uint b) -> bool
    {
        return atans[a] < atans[b];
    };
//LOG("TEST");
    //sort our vertices... by angle
    std::vector<uint> pindices;
    
    for (uint i = 0; i < MapPolygon.size(); i++)
    {
        pindices.push_back(i);
//        pindices[i] = i;
    }
    //std::vector<Point> points = MapPolygon;
    
    try{std::sort(pindices.begin(),pindices.end(),sortByAngle);}
    catch(std::exception &e){LOG(e.what()); throw;}

    LightFromLander.clear();
    sf::Vertex V;
    V.color = sf::Color(255,255,255,128);
    V.position = landerPos;
    LightFromLander.append(V);
    V.color = sf::Color(255,255,0,128);
    srand(10);
    for(uint i = 0; i< MapPolygon.size(); ++i)//auto &p : points)//create a triangle fan
    {
        int r = rand() % 255;
        int g = rand() % 255;
        int b = rand() % 255;
        V.color = sf::Color(r,g,b,128);
        V.position = MapPolygon[pindices[i]];//MapPolygon[i];
        LightFromLander.append(V);
    }
    LightFromLander.append(LightFromLander[1]);
    */
    //check for status update from lander???
    //check collision...
    const SFUTIL::LSegf * hitSeg = nullptr;
    if(hitSeg = m_bgCollidables.testHit(m_lander))
    {//notify of a hit.... object notifyHit(object*) or something like that?
        sf::Vector2f vel = m_lander.getVelocity();
        if (fabsf(vel.x) > 10.f || vel.y > 15.f)
            m_lander.crash();
//        else if(m_lander.getRotation() 
        else
        {
            //slope?
            float m = (hitSeg->b.y - hitSeg->a.y) / (hitSeg->b.x - hitSeg->a.x);
            m = fabs(m);
            if(m < .1f)//(hitSeg->a.y == hitSeg->b.y)
                m_lander.land();
            else
                m_lander.crash();
        }
    }
}

void World::processRequests(RequestQueue &requests)
{
    if(requests.empty()) return;

    while(!requests.empty())
    {
        m_lander.toggleEngine(requests.front());
        requests.pop();
    }
}

void World::draw(sf::RenderTarget &target)
{
    sf::RenderStates state;
    state.texture = m_bgTexture;
/*
    rendertexture.clear(sf::Color::Black);
    rendertexture.setView(m_view);
    rendertexture.draw(m_bgSprite);
    rendertexture.draw(m_bgShape);//, state);
    m_bgCollidables.render(rendertexture);
    rendertexture.draw(LightFromLander);
    rendertexture.draw(m_lander);

    sf::Sprite sprite;
    sprite.setTexture(rendertexture.getTexture());
    //sprite.scale(1.f,-1.f);
    sprite.scale(16.f,-16.f);
    sprite.setPosition(0, 864);
    target.draw(sprite);
*/    

    target.setView(m_view);
    target.draw(m_bgSprite);
    target.draw(m_bgShape, state);
    m_bgCollidables.render(target);
    target.draw(LightFromLander);
    target.draw(m_lander);
}