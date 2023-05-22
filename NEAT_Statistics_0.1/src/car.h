#pragma once
#include "ofMain.h"

class car : public ofBaseApp
{
public:
	ofVec3f car_position;
	ofVec3f car_direction;
	vector <ofVec3f> drift_vector;
	ofVec3f origin;

	float score;

	float car_speed;
	float car_rotation;

	vector <ofVec3f> intersections;
};

