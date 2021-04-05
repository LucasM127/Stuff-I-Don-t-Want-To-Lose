/*
Rules of the Game of Life
Life is played on a grid of square cells--like a chess board but extending infinitely in every direction. A cell can be live or dead. A live cell is shown by putting a marker on its square. A dead cell is shown by leaving the square empty. Each cell in the grid has a neighborhood consisting of the eight cells in every direction including diagonals.

To apply one step of the rules, we count the number of live neighbors for each cell. What happens next depends on this number.

    A dead cell with exactly three live neighbors becomes a live cell (birth).
    	
    A live cell with two or three live neighbors stays alive (survival).
    		
    In all other cases, a cell dies or remains dead (overcrowding or loneliness).
    			

Note: The number of live neighbors is always based on the cells before the rule was applied. In other words, we must first find all of the cells that change before changing any of them. Sounds like a job for a computer!
***/

#include <SFML/Graphics.hpp>
#include <vector>
#include <unistd.h>

struct Cell
{
    Cell():amAlive(false),willToggle(false){}
    operator bool() const {return amAlive;}
    void toggle() {if(willToggle) amAlive = !amAlive; willToggle = false;}
    bool amAlive;
    bool willToggle;
};


class Grid
{
public:
    Grid();
    const unsigned int getWidth() const { return m_width; }
    const unsigned int getHeight() const { return m_height; }
    const float getCellSize() const {return m_cellSize;}
    void iterate();
    void render(sf::RenderTarget &target);
    unsigned int countNeighbours(int i, int j);//unsigned int i, unsigned int j);
    void activateCell(unsigned int i, unsigned int j){Cell & c = getCell(i,j); c.willToggle = true; c.toggle();}
private:
    unsigned int m_width, m_height;
    float m_cellSize;
    std::vector<Cell> m_grid;
    Cell &getCell(unsigned int i, unsigned int j) {return m_grid[i + j * m_width];}

    std::vector<sf::Vertex> m_cellShapes;//just change the color...
    
    sf::RectangleShape m_cellShape;
};

Grid::Grid() : m_width(256), m_height(256), m_cellSize(4.f)
{
    m_cellShapes.resize(m_width * m_height * 6);
    for(uint j = 0; j < m_height; ++j)
    {
        for(uint i = 0; i < m_width; ++i)
        {
            uint cellID = i + j * m_width;
            m_cellShapes[6 * cellID + 0].position = {i * m_cellSize, j * m_cellSize};
            m_cellShapes[6 * cellID + 1].position = {(i+1) * m_cellSize, j * m_cellSize};
            m_cellShapes[6 * cellID + 2].position = {i * m_cellSize, (j+1) * m_cellSize};
            m_cellShapes[6 * cellID + 3].position = {(i+1) * m_cellSize, j * m_cellSize};
            m_cellShapes[6 * cellID + 4].position = {i * m_cellSize, (j+1) * m_cellSize};
            m_cellShapes[6 * cellID + 5].position = {(i+1) * m_cellSize, (j+1) * m_cellSize};
        }
    }
    for (auto &v : m_cellShapes) v.color = sf::Color::White;

    m_cellShape.setFillColor(sf::Color::Black);
    m_cellShape.setSize({m_cellSize,m_cellSize});
    m_grid.resize(m_width * m_height);

    //randomize
    
    int numCells = rand()%(m_width * m_height);
    for(int i = 0; i<numCells; i++)
    {
        int x = rand()%m_width;
        int y = rand()%m_height;
        getCell(x,y).amAlive = true;
        for(int x = 0; x<6; x++)
            m_cellShapes[6*(y * m_width)+x].color = sf::Color::Black;
    }
}

//what if made it just blit it out once... ?
void Grid::render(sf::RenderTarget &target)
{
    target.draw(&m_cellShapes[0],m_cellShapes.size(),sf::Triangles);
}
/*** vs..
void Grid::render(sf::RenderTarget &target)
{
    for(int j = 0; j < m_height; j++)
    {
        for(int i = 0; i < m_width; i++)
        {
            if(getCell(i,j))
            {
                m_cellShape.setPosition(m_cellSize * i, m_cellSize * j);
                target.draw(m_cellShape);
            }
        }
    }
}***/

