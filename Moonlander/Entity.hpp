#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include "Observer.hpp"
//#include "../LightRays/LightSource.hpp"
#include "../ParticleEngine/ConeSprayer.hpp"
#include "Object.hpp"
#include "../SFML_Functions/SFML_Functions.hpp"

class Entity : public sf::Transformable, public sf::Drawable
{
public:
    Entity(){}
    virtual ~Entity(){}
    virtual void update(float dt){}
protected:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override {}
};

class FuelTank : public Subject
{
public:
    FuelTank(float maxFuel):m_maxFuel(maxFuel),m_fuel(m_maxFuel){}
    float requestFuel(float request);
    const float getFuel() const {return m_fuel;}
    const float getMaxFuel() const {return m_maxFuel;}
    void refuel(){m_fuel = m_maxFuel;}
private:
    float m_maxFuel;
    float m_fuel;
};

class Lander;

class Engine : public Entity
{
public:
    Engine(/*float force,*/ Lander &lander);
    void update(float dt) override;
    void activate(bool on);
    void setMoment(float moment){m_momentForce = moment;}
    void setForce(float force);
private:
    sf::Vector2f m_force;
    Lander &r_lander;
    FuelTank &r_fuelTank;
    bool amActive;//state
    float m_momentForce;
    //ConeLight m_lightSource;
    ConeSprayer m_sprayer;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

enum LanderStateID
{
    Flying = 0,
    Landed,
    Crashed
};

enum EngineID
{
    Main = 0,
    Left,
    Right
};

//until we add fire lasers or something
//then maybe queue of a struct with
//the action enum id
//a pointer to something that can be used depending on the action... ie win32
struct LanderAction
{
    LanderAction(){}
    LanderAction(bool b, EngineID e) : state(b), engine(e){}
    bool state; //on or off
    /*EngineID*/int engine;
};

//reworking this to be states... and to eat my controller...
//void setEngines(bool,bool,bool)
//is the collidable object here... just by itself for the moment...
//getVertices() for collidable test...
class Lander : public Entity, public Subject
{
//friend class LanderState;
public:
    Lander();
    void applyForce(const sf::Vector2f &force);
    const sf::Vector2f &getVelocity(){return m_velocity;}
    void update(float dt) override;
    FuelTank &getFuelTank(){return m_fuelTank;}
    void toggleEngine(LanderAction& action);
    void reset();
    void land();//{amLanded = true;}
    void crash();
    const SFUTIL::AABBf &getAABB() const {return m_aabb;}
    const std::vector<Point> &getVertices() const {return m_transformedVertices;}
private:
    std::vector<sf::Vertex> m_vertices;//drawable
    std::vector<sf::Vector2f> m_transformedVertices;//collidable points
    SFUTIL::AABBf m_aabb;
    sf::Vector2f m_velocity;
    float m_rotation;
    FuelTank m_fuelTank;
    Engine m_engines[3];
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    bool amLanded, amCrashed;
};

#endif