#pragma once

#include "ofMain.h"
#include "car.h"
#include "microNEAT.h"

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
