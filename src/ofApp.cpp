#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("weltbevoelkerung.csv"));
    mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("naturkatastrophen.csv"));
}

//--------------------------------------------------------------
void ofApp::update()
{
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0,0,0);

    if (mDisplayGraph_1) mLoadAndDisplaySVGData[0].display(255, 255, 0);
    if (mDisplayGraph_2) mLoadAndDisplaySVGData[1].display(0, 255, 255);
}

//--------------------------------------------------------------
void ofApp::loadAndProcessCsvFiles(const std::vector<std::string>& fileNames)
{
    for (const auto& fileName : fileNames) 
    {
        ofxCsv csv;
        
        bool loaded = csv.load(fileName,",");
        if (!loaded) {
            ofLogError() << "Csv-Datei " << fileName << " konnte nicht geladen werden";
            continue;
        }
        
        for (int row = 0; row < csv.getNumRows(); row++) {
            for (int col = 0; col < csv.getNumCols(row); col++) {
                ofLog() << "Wert in Datei " << fileName << ", Zeile" << row << ", Spalte " << col << ": " << csv[row][col];
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == '1') mDisplayGraph_1 = !mDisplayGraph_1;
    if (key == '2') mDisplayGraph_2 = !mDisplayGraph_2;
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
