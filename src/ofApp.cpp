#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    csv.load("weltbevoelkerung.csv");
    numRows = csv.getNumRows();
    numCols = csv.getRow(0).size();
    
    
    // Store the data in a vector
    for (int i = 1; i < numRows; i++) {
        ofxCsvRow row = csv.getRow(i);
        for (int j = 0; j < numCols-1; j++) {
            vecArray.push_back(glm::vec2(row.getFloat(j), row.getFloat(j+1)));
            //std::cout<<row.getFloat(j)<<" "<<row.getFloat(j+1)<<endl;
        }
    }
    float highNumx = 0;
    highNumy = 0;
    for (int i = 0; i < vecArray.size(); i++){
        if (vecArray[i].x > highNumx){
            highNumx = vecArray[i].x;
        }
        if (vecArray[i].y > highNumy){
            highNumy = vecArray[i].y;
        }
    }
    float lowNumx = highNumx;
    float lowNumy = highNumy;
    for (int i = 0; i < vecArray.size(); i++){
        if (vecArray[i].x < lowNumx){
            lowNumx = vecArray[i].x;
        }
        if (vecArray[i].y < lowNumy){
            lowNumy = vecArray[i].y;
        }
    }
    float diffx = highNumx - lowNumx;
    scaledx = ofGetWidth() / diffx;
    
    float diffy = highNumy - lowNumy;
    scaledy = ofGetHeight() / diffy;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0,0,0);
    ofSetColor(255, 255, 255);
        
    // Set the thickness of the line
    ofSetLineWidth(2);

    for (int i = 0; i < vecArray.size(); i++) {
        ofDrawEllipse(i * scaledx, ofGetHeight() - ((vecArray[i].y / highNumy) * ofGetHeight()), 10, 10);
        
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
