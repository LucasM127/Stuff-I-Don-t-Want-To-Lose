#include <SFML/Graphics.hpp>
#include <vector>
#include <cassert>
#include <cmath>
#define LOG_DEBUG_STDOUT
#include "../Common/logger.hpp"

typedef sf::Vector2f Point;
typedef sf::Vector2f Vector;
typedef std::vector<Point> Points;

const int CLOSEDMESSAGE = -1;

sf::Color selectedColor = sf::Color::Green;
sf::Color vertexHandleColor = sf::Color(128,64,64,128);
sf::Color lineHandleColor = sf::Color(64, 64, 128, 128);

inline std::size_t next(std::size_t i, std::size_t sz)
{
    return (i < sz - 1 ? i + 1 : 0);
}

//functions
//add a point to a polygon
sf::Vertex pointToVertex(const Point &point)
{
    return sf::Vertex(point, sf::Color::Black);
};

class Observer
{
public:
    virtual ~Observer(){}
    virtual void onNotify(int id) = 0;
};

class Subject
{
public:
    void notify(int id){for(auto observer : m_observers) observer->onNotify(id);}
    void addObserver(Observer *observer) {m_observers.push_back(observer);}
private:
    std::vector<Observer*> m_observers;
};

class PolyEditor : public Subject //dynamic PolyEditor really a polyeditor?
{
public:
    PolyEditor():amClosed(false){}
    const bool isClosed() const {return amClosed;}
    const Points &getPoints() const {return m_points;}
    void addPoint(const Point &point);
    void insertPoint(const Point &point, unsigned int id);
    void movePoint(const Point &point, unsigned int id);
    void moveLine(const Vector &offset, unsigned int id);
    void toggleClosed();
private:
    Points m_points;
    bool amClosed;
    //edits the PolyEditor -> notify the watcher which id changes
};

void PolyEditor::toggleClosed()
{
    amClosed = !amClosed;
    notify(CLOSEDMESSAGE);
}

void PolyEditor::insertPoint(const Point &point, unsigned int id)
{
    m_points.insert(m_points.begin() + id, point);
    notify(id);
}

void PolyEditor::addPoint(const Point &point)
{
    m_points.push_back(point);
    notify(m_points.size()-1);
}

void PolyEditor::movePoint(const Point &point, unsigned int id)
{
    assert(id < m_points.size());

    m_points[id] = point;

    notify(id);
}

class sfPolyLine : public sf::Drawable, public Observer
{
public:
    sfPolyLine(PolyEditor &PolyEditor);
    void update(unsigned int id);
    void update();
private:
    void onNotify(int id) override;
    const PolyEditor &m_PolyEditor;
    const Points &m_points;
    std::vector<sf::Vertex> m_vertices;
    sf::Vertex m_closedLine[2];
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

sfPolyLine::sfPolyLine(PolyEditor &PolyEditor) : m_PolyEditor(PolyEditor), m_points(PolyEditor.getPoints())
{
    PolyEditor.addObserver(this);

    m_closedLine[0].color = m_closedLine[1].color = sf::Color(255,128,128);

    update();
}

void sfPolyLine::onNotify(int id)
{
    update();
//    update(id);
}

//watcher class... onNotify if the polyline changes
void sfPolyLine::update()
{
    m_vertices.clear();
    
    for (auto &point : m_points)
        m_vertices.emplace_back(sf::Vertex(point,sf::Color::Black));
    
    if (m_points.size() >= 2)
    {
        m_closedLine[0].position = m_points[0];
        m_closedLine[1].position = m_points.back();
    }
}

void sfPolyLine::update(unsigned int id)
{
    assert(id <= m_vertices.size());
    
    if(id < m_vertices.size())
    {
        m_vertices[id] = pointToVertex(m_points[id]);
        if(id == 0)
            m_closedLine[0].position = m_points[0];
    }
    else
    {
        if(id == 1)
            m_closedLine[0].position = m_points[0];
        m_vertices.push_back(pointToVertex(m_points[id]));
        m_closedLine[1].position = m_points[id];
    }
}

void sfPolyLine::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(&m_vertices[0],m_vertices.size(),sf::LineStrip,states);
    if(m_points.size()>2)
        target.draw(m_closedLine, 2, sf::LineStrip, states);
}

