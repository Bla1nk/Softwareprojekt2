#pragma once

#include "ofMain.h"
#include "ofxCsv.h"

class loadAndDisplaySVGData
{
public:
	loadAndDisplaySVGData(std::string pFileName) : fileName(pFileName), numRows(0), numCols(0), scaledx(0.0f), scaledy(0.0f), highNumy(0.0f), diffy(0)
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
                std::cout << row.getFloat(j) << " " << row.getFloat(j + 1) << endl;
            }
        }

        highNumy = 0;
        for (int i = 0; i < vecArray.size(); i++) {
            if (vecArray[i].y > highNumy) {
                highNumy = vecArray[i].y;
            }
        }

        float lowNumy = highNumy;
        for (int i = 0; i < vecArray.size(); i++) {
            if (vecArray[i].y < lowNumy) {
                lowNumy = vecArray[i].y;
            }
        }

        diffy = lowNumy / highNumy;
	}

    void display(unsigned int pRed, unsigned int pGreen, unsigned int pBlue, float pScaleFactor)
    {
        ofSetColor(pRed, pGreen, pBlue);

        for (int i = 0; i < vecArray.size(); i++) {
            ofDrawEllipse(i * (ofGetWidth() / vecArray.size()), (1 - ((vecArray[i].y / highNumy) * pScaleFactor)) * ofGetHeight(), 10, 10);
        }
    }

    float diffy;

private:
	ofxCsv csv;
    std::string fileName;
	vector<float> data;
	std::vector<glm::vec2> vecArray;
	int numRows, numCols;
	float scaledx, scaledy;
    float highNumy;
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
        float highNumyFromClass = 0;
};
