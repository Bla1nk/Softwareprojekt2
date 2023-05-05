#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	cout << "test";

    // Festlegen der zu verwendenden OpenGL Version
    ofGLFWWindowSettings settings;
    settings.setGLVersion(3, 0);
    settings.setSize(1024, 768);
    // settings.windowMode = OF_FULLSCREEN;
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
