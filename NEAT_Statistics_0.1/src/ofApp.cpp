#include "ofApp.h"
#include "microNN.h"
#include "microNEAT.h"

#define ROTATION_SPEED 0.1
#define ROTATION_MAX_SPEED 2.0
#define ROTATION_DETERIORATION_SPEED 0.05

#define SPEED_INCREMENT 0.02
#define SPEED_MAXIMUM 0.8
#define SPEED_DETERIORATION 0.001
#define SPEED_RATIO_FW_BW 2.0

#define PI 3.14159265359

#define DRIFT_STRENGTH 50


void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
	cam_z -= 10 * scrollY;

	camera.setPosition(
		ofVec3f(0, 0, cam_z)
	);
}

//--------------------------------------------------------------
void ofApp::resetCar(int which)
{
	cars[which].origin = { (roadL[1].x + roadR[1].x) / 2 + 5, (roadL[1].y + roadR[1].y) / 2, (roadL[1].z + roadR[1].z) / 2 };
	// cout << cars[which].origin << endl;

	cars[which].car_position.set(cars[which].origin);

	ofVec2f v = (roadL[2] + roadR[2]) / 2 - (roadL[1] + roadR[1]) / 2;
	v = v / v.length();

	cars[which].car_direction.set(v.x, v.y, 0.0);
	cars[which].drift_vector.clear();

	for (int i = 0; i < DRIFT_STRENGTH; i++)
		cars[which].drift_vector.push_back(cars[which].car_direction);

	cars[which].score = 1000;
}

//--------------------------------------------------------------
void ofApp::roadSelect(int pSelect)
{
	float x1 = mLoadAndDisplaySVGData[pSelect].scaledx;
	float y1 = (((mLoadAndDisplaySVGData[pSelect].vecArray[0].y / mLoadAndDisplaySVGData[pSelect].highNumy) * 0.7) + 0.1) * ofGetHeight();
	roadL.push_back(ofVec3f(x1 - ofGetWidth() / 2, y1 - ofGetHeight() / 2, 10.0));
	roadR.push_back(ofVec3f(x1 - ofGetWidth() / 2, y1 - (ofGetHeight() / 2) - 50, 10.0));
	for (int i = 1; i < mLoadAndDisplaySVGData[pSelect].vecArray.size(); i++) { // iterate through all ellipses except the last one

		x1 = i * mLoadAndDisplaySVGData[pSelect].scaledx;
		y1 = (((mLoadAndDisplaySVGData[pSelect].vecArray[i].y / mLoadAndDisplaySVGData[pSelect].highNumy) * 0.7) + 0.1) * ofGetHeight();

		std::cout << x1 - ofGetWidth() / 2 << " " << y1 - ofGetHeight() / 2 << " " << y1 - (ofGetHeight() / 2) - 50 << std::endl;

		roadL.push_back(ofVec3f(x1 - ofGetWidth() / 2, y1 - ofGetHeight() / 2, 10.0));
		roadR.push_back(ofVec3f(x1 - ofGetWidth() / 2, y1 - (ofGetHeight() / 2) - 50, 10.0));
	}
	roadL.push_back(roadR[roadR.size() - 1]);
	roadR.push_back(roadL[roadL.size() - 1]);

	for (int i = 0; i < roadL.size(); i++)
	{
		std::cout << roadL[i].x << " " << roadL[i].y << " " << roadL[i].z << std::endl;
	}

	road_mesh = makeRoad();
}

