//like line with thickness. but a graph with thickness
#include <SFML/Graphics.hpp>
#include "../../SFML_Functions/SFML_Functions.hpp"
#include <algorithm>
#include <iostream>
//https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader#23286000001269127
//for the 'channel' effect.
//each point is like a 'node' or 'circle'
//perpendicular
//cross
float (*cross)(const Point&, const Point&) = SFUTIL::cross<float>;
//is a graph...
//relationship between nodes... or etc.. indices..
typedef unsigned int NODEID;
typedef Point Vec2;
//each node has an edge goes out to point and an edge goes into point
//iterate by iterating graph...
//interior' holes would have to iterate the cyclic graph
//I CAN LIKELY USE THE GLM LIBRARY FOR NEXT TIME>>>

uint next(uint i, uint sz)
{
    return i + 1 == sz ? 0 : i + 1;
}
// monotonically increases with real angle, but doesn't need expensive trigonometry
//inline double pseudo_angle(const double dx, const double dy) {
//    const double p = dx / (std::abs(dx) + std::abs(dy));
//    return (dy > 0.0 ? 3.0 - p : 1.0 + p) / 4.0; // [0..1)
//}

float pseudoAngle(Point &P)
{
    float p = P.x / (fabsf(P.x) + fabsf(P.y));
    return (P.y > 0.f ? 3.f - p : 1.f + p);//0-4
}


//sort function of nodes...

struct Node
{
    Node(float x, float y, float w);
    //Node(const Node&) = delete;
    //Node(Node&&) = delete;
    Point pos;
    float r;
    std::vector<NODEID> edges;//connected to... (not OWNED)
};//ie a circle

std::vector<Node> nodes;
std::vector<Point> edgePoints;//resize

//sort
struct AngleSort
{
    bool operator()(NODEID a, NODEID b)
    {
        Point A = nodes[a].pos - base->pos;
        Point B = nodes[b].pos - base->pos;
        
        return pseudoAngle(A) < pseudoAngle(B);
    }
    Node *base;
};

Node::Node(float x, float y, float w) : pos(x,y), r(w)
{
    //ensure a gap?
    nodes.push_back(*this);
    edgePoints.resize(edgePoints.size()+2);
}
//orient by pseudo-angle...
//functions...
void connect(NODEID A, NODEID B)
{
    //is directed... line on the rhs (or lhs) only for a ccw or cw pattern of our 'edge' line
    nodes[A].edges.push_back(B);
    nodes[B].edges.push_back(A);
}

//visualizations...
std::vector<sf::CircleShape> nodeCircles;
void drawNodes()
{
    sf::CircleShape C;
    C.setFillColor(sf::Color(255,0,0,64));
    for(auto &n : nodes)
    {
        C.setRadius(n.r);
        C.setOrigin(n.r,n.r);
        C.setPosition(n.pos);
        nodeCircles.push_back(C);
    }
}

sf::VertexArray lines;
void drawEdges()
{
    lines.setPrimitiveType(sf::Lines);
    for(auto &n : nodes)
    {
        
        sf::Color color(rand()%255,rand()%255,rand()%255);
        //draw edge to and from...
        for(auto &e : n.edges)
        {
            //offset it to the right to see it work in action...
            Vec2 perp = nodes[e].pos - n.pos;
            perp = {-perp.y, perp.x};
            perp *= 0.1f;
            Point a = n.pos + perp;
            Point b = nodes[e].pos + perp;
            lines.append({a, sf::Color::Black});
            lines.append({b, sf::Color::Black});
        }
    }
}

std::vector<Point> points;

void genPoints()
{
    //for each node...
    for(auto &n : nodes)
    {
        //for each edge from this node... //sorted..
        for(uint i = 0, j = n.edges.size()-1; i < n.edges.size(); i++)
        {
            //is angle convex concave???
            //each edge?
            //go around ccw starting at the right...
            if(n.edges.size() == 1)//END point... 
            {//just the normal offsets...
                Vec2 V = nodes[i].pos - n.pos;
                Vec2 normal(-V.y, V.x);
                normal = SFUTIL::getUnitVec(normal);
                points.push_back(n.pos - normal * n.r);
                points.push_back(n.pos + normal * n.r);
            }
            else
            {
                //check angle with prior and after...
                Vec2 V_prior = nodes[j].pos - n.pos;
                Vec2 V       = nodes[i].pos - n.pos;
                Vec2 V_after = nodes[next(i,n.edges.size())].pos - n.pos;//next
                bool VPtoLeft = SFUTIL::cross<float>(V,V_prior) < 0.f;
                bool VAtoLeft = SFUTIL::cross<float>(V,V_after) < 0.f;

                if(VPtoLeft)
                //cross product i think
                //if to left is pos is 
            }
            
        }
    }
}//YUK

int main()
{
    srand(time(NULL));
    //make nodes...
    {
        Node(160,190,20);
        Node(180,140,15);
        Node(120, 60,40);
        Node(190, 60,10);
        Node( 60,130,20);
        Node( 40,100,10);
    }
    connect(0,1);
    connect(1,2);
    connect(2,3);
    connect(2,4);
    connect(4,5);
    drawNodes();
    drawEdges();

    std::cout<<lines.getVertexCount()<<std::endl;
    std::vector<NODEID> &edges = nodes[2].edges;
    AngleSort AS;
    AS.base = &nodes[2];
    std::cout<<"The edges were: ";
    for(auto &e : edges) std::cout<<e<<" ";
    std::cout<<std::endl;
    std::sort(edges.begin(),edges.end(),AS);
    std::cout<<"The edges now : ";
    for(auto &e : edges) std::cout<<e<<" ";
    std::cout<<std::endl;

    sf::RenderWindow window(sf::VideoMode(640,640),"Graph");
    sf::Event event;
    sf::View view;
    view.setCenter(125,125);
    view.setSize(250,-250);
    window.setView(view);

    while (window.isOpen())
    {
        window.clear(sf::Color(64,64,64));
        window.draw(lines);
        for(auto &c : nodeCircles) window.draw(c);
        window.display();

        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::Closed:
        case sf::Event::KeyPressed:
            window.close();
            break;
        default:
            break;
        }
    }
    return 0;
}