#include <GL/glew.h>
#include <SFML/Graphics.hpp>

//yippee
//now just use mouse for offsets!
//pan and zoom
//give a winwidth and height multiply it out
#include <iostream>
//resizing ... recentering... ???

//they had a scale factor!

//my zoom is not nice
//think of the transform matrix
//scale is 1:1 always
//center point is centered always
//then calculate width/height view width view height from scale and centerpoint
//viewWidth = width * scale
//viewHeight = height * scale
//resize the window? -> don't resize the scale

//shader sources
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

struct ViewMapper
{
    float scale;//scale!
    //winWidth = 900px, viewWidth = 3, (at beginning) -> scale = 1/300
    //winHeight = 600, viewHeight = 2, scale = 1/300
    //viewWidth = (1/300)(900) = 3;  cx = 1, given!
    
    float cx, cy;
    float viewWidth, viewHeight;//dx dy...
    //float scale;
    void zoom(float amt, const sf::Vector2f &mousePos)//give it a mouse to zoom -> towards?
    {//mouse before zoom, mouse after  + (before - after) ?????? need to watch that video
    //need the center point!
    //he had it so mouse pos was the same before and after the zoom
    //get the mouse position before mbx mby
    //zoom
    //recalculate mouse position
    //move the center by the offset
    std::cout<<cx<<" "<<cy<<std::endl;
        float sbx = viewWidth / winWidth;
        float sby = viewHeight / winHeight;
        float mbx = (mousePos.x - winWidth/2.f) * sbx + cx;//ma mouse after
        float mby = (mousePos.y - winHeight/2.f) * sby + cy;
        viewWidth *= amt;
        viewHeight *= amt;
        float sax = viewWidth / winWidth;
        float say = viewHeight / winHeight;
        float max = (mousePos.x - winWidth/2.f) * sax + cx;//ma mouse after
        float may = (mousePos.y - winHeight/2.f) * say + cy;
        //( gl_FragCoord.x * viewWidth )/winWidth + x_offset;
        //cx += (mbx - max);
        //cy -= (mby - may);//neg!
        std::cout<<mbx<<" "<<max<<" ... "<<mby<<" "<<may<<std::endl;
        std::cout<<cx<<" "<<cy<<std::endl<<std::endl;
    }
    float winWidth, winHeight;
    void pan(float win_dx, float win_dy)//move
    {
        float dx = win_dx * viewWidth / winWidth;
        float dy = win_dy * viewHeight / winHeight;
        cx += dx;
        cy += dy;
    }
    
    void resize(float width, float height)
    {
        //std::cout<<width<<" "<<winWidth<<std::endl;
        bool isWider = false;
        //assume winwidth/height changed, transfer viewwidth/height to adapt
        if(width != winWidth) isWider = true;
        winWidth = width;
        winHeight = height;
        float winAspect = winWidth / winHeight;
        float viewAspect = viewWidth / viewHeight;
        //if(winAspect > viewAspect) //goes 'wider'
        if(isWider)
        {
            viewWidth *= winAspect/viewAspect;//works great for right/left width adjusts
        }
        else
        {
            viewHeight *= viewAspect / winAspect;//works great for up/down
        }
        
    }
};

int main()
{
    unsigned int width = 1440;//720;
    unsigned int height = 960;//480;

    sf::ContextSettings Settings;
    Settings.depthBits = 24;
    Settings.stencilBits = 8;

    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot", 7u, Settings);

    ViewMapper VM;
    VM.winWidth = width;
    VM.winHeight = height;
    VM.viewWidth = 3.f;
    VM.viewHeight = 2.f;//aspect ratio the same (fixed)
    VM.cx = VM.cy = 0.f;
    float iterations = 256.f;

    GLuint programID = LoadShaders("",shader.frag);

    sf::Shader shader;
    shader.loadFromFile("shader.frag", sf::Shader::Fragment);
    shader.setUniform("viewWidth", VM.viewWidth);
    shader.setUniform("viewHeight", VM.viewHeight);//2.f);
    shader.setUniform("cx", VM.cx);
    shader.setUniform("cy", VM.cy);
    shader.setUniform("maxIterations", iterations);
    shader.setUniform("winWidth",  VM.winWidth);
    shader.setUniform("winHeight", VM.winHeight);

    bool amPanning = false;

    sf::Vector2f mousePos;
    sf::Vector2f panPos;

    sf::Vertex V[4] =
    {
        sf::Vertex({0.f,0.f}),
        sf::Vertex({0.f,float(height)}),
        sf::Vertex({float(width), 0.f}),
        sf::Vertex({float(width),float(height)})
    };//triangleStrip

    sf::Event event;
    sf::RenderStates states;
    sf::Clock clock;
    float elapsed;
    sf::Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    sf::Text elapsedText;
    elapsedText.setFont(font);

    states.shader = &shader;

    while (window.isOpen())
    {
        elapsed = clock.restart().asSeconds();
        elapsedText.setString(std::to_string(elapsed));

        window.clear(sf::Color::White);
        window.draw(V,4,sf::TriangleStrip,states);
        window.draw(elapsedText);
        window.display();

        while(window.pollEvent(event))// window.waitEvent(event);
        {
        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
        {
            if(event.key.code == sf::Keyboard::Space)
            {
                iterations += 256.f;
            }
            else iterations -= 256.f;
            shader.setUniform("maxIterations", iterations);
        } 
        case sf::Event::MouseMoved:
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        break;
        case sf::Event::MouseWheelMoved:
        {
            float amt;
            if(event.mouseWheel.delta > 0)
                amt = 0.8f;
            else
                amt = 1.25f;
            VM.zoom(amt, mousePos);
            shader.setUniform("viewWidth", VM.viewWidth);
            shader.setUniform("viewHeight", VM.viewHeight);//2.f);

            shader.setUniform("cx", VM.cx);
            shader.setUniform("cy", VM.cy);
        }
        break;
        case sf::Event::MouseButtonPressed:
        {
            amPanning = true;
            panPos = mousePos;
        }
        break;
        case sf::Event::MouseButtonReleased:
        case sf::Event::LostFocus:
        {
            amPanning = false;
        }
        break;
        case sf::Event::Resized:
        {
            VM.resize(event.size.width, event.size.height);
//            VM.winWidth = event.size.width;
//            VM.winHeight = event.size.height;
            shader.setUniform("winWidth",  VM.winWidth);
            shader.setUniform("winHeight", VM.winHeight);
            //gets 'squished' as not changing aspect view width/height
            
            shader.setUniform("viewWidth", VM.viewWidth);
            shader.setUniform("viewHeight", VM.viewHeight);
        }
        default:
            break;
        }
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))//amPanning)
        {
            sf::Vector2f delta = panPos - mousePos;
            VM.pan(delta.x, -delta.y);
            shader.setUniform("cx", VM.cx);
            shader.setUniform("cy", VM.cy);
            panPos = mousePos;
        }
        else(panPos = mousePos);
    }

    return 0;
}