//--------------------------------------------------------------
void ofApp::setup(){

	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("weltbevoelkerung.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("naturkatastrophen.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("co2ausstoss.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("kunstoffproduktion12.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("kunstoffproduktion13.csv"));
	roadSelect(3);

	max_generation_life_span = 2000;
	current_generation_lifetime = max_generation_life_span;

	ai = true;

	microNN nnSample;

	// Inputs: car_speed (3 Werte) + distances front, back, left, right (4 Werte);
	// Outputs: accelerate, decelerate, left, right
	int num_inputs = 7;
	int num_outputs = 2;
	int population_size = 100;
	float mutate_range = 5.0;
	float min_fitness = 0.0;
	float init_fitness = 1000;

	//nnSample.addHiddenLayer(4);

	nnSample.setupNN(num_inputs, num_outputs);
	nnSample.initNN(2, -1.0, 1.0);

	neat.initializePopulation(population_size, nnSample, mutate_range, min_fitness, init_fitness);

	car newcar;

	for (int n = 0; n < population_size; n++)
	{
		cars.push_back(newcar);
		//resetCar(n, { roadL[roadL.size() / 2].x, roadL[roadL.size() / 2].y - 25, roadL[roadL.size() / 2].z });
		resetCar(n);
	}

	// Je höher die "difficulty", desto einfacher!
	difficulty = 2;

	max_score = 0;

	crash_distance = 10.1;

	w_pressed = s_pressed = a_pressed = d_pressed = false;



	// Die Position der Kamera wird gesetzt.
	camera.setPosition(
		ofVec3f(0, 0, cam_z)
	);

	// Der Punkt, welchen die Kamera anschaut, wird gesetzt.
	camera.lookAt(
		ofVec3f(0, 0, 0)
	);

	// Der Tiefentest wird eingeschaltet, sodass Objekte, die sich hinter
	// einem anderen befinden, verborgen werden.
	glEnable(GL_DEPTH_TEST);
}

ofMesh ofApp::makeRoad()
{
	ofMesh r;

	int length = roadL.size();

	for (int n = 0; n < length - 1; n++)
	{
		r.addVertex(roadL[n]);
		r.addVertex(roadL[n + 1]);
		r.addVertex(roadR[n]);

		r.addVertex(roadL[n + 1]);
		r.addVertex(roadR[n + 1]);
		r.addVertex(roadR[n]);
	}

	return r;
}

ofVec2f ofApp::crossLines(ofVec2f p1, ofVec2f p2, ofVec2f p3, ofVec2f p4)
{
	ofVec2f result;

	float m1, b1, m2, b2;
	float threshold = 0.00001;

	bool line1_vertical = false;
	bool line1_horizontal = false;
	bool line2_vertical = false;
	bool line2_horizontal = false;

	// Gerade 1 vertikal?
	if (abs(p1.x - p2.x) < threshold)
	{
		line1_vertical = true;
	}

	// Gerade 1 horizontal?
	if (abs(p1.y - p2.y) < threshold)
	{
		line1_horizontal = true;
	}

	// Gerade 2 vertikal?
	if (abs(p3.x - p4.x) < threshold)
	{
		line2_vertical = true;
	}

	// Gerade 2 horizontal?
	if (abs(p3.y - p4.y) < threshold)
	{
		line2_horizontal = true;
	}

	if (line1_vertical && line2_vertical || line1_horizontal && line2_horizontal)
	{
		result.x = 100000000000;
		result.y = 100000000000;

		return result;
	}

	if (line1_vertical)
	{
		result.x = p1.x;

		m2 = (p3.y - p4.y) / (p3.x - p4.x);
		b2 = p3.y - m2 * p3.x;

		result.y = m2 * p1.x + b2;
		return result;
	}

	if (line1_horizontal)
	{
		result.y = p1.y;

		if (line2_vertical)
		{
			result.x = p3.x;
			return result;
		}

		m2 = (p3.y - p4.y) / (p3.x - p4.x);
		b2 = p3.y - m2 * p3.x;

		result.x = (result.y - b2) / m2;
		return result;
	}

	if (line2_vertical)
	{
		result.x = p3.x;

		m1 = (p1.y - p2.y) / (p1.x - p2.x);
		b1 = p1.y - m1 * p1.x;

		result.y = m1 * p1.x + b1;
		return result;
	}

	if (line2_horizontal)
	{
		result.y = p3.y;

		if (line1_vertical)
		{
			result.x = p1.x;
			return result;
		}

		m1 = (p1.y - p2.y) / (p1.x - p2.x);
		b1 = p1.y - m1 * p1.x;

		result.x = (result.y - b1) / m1;
		return result;
	}

	m1 = (p1.y - p2.y) / (p1.x - p2.x);
	m2 = (p3.y - p4.y) / (p3.x - p4.x);


	b1 = p1.y - m1 * p1.x;
	b2 = p3.y - m2 * p3.x;

	result.y = (-m1 * b2 + m2 * b1) / (m2 - m1);
	result.x = (result.y - b1) / m1;

	return result;
}

bool ofApp::isIntersectionInLine(ofVec2f p1, ofVec2f p2, ofVec2f intersection)
{
	float xleft, xright, ytop, ybottom;

	if (p1.x < p2.x) 
	{ 
		xleft = p1.x;
		xright = p2.x;
	}
	else
	{
		xleft = p2.x;
		xright = p1.x;
	}

	if (p1.y < p2.y)
	{
		ytop = p2.y;
		ybottom = p1.y;
	}
	else
	{
		ytop = p1.y;
		ybottom = p2.y;
	}

	if (   xleft <= intersection.x   && intersection.x <= xright
		&& ybottom <= intersection.y && intersection.y <= ytop)
	{
		return true;
	}

	return false;
}


//--------------------------------------------------------------
void ofApp::update()
{
	for (int n = 0; n < cars.size(); n++)
	{
		if (cars[n].car_position.x > ((roadL[roadL.size() - 1] + roadR[roadR.size() - 1]) / 2).x - 20)
		{
			resetCar(n);
		}
	}
	ofVec3f intersection;
	vector <ofVec3f> intersections;
	vector <vector <ofVec3f>> intersections_per_probe;

	float distance_to_border;

	vector<ofVec3f> probes;
	ofVec3f new_vector;

	car newcar;

	current_generation_lifetime--;
	if (current_generation_lifetime <= 0)
	{
		for (int n = 0; n < neat.populationSize(); n++)
		{
			neat.kill(n);
		}
	}

	if (neat.numPopulationAlive() == 0)
	{
		current_generation_lifetime = max_generation_life_span;

		neat.rebirth(neat.apocalypse(3), 0.2);
		cars.clear();
		
		for (int n = 0; n < neat.numPopulationAlive(); n++)
		{
			cars.push_back(newcar);
			// resetCar(n, { roadL[roadL.size() / 2].x, roadL[roadL.size() / 2].y - 25, roadL[roadL.size() / 2].z });
			resetCar(n);


		}

		return;
	}



	// Find distances to the border for each car.
	for (int n = 0; n < cars.size(); n++)
	{
		if (ai)
		{

			if (!neat.isIndividualAlive(n)) continue;
		}
		else
		{
			if (n != 0) continue;
		}

		intersections.clear();
		intersections_per_probe.clear();



		// It is necessary to generate four rays to "see" the boundaries.
		// One is cast forward, one backwards, and two towards the left and right.
		probes.clear();

		// Forwards (pointing in the car's direction):
		probes.push_back(cars[n].car_direction);

		// The vector car_direction points forward.
		// The back vector faces the exact opposite direction:
		new_vector.x = -cars[n].car_direction.x;
		new_vector.y = -cars[n].car_direction.y;
		probes.push_back(new_vector);

		// The vector pointing right (Why this way? Picture it and draw the vectors
		// on a sheet of paper!):
		new_vector.x = cars[n].car_direction.y;
		new_vector.y = -cars[n].car_direction.x;
		probes.push_back(new_vector);

		// The vector pointing left. It's the inverse of the one pointing right.
		new_vector.x = -cars[n].car_direction.y;
		new_vector.y = cars[n].car_direction.x;
		probes.push_back(new_vector);

		for (int directions = 0; directions < probes.size(); directions++)
		{

			intersections.clear();

			for (int a = 0; a < roadL.size() - 1; a++)
			{
				intersection = crossLines(roadL[a], roadL[a + 1], cars[n].car_position, cars[n].car_position + probes[directions]);

				if (isIntersectionInLine(roadL[a], roadL[a + 1], intersection))
				{
					intersections.push_back(intersection);
				}
			}

			for (int b = 0; b < roadR.size() - 1; b++)
			{
				intersection = crossLines(roadR[b], roadR[b + 1], cars[n].car_position, cars[n].car_position + probes[directions]);

				if (isIntersectionInLine(roadR[b], roadR[b + 1], intersection))
				{
					intersections.push_back(intersection);
				}
			}

			intersections_per_probe.push_back(intersections);
		}

		int closest = 0;
		float distance;
		float min_distance;
		float angle;
		intersections.clear();


		for (int i = 0; i < intersections_per_probe.size(); i++)
		{
			min_distance = 1000000;
			closest = 0;

			for (int j = 0; j < intersections_per_probe[i].size(); j++)
			{
				angle = probes[i].angle(intersections_per_probe[i][j] - cars[n].car_position);
				distance = cars[n].car_position.distance(intersections_per_probe[i][j]);

				if (angle < 90.0)
				{
					if (distance < min_distance)
					{
						min_distance = distance;
						closest = j;
					}
				}
			}

			intersections.push_back(intersections_per_probe[i][closest]);
		}

		cars[n].intersections = intersections;

		for (int c = 0; c < intersections.size(); c++)
		{

			distance_to_border = cars[n].car_position.distance(intersections[c]);

			if (!ai)
			{
				if (distance_to_border < crash_distance)
				{
					resetCar(n);

				}
			}
			else
			{	
				// ofVec2f intersection = crossLines(cars[n].car_position, cars[n].car_position + cars[n].car_direction, roadL[roadL.size() - 1], roadR[roadR.size() - 1]);
				// if (isIntersectionInLine(roadL[roadL.size() - 1], roadR[roadR.size() - 1], intersection) && intersection.distance(cars[n].car_position) <= crash_distance)
				if (cars[n].car_position.x >= ofGetWidth() / 4)
				{
					std::cout << "out of frame " << std::endl;
					cars[n].car_position = { roadL[0].x + 20, roadL[0].y - 25, roadL[0].z };
				}
				else if (distance_to_border < crash_distance)
				{
					// std::cout << "intercection " << intersection.distance(cars[n].car_position) << std::endl;

					neat.kill(n);
					resetCar(n);

				}
			}
		}
	}


	if (ai)
	{
		aiControl();

		for (int n = 0; n < cars.size(); n++)
		{
			control(n);

			float distanceGoal = 0;
			distanceGoal = cars[n].car_position.distance(0.5 * (roadL[roadL.size() - 1] + roadR[roadR.size() - 1]));
			cars[n].score += 20.0 / distanceGoal;

			cars[n].score--;

			cars[n].score += round(difficulty * cars[n].car_speed);

			if (cars[n].score > max_score) max_score = cars[n].score;

			cars[n].car_position += cars[n].drift_vector.at(0) * cars[n].car_speed;
			cars[n].drift_vector.erase(cars[n].drift_vector.begin());

			cars[n].car_direction.rotate(cars[n].car_rotation, ofVec3f(0, 0, 1));
			cars[n].drift_vector.push_back(cars[n].car_direction);
		}
	}
	else if (!ai)
	{
		control(0);
		
		cars[0].score--;

		cars[0].score += round(difficulty * cars[0].car_speed);

		if (cars[0].score > max_score) max_score = cars[0].score;

		cars[0].car_position += cars[0].drift_vector.at(0) * cars[0].car_speed;
		cars[0].drift_vector.erase(cars[0].drift_vector.begin());

		cars[0].car_direction.rotate(cars[0].car_rotation, ofVec3f(0, 0, 1));
		cars[0].drift_vector.push_back(cars[0].car_direction);

	}
}

void ofApp::aiControl()
{
	// Inputs: car_speed (3 Werte) + distances front, back, left, right (4 Werte);
	// Outputs: accelerate, decelerate, left, right
	vector<float> input;
	vector<float> output;
	ofVec3f car_speed_vec;

	vector<float> fitnesses;

	float distance_front;
	float distance_back;
	float distance_left;
	float distance_right;

	for (int n = 0; n < cars.size(); n++)
	{

		fitnesses.push_back(cars[n].score);
		if (!neat.isIndividualAlive(n))
		{
			cars[n].car_speed = 0;
			continue;
		}

		input.clear();
		output.clear();

		car_speed_vec  = cars[n].car_direction.getNormalized() * cars[n].car_speed;
		distance_front = cars[n].car_position.distance(cars[n].intersections[0]);
		distance_back  = cars[n].car_position.distance(cars[n].intersections[1]);
		distance_right = cars[n].car_position.distance(cars[n].intersections[2]);
		distance_left  = cars[n].car_position.distance(cars[n].intersections[3]);

		input.push_back(car_speed_vec.x);
		input.push_back(car_speed_vec.y);
		input.push_back(car_speed_vec.z);
		input.push_back(distance_front);
		input.push_back(distance_back);
		input.push_back(distance_right);
		input.push_back(distance_left);

		output = neat.react(input, n);

		cars[n].car_rotation += ROTATION_SPEED * output[0];

		float speed_increase = output[1];

		if (speed_increase > 1) speed_increase = 1;
		else if (speed_increase < 1) speed_increase = -1;

		cars[n].car_speed += SPEED_INCREMENT * output[1];

		limitCars(n);

	}

	neat.receiveFitness(fitnesses);

	neat.judgement();
}

void ofApp::control(int num)
{
	if (a_pressed)
	{
		cars[num].car_rotation += ROTATION_SPEED;
	}

	if (d_pressed)
	{
		cars[num].car_rotation -= ROTATION_SPEED;
	}

	if (w_pressed)
	{
		cars[num].car_speed += SPEED_INCREMENT;
	}

	if (s_pressed)
	{
		cars[num].car_speed -= SPEED_INCREMENT;
	}

	limitCars(num);
}

void ofApp::limitCars(int num)
{
	if (cars[num].car_rotation < 0)
	{
		cars[num].car_rotation += ROTATION_DETERIORATION_SPEED;
		if (cars[num].car_rotation > 0) cars[num].car_rotation = 0;
	}
	else if (cars[num].car_rotation > 0)
	{
		cars[num].car_rotation -= ROTATION_DETERIORATION_SPEED;
		if (cars[num].car_rotation < 0) cars[num].car_rotation = 0;
	}

	if (cars[num].car_rotation > ROTATION_MAX_SPEED)
	{
		cars[num].car_rotation = ROTATION_MAX_SPEED;
	}
	else if (-cars[num].car_rotation > ROTATION_MAX_SPEED)
	{
		cars[num].car_rotation = -ROTATION_MAX_SPEED;
	}

	if (cars[num].car_speed > 0)
	{
		cars[num].car_speed -= SPEED_DETERIORATION;
		if (cars[num].car_speed < 0) cars[num].car_speed = 0;
	}
	else if (cars[num].car_speed < 0)
	{
		cars[num].car_speed += SPEED_DETERIORATION;
		if (cars[num].car_speed > 0) cars[num].car_speed = 0;
	}

	if (cars[num].car_speed > SPEED_MAXIMUM)
	{
		cars[num].car_speed = SPEED_MAXIMUM;
	}
	else if (cars[num].car_speed < -SPEED_MAXIMUM / SPEED_RATIO_FW_BW)
	{
		cars[num].car_speed = -SPEED_MAXIMUM / SPEED_RATIO_FW_BW;
	}
}


//--------------------------------------------------------------
void ofApp::draw() 
{
	ofSetColor(ofColor::white);

	ofDrawBitmapString("Stupid Racer V0.1", 50, 50);
	ofDrawBitmapString("Score: " + ofToString(cars[0].score), 50, 70);
	ofDrawBitmapString("Max score: " + ofToString(max_score), 50, 90);
	ofDrawBitmapString("Current generation: " + ofToString(neat.whichGeneration()), 50, 110);
	ofDrawBitmapString("Current generation lifetime left: " + ofToString(current_generation_lifetime), 50, 130);

	// Die Kamera ist für die Perspektivische Darstellung zuständig.
	camera.begin();

	for (int n = 0; n < cars.size(); n++)
	{
		ofSetColor(ofColor::red);
		ofDrawCircle(cars[n].car_position, 2);
		ofDrawLine(cars[n].car_position, cars[n].car_position + cars[n].car_direction * 8);

		ofSetColor(ofColor::blue);

		for (int m = 0; m < cars[n].intersections.size(); m++)
		{
			ofDrawLine(cars[n].car_position, cars[n].intersections[m]);
		}
	}

	drawTrack();

	camera.end();


}


void ofApp::drawTrack()
{
	ofSetColor(ofColor::green);

	for (int x = 0; x < roadL.size() - 1; x++)
	{
		ofDrawLine(roadL[x], roadL[x + 1]);
	}

	for (int x = 0; x < roadR.size() - 1; x++)
	{
		ofDrawLine(roadR[x], roadR[x + 1]);
	}
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5')
	{
		for (int n = 0; n < neat.populationSize(); n++)
		{
			neat.kill(n);
		}

		roadL.clear();
		roadR.clear();

		if (key == '1') roadSelect(0);
		if (key == '2') roadSelect(1);
		if (key == '3') roadSelect(2);
		if (key == '4') roadSelect(3);
		if (key == '5') roadSelect(4);

		for (int n = 0; n < neat.populationSize(); n++)
		{
			resetCar(n);

		}
	}



	if (key == 119) // "w" wurde gedrückt.
	{
		w_pressed = true;
	}

	if (key == 115) // "s" wurde gedrückt.
	{
		s_pressed = true;
	}

	if (key == 97) // "a" wurde gedrückt.
	{
		a_pressed = true;
	}

	if (key == 100) // "d" wurde gedrückt.
	{
		d_pressed = true;
	}


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	if (key == 119) // "w" wurde losgelassen.
	{
		w_pressed = false;
	}

	if (key == 115) // "s" wurde losgelassen.
	{
		s_pressed = false;
	}

	if (key == 97) // "a" wurde losgelassen.
	{
		a_pressed = false;
	}

	if (key == 100) // "d" wurde losgelassen.
	{
		d_pressed = false;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	cout << x << " __ " << y << endl;
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
