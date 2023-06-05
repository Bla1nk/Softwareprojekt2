#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( )
{
    // Festlegen der zu verwendenden OpenGL Version
    ofGLFWWindowSettings settings;
    settings.setGLVersion(3, 0);
    settings.setSize(1280, 720);
    settings.windowMode = OF_WINDOW;
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
