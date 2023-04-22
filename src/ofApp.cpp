#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("weltbevoelkerung.csv"));
    mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("naturkatastrophen.csv"));
    mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("co2ausstoss.csv"));
    mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("kunstoffproduktion12.csv"));
    mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("kunstoffproduktion13.csv"));

    for (int i = 0; i < mLoadAndDisplaySVGData.size(); i++) {
        if (mLoadAndDisplaySVGData[i].diffy > highNumyFromClass) {
            highNumyFromClass = mLoadAndDisplaySVGData[i].diffy;
        }
    }
    std::cout << highNumyFromClass << std::endl;
}

//--------------------------------------------------------------
void ofApp::update()
{
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0,0,0);

    if (mDisplayGraph_1) mLoadAndDisplaySVGData[0].display(255, 255, 0, 0.1);
    if (mDisplayGraph_2) mLoadAndDisplaySVGData[1].display(0, 255, 255, 0.5);
    if (mDisplayGraph_3) mLoadAndDisplaySVGData[2].display(255, 255, 0, 1.0);
    if (mDisplayGraph_4) mLoadAndDisplaySVGData[3].display(0, 255, 255, 0.5);
    if (mDisplayGraph_5) mLoadAndDisplaySVGData[4].display(255, 255, 0, 0.2);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == '1') mDisplayGraph_1 = !mDisplayGraph_1;
    if (key == '2') mDisplayGraph_2 = !mDisplayGraph_2;
    if (key == '3') mDisplayGraph_3 = !mDisplayGraph_3;
    if (key == '4') mDisplayGraph_4 = !mDisplayGraph_4;
    if (key == '5') mDisplayGraph_5 = !mDisplayGraph_5;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
