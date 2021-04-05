#include "ConeSprayer.hpp"
#include <cmath>

//#include <iostream>
//it shoots them ALL out at the beginnng.. should only shoot out a few then them all
TriangleParticle::TriangleParticle() : lifeSpan(-1.f), age(0.f), isActive(false)
{
}

ConeSprayer::ConeSprayer(float angle)
{
    m_sprayAngle = (angle);
    m_numActive = 0;
    amActive = false;
    m_maxLife = 2;
    m_numParticles = 100;

    m_vertices.resize(m_numParticles * 3);
    m_particles.resize(m_numParticles);
}

void ConeSprayer::update(float dt)
{
    //std::cout<<m_numActive<<std::endl;
    if(!amActive && m_numActive == 0) return;
    for(unsigned int i = 0; i<m_numParticles; i++)
        updateParticle(dt,i);
}

void ConeSprayer::updateParticle(float dt, unsigned int id)
{
    TriangleParticle &P = m_particles[id];

    if(P.age < P.lifeSpan)
    {
        m_vertices[3 * id].position += P.velocity * dt;
        m_vertices[3 * id + 1].position += P.velocity * dt;
        m_vertices[3 * id + 2].position += P.velocity * dt;

        int r = (P.lifeSpan - P.age) * 255 / m_maxLife;
        int g = r / 4;
        m_vertices[3 * id].color.r = r;
        m_vertices[3 * id + 1].color.r = r;
        m_vertices[3 * id + 2].color.r = r;
        m_vertices[3 * id].color.g = g;
        m_vertices[3 * id + 1].color.g = g;
        m_vertices[3 * id + 2].color.g = g;

        P.age += dt;
    }
    else if(amActive)
    {
        //m_numActive++;
        resetParticle(id);
    }
    else
    {
        //m_numActive--;
        killParticle(id);
    }
}

void ConeSprayer::resetParticle(unsigned int id)
{
    TriangleParticle &P = m_particles[id];

    if(!P.isActive)
    {
        P.isActive = true;
        m_numActive++;
    }

    P.lifeSpan = float(rand()%(m_maxLife*20))/20.f;
    P.age = 0.f;//float(rand()%(m_maxLife*20))/20.f;//0.f;

    m_vertices[3 * id].position = {-.5f, 0.f};
    m_vertices[3 * id + 1].position = {0.0f, 1.0f};
    m_vertices[3 * id + 2].position = {0.5f, 0.f};

    sf::Transform T;
    //T.translate(m_pos);
    T.translate(m_pos);
    T.rotate(rand() % 360);
    T.scale(m_scale, m_scale);//2.f, 2.f);
    m_vertices[3 * id].position = T.transformPoint(m_vertices[3 * id].position);
    m_vertices[3 * id + 1].position = T.transformPoint(m_vertices[3 * id + 1].position);// + m_pos;
    m_vertices[3 * id + 2].position = T.transformPoint(m_vertices[3 * id + 2].position);// + m_pos;

    m_vertices[3 * id].color = m_vertices[3 * id + 1].color = m_vertices[3 * id + 2].color = sf::Color(255, 0, 0, 50);

    float angle = m_angle + ((float(rand() % 200) - 100.f) * m_sprayAngle / 200.f);
    float cosAng = cosf(angle * M_PI / 180.f);
    float sinAng = sinf(angle * M_PI / 180.f);
    float speed = (float(rand() % 100) / 100.f) * m_scale + 10.f * m_scale;

    m_particles[id].velocity = {speed * cosAng, speed * sinAng};
}

void ConeSprayer::killParticle(unsigned int id)
{
    if(m_particles[id].isActive)
    {
        m_particles[id].isActive = false;
        m_numActive--;
    }

    m_vertices[3 * id].color = sf::Color::Transparent;
    m_vertices[3 * id + 1].color = sf::Color::Transparent;
    m_vertices[3 * id + 2].color = sf::Color::Transparent;

    m_particles[id].lifeSpan = -1.f;
}

void ConeSprayer::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if(m_numActive == 0) return;
    //states.transform *= getTransform();
    target.draw(&m_vertices[0], m_vertices.size(), sf::Triangles);
}