//notify when camera view changes for size change...
class Handler
{
public:
    Handler(PolyEditor &PolyEditor);
    virtual ~Handler(){}
    void draw(sf::RenderTarget &target);
    void select(const Point &selectionPoint);
    void handleMouseButtonPressed(const Point &mousePos);
    void handleMouseButtonReleased();
    void handleMouseMove(const Point &mousePos);

    bool amInserting;

protected:
    PolyEditor &r_PolyEditor;
    const Points &m_points;

    enum class SelectionType
    {
        NONE,
        VERTEX,
        LINE
    } m_selectionType;
    unsigned int m_selectedID;
    bool amSelected;
    bool amHovered;
    bool amClicked;
    
    Point m_lastMousePos;
    Point m_lastPos[2];

    sf::CircleShape m_vertexHandleShape;
    sf::RectangleShape m_lineHandleShape;
    float m_radius;
};

Handler::Handler(PolyEditor &PolyEditor) : r_PolyEditor(PolyEditor), m_points(r_PolyEditor.getPoints()), m_selectionType(SelectionType::NONE)
{
    m_radius = 15.f;
    m_vertexHandleShape.setFillColor(vertexHandleColor);
    m_vertexHandleShape.setRadius(m_radius);
    m_vertexHandleShape.setOrigin(m_radius, m_radius);

    m_lineHandleShape.setFillColor(lineHandleColor);
    m_lineHandleShape.setOrigin(-m_radius, m_radius);

    amSelected = false;
    amHovered = false;
    amClicked = false;
    amInserting = false;
}

void Handler::handleMouseButtonPressed(const Point &mousePos)
{//insert mode?
    amClicked = true;
    if(amInserting && amSelected && m_selectionType == SelectionType::LINE)
    {
        r_PolyEditor.insertPoint(mousePos, m_selectedID);
    }
    else if(amHovered)
    {
        amSelected = true;
        m_lastMousePos = mousePos;
        m_lastPos[0] = m_points[m_selectedID];
        if(m_selectionType == SelectionType::LINE)
            m_lastPos[1] = m_points[next(m_selectedID, m_points.size())];
    }
    else
    {
        amSelected = false;
        r_PolyEditor.addPoint(mousePos);
    }
    
}

void Handler::handleMouseButtonReleased()
{
    //amSelected = false;
    amClicked = false;
}

void Handler::handleMouseMove(const Point &mousePos)
{
    amHovered = false;
    if(amSelected && amClicked)
    {
        //move the point
        //if (m_selectionType == SelectionType::VERTEX
        Point newPos = m_lastPos[0] + mousePos - m_lastMousePos;
            r_PolyEditor.movePoint(newPos, m_selectedID);//move by an offset...
        if(m_selectionType == SelectionType::LINE)
        {
            m_lineHandleShape.setPosition(newPos);
            newPos = m_lastPos[1] + mousePos - m_lastMousePos;
            //unsigned int next = (m_selectedID == r_PolyEditor.getPoints().size() ? 0 : m_selectedID + 1);
            r_PolyEditor.movePoint(newPos, next(m_selectedID, m_points.size()));
        }
        else m_vertexHandleShape.setPosition(newPos);
    }
    else
//    if(!amSelected)
    {
    m_selectionType = SelectionType::NONE;
    amHovered = false;
    //loop throught the vertices...
    const Points &points = r_PolyEditor.getPoints();
    if(points.size() == 0) return;

    for (unsigned int i = 0; i<points.size(); i++)
    {
        Vector dist = points[i] - mousePos;
        if(dist.x*dist.x + dist.y*dist.y < m_radius*m_radius)
        {
            m_vertexHandleShape.setPosition(points[i]);
            m_selectionType = SelectionType::VERTEX;
            m_selectedID = i;
            amHovered = true;
            return;
        }
    }

    for (unsigned int i = 0; i < points.size(); i++)//overflow with unsigneds
    {
        unsigned int next = (i == points.size()-1 ? 0 : i + 1);
        //get length
        Vector dist = points[next] - points[i];
        float length = sqrtf(dist.x * dist.x + dist.y * dist.y);
        length -= 2*m_radius;
        //m_lineHandleShape.setSize({length - 2 * m_radius, 2 * m_radius});

        //get rotation???
        float angle = atan2f(dist.y, dist.x) * 180.f / M_PI;
        //m_lineHandleShape.setRotation(angle);
        //m_lineHandleShape.setPosition(points[i]);
        Vector transformedPt = points[i] - mousePos;
        sf::Transform transform;
        transform.rotate(180.f - angle);
        transformedPt = transform.transformPoint(transformedPt);
        //transformedPt = points[i] - transformedPt;
        transformedPt.x -= m_radius;
        if(transformedPt.x > 0 && transformedPt.x < length && transformedPt.y > -m_radius && transformedPt.y < m_radius)
        {
            m_lineHandleShape.setSize({length, 2 * m_radius});
            m_lineHandleShape.setRotation(angle);
            m_lineHandleShape.setPosition(points[i]);
            m_selectionType = SelectionType::LINE;
            m_selectedID = i;
            amHovered = true;
            return;
        }
    }
    }
}

