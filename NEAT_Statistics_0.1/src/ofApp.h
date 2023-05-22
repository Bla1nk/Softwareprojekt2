#pragma once

#include "ofMain.h"
#include "car.h"
#include "microNEAT.h"
#include "ofxCsv.h"

class loadAndDisplaySVGData
{
public:
    loadAndDisplaySVGData(std::string pFileName) : fileName(pFileName), numRows(0), numCols(0), scaledx(0.0f), scaledy(0.0f), highNumy(0.0f)
    {
        loaded = csv.load(fileName);

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

        float diffy = highNumy - lowNumy;
        scaledy = ofGetHeight() / diffy;
    }

    std::vector<glm::vec2> vecArray;
    std::vector<float> data;
    float scaledx, scaledy, highNumy;
    bool loaded;

private:
    ofxCsv csv;
    std::string fileName;
    int numRows, numCols;
};

class ofApp : public ofBaseApp{

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

        void mouseScrolled(int x, int y, float scrollX, float scrollY);

        std::vector<loadAndDisplaySVGData> mLoadAndDisplaySVGData;
        bool mDisplayGraph_1 = true, mDisplayGraph_2 = true, mDisplayGraph_3 = true, mDisplayGraph_4 = true, mDisplayGraph_5 = true;

        void roadSelect(int pCsvName);
		ofMesh makeRoad();
		ofVec2f crossLines(ofVec2f p1, ofVec2f p2, ofVec2f p3, ofVec2f p4);
		bool isIntersectionInLine(ofVec2f p1, ofVec2f p2, ofVec2f intersection);
		void resetCar(int which);

		void drawTrack();

		bool ai;
		microNEAT neat;

		vector<car> cars;

		ofCamera camera;

		vector<ofVec3f> roadL;
		vector<ofVec3f> roadR;

		ofMesh road_mesh;

		float difficulty;

        float cam_z = 600;

		bool w_pressed;
		bool s_pressed;
		bool a_pressed;
		bool d_pressed;

		int max_score;
		float crash_distance;


		int max_generation_life_span;
		int current_generation_lifetime;

private:
	void limitCars(int num);

	void control(int num);
	void aiControl();
};
