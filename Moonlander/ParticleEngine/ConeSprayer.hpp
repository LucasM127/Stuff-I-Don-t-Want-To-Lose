#ifndef CONESPRAYER_HPP
#define CONESPRAYER_HPP

#include <SFML/Graphics.hpp>
/*
#include <cmath>

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
  public:
    ParticleSystem(unsigned int count) : m_particles(count),
                                         m_vertices(sf::Points, count),
                                         m_lifetime(sf::seconds(3.f)),
                                         m_emitter(0.f, 0.f)
    {
    }

    void setEmitter(sf::Vector2f position)
    {
        m_emitter = position;
    }

    void update(sf::Time elapsed)
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            // update the particle lifetime
            Particle &p = m_particles[i];
            p.lifetime -= elapsed;

            // if the particle is dead, respawn it
            if (p.lifetime <= sf::Time::Zero)
                resetParticle(i);

            // update the position of the corresponding vertex
            m_vertices[i].position += p.velocity * elapsed.asSeconds();

            // update the alpha (transparency) of the particle according to its lifetime
            float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
        }
    }

  private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = NULL;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

  private:
    struct Particle
    {
        sf::Vector2f velocity;
        sf::Time lifetime;
    };

    void resetParticle(std::size_t index)
    {
        // give a random velocity and lifetime to the particle
        float angle = (std::rand() % 360) * 3.14f / 180.f;
        float speed = (std::rand() % 50) + 50.f;
        m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

        // reset the position of the corresponding vertex
        m_vertices[index].position = m_emitter;
    }

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Time m_lifetime;
    sf::Vector2f m_emitter;
};
*/


struct TriangleParticle
{
    TriangleParticle();
    sf::Vector2f velocity;
    float lifeSpan;
    float age;
    bool isActive;
    //sf::Transform transform;
};


class ConeSprayer : public sf::Drawable
{
public:
    ConeSprayer(float angle = 45.f);
    void update(float dt);
    void activate(bool active){amActive = active;}
    void setPos(sf::Vector2f pos){m_pos = pos;}
    void setAngle(float angle){m_angle = angle;}
    void setScale(float scale){m_scale = scale;}
private:
    sf::Vector2f m_pos;
    float m_angle;
    float m_scale;
    bool amActive;
    unsigned int m_maxLife;
    float m_sprayAngle;
    unsigned int m_numParticles;
    unsigned int m_numActive;
    std::vector<TriangleParticle> m_particles;
    std::vector<sf::Vertex> m_vertices;

    void updateParticle(float dt, unsigned int id);
    void resetParticle(unsigned int id);
    void killParticle(unsigned int id);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

#endif //CONESPRAYER_HPP