#pragma once

#include "ofMain.h"
#include "ofxCsv.h"

class loadAndDisplaySVGData
{
public:
    loadAndDisplaySVGData(std::string pFileName) : fileName(pFileName), numRows(0), numCols(0), scaledx(0.0f), scaledy(0.0f), highNumy(0.0f)
    {
        bool loaded = csv.load(fileName);

        if (!loaded) {
            ofLogError() << "Csv-Datei " << fileName << " konnte nicht geladen werden";
            return;
        }

        csv.trim();
        numRows = csv.getNumRows();
        numCols = csv.getRow(0).size();

        for (int i = 1; i < numRows; i++) {
            ofxCsvRow row = csv.getRow(i);
            for (int j = 0; j < numCols - 1; j++) {
                vecArray.push_back(glm::vec2(row.getFloat(j), row.getFloat(j + 1)));
                //std::cout << row.getFloat(j) << " " << row.getFloat(j + 1) << endl;
            }
        }

        float highNumx = 0;
        highNumy = 0;
        for (int i = 0; i < vecArray.size(); i++) {
            if (vecArray[i].x > highNumx) {
                highNumx = vecArray[i].x;
            }
            if (vecArray[i].y > highNumy) {
                highNumy = vecArray[i].y;
            }
        }

        float lowNumx = highNumx;
        float lowNumy = highNumy;
        for (int i = 0; i < vecArray.size(); i++) {
            if (vecArray[i].x < lowNumx) {
                lowNumx = vecArray[i].x;
            }
            if (vecArray[i].y < lowNumy) {
                lowNumy = vecArray[i].y;
            }
        }

        float diffx = highNumx - lowNumx;
        scaledx = ofGetWidth() / vecArray.size();
        //std::cout<<highNumx<<" " <<lowNumx<<endl;
        //std::cout<<scaledx<<" " <<scaledx * vecArray.size() <<endl;
        
        float diffy = highNumy - lowNumy;
        scaledy = ofGetHeight() / diffy;
        //std::cout<<highNumy<<" " <<lowNumy<<endl;
    }

    void display(unsigned int pRed, unsigned int pGreen, unsigned int pBlue, float pScaleFactor)
    {
        ofSetColor(pRed, pGreen, pBlue);

        for (int i = 0; i < vecArray.size() - 1; i++) { // iterate through all ellipses except the last one
            float x1 = i * scaledx;
            float y1 = ofGetHeight() - ((((vecArray[i].y / highNumy) * 0.7) + 0.1) * ofGetHeight());

            float x2 = (i + 1) * scaledx;
            float y2 = ofGetHeight() - ((((vecArray[i+1].y / highNumy) * 0.7) + 0.1) * ofGetHeight());

            ofDrawEllipse(x1, y1, 10, 10); // draw current ellipse
            ofDrawLine(x1 + 5, y1, x2 - 5, y2);
        }

        int lastIndex = vecArray.size() - 1;
        ofDrawEllipse(lastIndex * scaledx, ofGetHeight() - ((((vecArray[lastIndex].y / highNumy) * 0.7) + 0.1) * ofGetHeight()), 10, 10);
    }

private:
    ofxCsv csv;
    std::string fileName;
    vector<float> data;
    std::vector<glm::vec2> vecArray;
    int numRows, numCols;
    float scaledx, scaledy, highNumy;
};


class ofApp : public ofBaseApp
{
    public:
        void setup();
        void update();
        void draw();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

        std::vector<loadAndDisplaySVGData> mLoadAndDisplaySVGData;
        bool mDisplayGraph_1 = true, mDisplayGraph_2 = true, mDisplayGraph_3 = true, mDisplayGraph_4 = true, mDisplayGraph_5 = true;
};