void Grid::iterate()
{
    for(int i = 0; i<m_width; i++)
    {
        for(int j = 0; j<m_height; j++)
        {
            unsigned int neighbours = countNeighbours(i,j);
            Cell &curCell = getCell(i,j);
            if(curCell)//alive
            {
                if(neighbours < 2 || neighbours > 3)
                {
                    curCell.willToggle = true;
                //set to dead... white
                for (int x = 0; x < 6; x++)
                    m_cellShapes[6 * (i + j * m_width) + x].color = sf::Color::White;
                }
            }
            else
            {
                if(neighbours == 3)
                {
                    curCell.willToggle = true;
                for (int x = 0; x < 6; x++)
                    m_cellShapes[6 * (i + j * m_width) + x].color = sf::Color::Black;
                }
            }
        }
    }
    for(auto &cell : m_grid) cell.toggle();//toggles its id...
}

unsigned int Grid::countNeighbours(int i, int j)//(unsigned int i, unsigned int j)
{
    /*
    int topRow = j > 0 ? j - 1 : -1;
    int bottowRow = j < m_height - 1 ? j + 1 : -1;
    int leftCol = i > 0 ? i - 1 : -1;
    int rightCol = i < m_width - 1 ? i + 1 : -1;
    */
    unsigned int count = 0;

    for (int rowID = -1; rowID <= 1; rowID++)
    {
        int curRow = j + rowID;
        if(curRow < 0) continue;
        else if(curRow == m_width) break;
        for (int colID = -1; colID <= 1; colID++)
        {
            if(rowID == 0 && colID == 0) continue;
            int curCol = i + colID;
            if(curCol < 0) continue;
            else if(curCol == m_width) break;
            if(getCell(curCol, curRow)) count++;
        }
    }
    /**
    for (int y = j > 0 ? j - 1 : 0; y < m_height && y < j+2; y++)
    {
        for (int x = i > 0 ? i - 1 : 0; x < m_width && x < i+2; x++)
        {
            if (x == i && y == j)
                continue;
            if (getCell(x, y))
                count++;
        }
    }**/
    return count;
};

int main()
{
    srand(time(NULL));

    Grid grid;
    unsigned int width = grid.getWidth() * grid.getCellSize();
    unsigned int height = grid.getHeight() * grid.getCellSize();
    sf::RenderWindow window(sf::VideoMode(width,height),"Life");

    sf::Font font;
    if(!font.loadFromFile("Soft Elegance.ttf")) return -1;
    sf::Text countText;
    countText.setFillColor(sf::Color(255, 0, 0, 128));
    countText.setFont(font);
    countText.setPosition(0.f,0.f);
    countText.setCharacterSize(64);
    countText.setString("0");

    sf::Vector2i mousePos;
    bool inSetUpState = true;
    unsigned int i, j;
    int x = -1; int y = -1;

    sf::Event event;
    sf::Clock clock;//8fps
    sf::Time elapsed = sf::Time::Zero;
    float timer = 0.f;
    int frameCounter = 0;
    float frameTimer = 0;
    while (window.isOpen())
    {
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }

        window.clear(sf::Color::White);
        grid.render(window);
        window.draw(countText);
        window.display();
        
        elapsed = clock.restart();
        timer += elapsed.asSeconds();
        frameTimer += elapsed.asSeconds();
        frameCounter++;
        if(timer >= 0.125f)
        {   
            timer -= 0.125f;
            grid.iterate();
        }
        if(frameTimer >= 1.f)
        {
            frameTimer -= 1.f;
            countText.setString(std::to_string(frameCounter));
            frameCounter = 0;
        }
        usleep(1000);
    }
    /*
    while(window.isOpen())
    {
        window.clear(sf::Color::White);
        grid.render(window);
        window.draw(countText);
        window.display();

        window.waitEvent(event);
        switch (event.type)
        {
        case sf::Event::KeyPressed:
            if(event.key.code == sf::Keyboard::Space)
            {
                if(inSetUpState)
                {
                    inSetUpState = false;
                    break;
                }
                grid.iterate();
                break;
            }
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::MouseMoved:
        {/*
            if(inSetUpState && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                if(i != x && j != y)
                {
                    grid.activateCell(i, j);
                    x = i; y = j;
                }
            }* /
            mousePos = sf::Mouse::getPosition(window);
            i = mousePos.x / grid.getCellSize();
            j = mousePos.y / grid.getCellSize();
            countText.setString(std::to_string( grid.countNeighbours(i,j) ));
        }
            break;
        case sf::Event::MouseButtonPressed:
        {
            if(inSetUpState)
            {
                grid.activateCell(i,j);
            }
        }
        default:
            break;
        }
    }*/

    return 0;
}