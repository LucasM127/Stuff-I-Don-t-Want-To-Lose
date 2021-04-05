#version 410
//region of interest
//Bird of paradise
//center x 0.3750001200618655
//Center Y Coordinate 	-0.2166393884377127
//Size 	 0.000000000002 (square)
//Max Iterations Per Pixel 	1000000000
//double lf suffix = 1.0lf
uniform float viewWidth;
uniform float viewHeight;
uniform float cx;//center coordinates
uniform float cy;
uniform float maxIterations;
uniform float winWidth;
uniform float winHeight;

void main()
{
//    float width = 1440;720;
//    float height = 960;480;
	//vec2 textureCoordinates = gl_TexCoord[0].xy;
    //clamp!
    //x = -2 -> 1
    //y = -1 -> 1
    //float c_real = (gl_FragCoord.x / 1920.0) - 1.5;//(gl_FragCoord.x / 480.0) - 2.0;//x 0 -> 320 ==> -2 -> 1  //multiple of 3!
    //float c_imag = (gl_FragCoord.y - 480.0) / 1920.0;//960.0; //multiple of 2!
    double x_offset = cx - viewWidth/2.0;
    double y_offset = cy - viewHeight/2.0;
    double c_real = ( gl_FragCoord.x * double(viewWidth) )/double(winWidth) + double(x_offset);
    double c_imag = (gl_FragCoord.y * double(viewHeight) )/double(winHeight) + double(y_offset);

    //x = real component
    //y = imaginary component

    int iterations = 0;
    double z_real = 0.0;
    double z_imag = 0.0;

    do
    {
        //z^2 + c
        //(z_real + z_imag)(z_real + z_imag)
        //z_real * z_real + 2.0 * z_real * z_imag + (z_imag * z_imag)-> -1 
        double real = z_real * z_real - z_imag * z_imag + c_real;
        double imag = z_real * z_imag * 2.0 + c_imag;
        z_real = real;
        z_imag = imag;
        iterations++;
    }
    while (((z_imag * z_imag + z_real * z_real) < 4.0) && iterations < int(maxIterations));
    /*
    float n = float(iterations);
    float a = 0.1;
    float r = 0.5 * sin(a * n) + 0.5;//0.0;
    float g = 0.5 * sin(a * n + 2.094) + 0.5;
    float b = 0.5 * sin(a * n + 4.188) + 0.5;
    gl_FragColor = vec4(r,g,b,1.0);
    */
//more like want the log of it...

    float x = float(iterations) / maxIterations;
    
    //float x = log(float(iterations)) / log(maxIterations);
    gl_FragColor = vec4(x,x,x,1.0);
    
}