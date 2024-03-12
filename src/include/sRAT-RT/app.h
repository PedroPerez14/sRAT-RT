#ifndef APP_CLASS_H
#define APP_CLASS_H

#include <sRAT-RT/settings.h>

class App
{
    
private:
    Settings* settings;         // Create settings here
    Scene* scene;
    
    // change this to glfwwindow or however it is written lmao
    Window* window;

    //TODO: Any reference to the LUT3D tables here???

public:
    App();
    App(std::string settings);

    Run();
    //TODO: More stuff, add it when I come back for lunch
}

#endif