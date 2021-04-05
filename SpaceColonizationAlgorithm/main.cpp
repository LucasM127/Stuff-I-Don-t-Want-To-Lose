#include <SFML/Graphics.hpp>
#include <list>
#include <cmath>
#include <iostream>
#include "../../SFML_Functions/SFMLCameraController.hpp"

//controls
//use mouse to click and place points around the screen (attraction points)
//node start at 0,0
//use algorithm to gen new nodes, visualize branch lines as grow....
//kill points within kill distance
//iterate over all

float killDist2 = 30.f * 30.f;
float attractDist2 = std::numeric_limits<float>::max();//100.f * 100.f;
sf::CircleShape branchNode;

//to fix... want closest node pair for influence...
//loop through attractors, find closest node, pair node, normalized influence on list
//then through list and apply for each node and that's that
void iterate(std::vector<sf::CircleShape>& attractors, std::vector<sf::CircleShape>& branchNodes)
{
    if(attractors.size() == 0) return;
    std::vector<bool> killable(attractors.size(), false);
    std::list<std::pair<uint, sf::Vector2f> > influencers;

    for(uint i_a = 0; i_a < attractors.size(); ++i_a)
    {
        sf::Vector2f p_a = attractors[i_a].getPosition();

        float d_min2 = std::numeric_limits<float>::max();
        sf::Vector2f dir_min;
        uint branch_id_min = 0;
        for(uint i_b = 0; i_b < branchNodes.size(); ++i_b)
        {
            sf::Vector2f p_b = branchNodes[i_b].getPosition();
            sf::Vector2f delta = p_a - p_b;
            float dist2 = delta.x * delta.x + delta.y * delta.y;
            if(dist2 < killDist2) killable[i_a] = true;
            if(dist2 < d_min2)//only if significantly closer?
            {
                branch_id_min = i_b;
                dir_min = delta;
                d_min2 = dist2;
            }
        }
        //now know the closest branch node id...
        //normalize dir_min
        if(d_min2 > attractDist2) continue;
        float d_min = sqrtf(d_min2);
        dir_min.x /= d_min;
        dir_min.y /= d_min;
        influencers.emplace_back(branch_id_min, dir_min);
    }

    //now go through our influencers!
    if(influencers.size() == 0)
        attractors.clear();
    
    while (influencers.size())
    {
        uint activeBranchId = influencers.front().first;
        sf::Vector2f dir(0.f,0.f);
        for(auto it = influencers.begin(); it != influencers.end(); )
        {
            if(it->first == activeBranchId)
            {
                dir += it->second;
                it = influencers.erase(it);
            }
            else it++;
        }
        //normalize and apply our dir...
        //random factor?
        //offset by distance?

        sf::Vector2f randomVec(.1f,0.f);
        //rotate it random angle
        float randomAngle = rand()%360;
        sf::Transform T;
        T.rotate(randomAngle);
        randomVec = T.transformPoint(randomVec);
        dir += randomVec;
        

        float l = sqrtf(dir.x * dir.x + dir.y * dir.y);
        float diameter = branchNodes[activeBranchId].getRadius() * 2.f;
        dir.x *= diameter / l;
        dir.y *= diameter / l;
        sf::Vector2f newPos = branchNodes[activeBranchId].getPosition() + dir;
        branchNode.setPosition(newPos);
        branchNodes.push_back(branchNode);
    }

    //remove killable attractors
    for(uint i = 0; i < attractors.size(); ++i)
    {
        uint reverse_i = attractors.size() - i - 1;
        if(killable[reverse_i]) attractors.erase(attractors.begin() + reverse_i);
    }

    std::cout<<branchNodes.size()<<" branch nodes"<<std::endl;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800,800),"Space Colonization Visualized");
    sf::View view(window.getDefaultView());
    window.setView(view);
    CameraController camera(window,view);

    std::vector<sf::CircleShape> attractors;
    std::vector<sf::CircleShape> branchNodes;
    
    sf::CircleShape attractorCircle;
    attractorCircle.setFillColor(sf::Color::Green);
    attractorCircle.setRadius(5.f);
    attractorCircle.setOrigin(5.f,5.f);

    
    
    branchNode.setRadius(4.f);
    branchNode.setFillColor(sf::Color::Transparent);
    branchNode.setOrigin(4.f,4.f);
    branchNode.setOutlineColor(sf::Color(0,0,0,60));//::Black);
    branchNode.setOutlineThickness(1.f);
    branchNode.setPosition(400,800);
    branchNodes.push_back(branchNode);

    //place points
    for(uint i = 0; i < 1000; ++i)
    {
        sf::Vector2f pos;
        pos.x = rand()%window.getSize().x;
        pos.y = rand()%(window.getSize().y-50);
        sf::Vector2f o = pos;
        //a ^ shape Triangle...
        if(2*o.x < 800 - o.y) continue;
        if(o.y < 2 * o.x - 800) continue;
        
        attractorCircle.setPosition(pos);
        attractors.push_back(attractorCircle);
    }

    sf::Event event;
/*
    bool amPlacingPoints = true;
    
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    while(amPlacingPoints && window.isOpen())
    {
        window.clear(sf::Color::White);
        for(auto &c : attractors) window.draw(c);
        for(auto &c : branchNodes) window.draw(c);
        window.display();
        
        window.waitEvent(event);
        if(event.type == sf::Event::MouseButtonPressed)
        {
            attractorCircle.setPosition(mousePos);
            attractors.push_back(attractorCircle);
        }
        if(event.type == sf::Event::MouseMoved)
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
            amPlacingPoints = false;
    }
*/

    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        for(auto &c : attractors) window.draw(c);
        for(auto &c : branchNodes) window.draw(c);
        window.display();
        window.waitEvent(event);
        if(event.type == sf::Event::Closed)
            window.close();
        if(event.type == sf::Event::KeyPressed)
            iterate(attractors, branchNodes);
        camera.handleEvent(event);
    }
    
    return 0;
}