void Handler::draw(sf::RenderTarget &target)
{
    /*
    const Points &points = r_polyline.getPoints();
    if(points.size()==0) return;

    for (unsigned int i = 0; i < points.size(); i++)
    {
        if(m_selectionType == SelectionType::VERTEX && m_selectedID == i)
            m_vertexHandleShape.setFillColor(selectedColor);
        else
            m_vertexHandleShape.setFillColor(vertexHandleColor);
        m_vertexHandleShape.setPosition(points[i]);
        target.draw(m_vertexHandleShape);
    }

    for (unsigned int i = 0; i<points.size()-1; i++)
    {
        if (m_selectionType == SelectionType::LINE && m_selectedID == i)
            m_lineHandleShape.setFillColor(selectedColor);
        else
            m_lineHandleShape.setFillColor(lineHandleColor);

        //get length
        Vector dist = points[i+1] - points[i];
        float length = sqrtf(dist.x*dist.x + dist.y*dist.y);
        m_lineHandleShape.setSize({length-2*m_radius, 2*m_radius});

        //get rotation???
        float angle = atan2f(dist.y,dist.x) * 180.f / M_PI;
        m_lineHandleShape.setRotation(angle);
        m_lineHandleShape.setPosition(points[i]);
        target.draw(m_lineHandleShape);
    }
    if(r_polyline.isClosed())
    {
        //do again...
        Vector dist = points[0] - points.back();
        float length = sqrtf(dist.x * dist.x + dist.y * dist.y);
        m_lineHandleShape.setSize({length - 2 * m_radius, 2 * m_radius});

        //get rotation???
        float angle = atan2f(dist.y, dist.x) * 180.f / M_PI;
        m_lineHandleShape.setRotation(angle);
        m_lineHandleShape.setPosition(points.back());
        target.draw(m_lineHandleShape);
    }*/
    if(m_selectionType == SelectionType::VERTEX && (amHovered || amSelected))
        target.draw(m_vertexHandleShape);
    else if(m_selectionType == SelectionType::LINE && (amHovered || amSelected))
        target.draw(m_lineHandleShape);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(640,640), "PolyEdit");

    sf::Event event;
    Point mousePos;

    PolyEditor PolyEditor;
    sfPolyLine sfpolyline(PolyEditor);
    Handler handler(PolyEditor);

    bool handlesActivated;
    
    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        window.draw(sfpolyline);
        handler.draw(window);
        window.display();
        
        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::KeyPressed:
        {
            if(event.key.code == sf::Keyboard::Space)
            {
                handler.amInserting = true;
                break;
            }
        }
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyReleased:
            handler.amInserting = false;
            break;
        case sf::Event::MouseMoved:
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            handler.handleMouseMove(mousePos);
//            handler.select(mousePos);
        }
            break;
        case sf::Event::MouseButtonPressed:
        {

            handler.handleMouseButtonPressed(mousePos);
            
            //pline.push_back(mousePos);
            //convertPolyLine(pline,drawnVertices);
        }
            break;
        case sf::Event::MouseButtonReleased:
            handler.handleMouseButtonReleased();
            break;
        default:
            break;
        }
    }
    

    return 0;
}