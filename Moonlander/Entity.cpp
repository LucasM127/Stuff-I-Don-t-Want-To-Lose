#include "Entity.hpp"

#include <cmath>

float FuelTank::requestFuel(float x)
{
    float ratio;
    if (m_fuel >= x)
    {
        m_fuel -= x;
        ratio = 1.0f;
    }
    else if (!m_fuel)
        ratio = 0.f;
    else
    {
        ratio = m_fuel / x;
        m_fuel = 0.0f;
    }
    notify();
    return ratio;
}

Engine::Engine(Lander &lander)
    : r_lander(lander), r_fuelTank(lander.getFuelTank()), m_momentForce(0.f)
{
    /*
    m_vertices.resize(3);
    m_vertices[0].position = sf::Vector2f(0.f, -1.f);
    m_vertices[1].position = sf::Vector2f(-0.5f, 0.f);
    m_vertices[2].position = sf::Vector2f(0.5f, 0.f);
    m_vertices[0].color = sf::Color(255, 185, 0);
    m_vertices[1].color = sf::Color(0, 0, 0, 0);
    m_vertices[2].color = m_vertices[1].color;
    setOrigin(sf::Vector2f(0.f, -1.0f));
    */
    amActive = false;

    //m_sprayer.rotate(90.f);
}

void Engine::activate(bool on)
{
    amActive = on;
    m_sprayer.activate(on);
}

void Engine::setForce(float force)
{
    m_force.x = 0.f;
    m_force.y = -force;
    m_sprayer.setScale(20.f);//(force/50.f);//, force/50.f);//30 = 0.6
}

void Engine::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    //if (amActive)
        //states.transform *= getTransform();
        //target.draw(&m_vertices[0], m_vertices.size(), sf::Triangles, states);
//if(amActive)        target.draw(m_lightSource);
        target.draw(m_sprayer);//, states);
}

void Engine::update(float dt)
{
    m_sprayer.update(dt);
//has to stop generating if not active ...
    m_sprayer.setPos(getPosition() + r_lander.getPosition());
    m_sprayer.setAngle(getRotation() + r_lander.getRotation() + 90.f);

    if (!amActive)
        return;
    m_sprayer.update(dt);
    float fuelRatio = r_fuelTank.requestFuel(m_force.y * -1.f * dt);
    if (fuelRatio != 0)
    {
        float rotation = getRotation() + r_lander.getRotation();
        sf::Transform transform;
        transform.rotate(rotation);

        //push it by a factor of fuel
        sf::Vector2f force = transform * m_force * fuelRatio * dt;
        r_lander.applyForce(force);

        //rotate by momentForce
        r_lander.rotate(m_momentForce * dt);

        m_sprayer.setPos(getPosition() + r_lander.getPosition());
        m_sprayer.setAngle(rotation + 90.f);
    }
    else
        amActive = false;
//        activate(false);
}//

Lander::Lander() : m_velocity(0.f, 0.f), m_rotation(0.f), m_fuelTank(11200.f),
                   m_engines{{*this},{*this},{*this}},
                   amLanded(false), amCrashed(false)
{
    m_transformedVertices.resize(3);
    m_vertices.resize(3);
    m_vertices[0].position = sf::Vector2f(0.f, -1.f);//-0.866025403781f);
    m_vertices[1].position = sf::Vector2f(-0.5f, 0.f);
    m_vertices[2].position = sf::Vector2f(0.5f, 0.f);
    m_vertices[0].color = sf::Color(250, 80, 10);
    m_vertices[1].color = sf::Color(100, 0, 250);
    m_vertices[2].color = m_vertices[1].color;
    scale(50.f, 50.f); //50m big
    setOrigin(sf::Vector2f(0.f, -0.333f));

    m_engines[EngineID::Main].setForce(120.f);//60//30
    m_engines[EngineID::Main].setPosition(0.f, 0.f);
    m_engines[EngineID::Main].setRotation(0.f);//rotate(-0.f);
    //mainEngine.setScale(10.f,10.f);

    m_engines[EngineID::Right].setForce(32.f);//8
    m_engines[EngineID::Right].setPosition(-0.5f, 0.f);
    m_engines[EngineID::Right].setRotation(90.f);//rotate(90.f);
    m_engines[EngineID::Right].setMoment(-35.f); //45
    //leftEngine.scale(10.f,10.f);

    m_engines[EngineID::Left].setForce(32.f);//8
    m_engines[EngineID::Left].setPosition(0.5f, 0.f);
    m_engines[EngineID::Left].setRotation(-90.f);//rotate(-90.f);
    m_engines[EngineID::Left].setMoment(35.f);
    //rightEngine.scale(10.f,10.f);
}

void Lander::reset()
{
    m_velocity = {0.f, 0.f};
    m_fuelTank.refuel();
}

void Lander::applyForce(const sf::Vector2f &force)
{
    m_velocity += force;
}

void Lander::update(float dt)
{
    for(auto &engine : m_engines) engine.update(dt);//, map);

    if (amLanded || amCrashed)
        return;

    move(m_velocity * dt);
    //rotate(m_rotation*dt);

    //check collide?
    //Update transformed vertices... ie update AABB... and so on and so forth...
    //it owns or inherits from a collidable shape
    for(unsigned int i = 0; i<m_vertices.size(); i++)
    {
        m_transformedVertices[i] = getTransform().transformPoint(m_vertices[i].position);
    }
    m_aabb.resize(&m_transformedVertices[0],m_transformedVertices.size());
/*
    Coordinate C;
    for (auto &V : m_vertices)
    {
        auto pos = getTransform().transformPoint(V.position);
        map.convertPosToCoordinate(pos, C);
        if (map.checkTile(C))
        {
            if(fabsf(m_velocity.x) > 10.f || m_velocity.y > 10.f) crash();
            else land();
        }
    }
*/
    notify();
}

void Lander::toggleEngine(LanderAction &action)
{
    if(!amLanded && !amCrashed)
    m_engines[action.engine].activate(action.state);
}

void Lander::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    //if (!amLanded && !amCrashed)
        for (auto &engine : m_engines)
            target.draw(engine, states);
    target.draw(&m_vertices[0], m_vertices.size(), sf::Triangles, states);
}

void Lander::land()
{
    LanderAction action;
    //foreachenumvalue...
    for(int i = 0; i<3; i++)
    {
        action.engine = i;
        action.state = false;
        toggleEngine(action);
    }
    amLanded = true;

    setRotation(180.f);
}

void Lander::crash()
{
    LanderAction action;
    //foreachenumvalue...
    for (int i = 0; i < 3; i++)
    {
        action.engine = i;
        action.state = false;
        toggleEngine(action);
    }
    amCrashed = true;

    for(auto &V : m_vertices) V.color = sf::Color::Red;

    //setRotation(180.f);
}