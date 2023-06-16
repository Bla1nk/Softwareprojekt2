#include "ofApp.h"
#include "microNN.h"
#include "microNEAT.h"

#define PI 3.14159265359

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
	cam_z -= 10 * scrollY;

	camera.setPosition(ofVec3f(0, 0, cam_z));
}


//--------------------------------------------------------------
void ofApp::resetCar(int which)
{
	cars[which].origin = { (roadL[1].x + roadR[1].x) / 2 + 10, (roadL[1].y + roadR[1].y) / 2, (roadL[1].z + roadR[1].z) / 2 };

	cars[which].car_position.set(cars[which].origin);
	cars[which].previous_car_position = cars[which].car_position;

	ofVec2f v = (roadL[2] + roadR[2]) / 2 - (roadL[1] + roadR[1]) / 2;
	v = v / v.length();

	cars[which].drift_vector.clear();
	cars[which].car_direction.set(v.x, v.y, 0.0);

	for (int i = 0; i < drift_strength; i++)
	{
		cars[which].drift_vector.push_back(cars[which].car_direction);
	}

	cars[which].score = init_fitness;
}

//--------------------------------------------------------------
void ofApp::roadSelect(int pSelect)
{
	double x1 = mLoadAndDisplaySVGData[pSelect].scaledx;
	double y1 = (((mLoadAndDisplaySVGData[pSelect].vecArray[0].y / mLoadAndDisplaySVGData[pSelect].highNumy) * 0.7) + 0.1) * ofGetHeight();

	roadR.push_back(ofVec3f(x1 - ofGetWidth() * 0.5, y1 - ofGetHeight() * 0.5, 10));

	for (int i = 0; i < mLoadAndDisplaySVGData[pSelect].vecArray.size() - 1; i++) { 

		x1 = (i + 1) * mLoadAndDisplaySVGData[pSelect].scaledx;
		y1 = (((mLoadAndDisplaySVGData[pSelect].vecArray[i].y / mLoadAndDisplaySVGData[pSelect].highNumy) * 0.7) + 0.1) * ofGetHeight();

		roadL.push_back(ofVec3f(x1 - ofGetWidth() * 0.5, y1 - ofGetHeight() * 0.5, 10.0));
		roadR.push_back(ofVec3f(x1 - ofGetWidth() * 0.5, y1 - (ofGetHeight() * 0.5) - 100, 10.0));
	}

	roadR.push_back(roadL[roadL.size() - 1]);

	road_mesh = makeRoad();
}


//--------------------------------------------------------------
void ofApp::setup()
{
	ofBackground(ofColor(0, 0, 0));

	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("weltbevoelkerung.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("naturkatastrophen.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("co2ausstoss.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("kunstoffproduktion12.csv"));
	mLoadAndDisplaySVGData.push_back(loadAndDisplaySVGData("kunstoffproduktion13.csv"));

	roadIndex = 0;
	roadSelect(roadIndex);

	w_pressed = s_pressed = a_pressed = d_pressed = false;

	// Die Position der Kamera wird gesetzt.
	camera.setPosition(ofVec3f(0, 0, cam_z));

	// Der Punkt, welchen die Kamera anschaut, wird gesetzt.
	camera.lookAt(ofVec3f(0, 0, 0));

	// Je höher die "difficulty", desto einfacher!
	difficulty = 2;
	max_score = 0;
	crash_distance = 10.1;

	// GUI 1
	gui_pheno.setDefaultWidth(300);
	gui_pheno.setup("Phenotype", "Configuration System", ofGetWidth() * 0.7, ofGetHeight() * 0.7);

	gui_pheno.add(gui_ROTATION_SPEED.setup("Rotation speed", "0.01"));
	gui_pheno.add(gui_ROTATION_MAX_SPEED.setup("Rotation maximum speed", "1.0"));
	gui_pheno.add(gui_ROTATION_DETERIORATION_SPEED.setup("Rotation deterioration speed", "0.2"));
	gui_pheno.add(gui_SPEED_INCREMENT.setup("Speed increment", "0.0009"));
	gui_pheno.add(gui_SPEED_MAXIMUM.setup("Speed maximum", "0.8"));
	gui_pheno.add(gui_SPEED_DETERIORATION.setup("Speed deterioration", "0.04"));
	gui_pheno.add(gui_SPEED_RATIO_FW_BW.setup("Speed ratio forward backward", "4.0"));
	gui_pheno.add(gui_MAX_GENERATION_LIFE_SPAN.setup("Max generation life span", "3000"));
	gui_pheno.add(gui_DRIFT_STRENGTH.setup("Drift strength", "5"));

	gui_pheno.add(button_pheno.setup("Set values"));
	button_pheno.addListener(this, &ofApp::phenoButtonPressed);

	rotation_speed = 0.01;
	rotation_max_speed = 1.0;
	rotation_deterioration_speed = 0.2;
	speed_increment = 0.0009;
	speed_maximum = 0.8;
	speed_deterioration = 0.04;
	speed_ratio_fw_bw = 4.0;
	drift_strength = 5;
	max_generation_life_span = 3000;
	current_generation_lifetime = max_generation_life_span;

	// GUI 2
	gui_geno.setDefaultWidth(300);
	gui_geno.setup("Genotype", "Configuration System", ofGetWidth() * 0.7, ofGetHeight() * 0.5);

	gui_geno.add(gui_POPULATION_SIZE.setup("Population Size", "100"));
	gui_geno.add(gui_MUTATE_RANGE.setup("Mutate Range", "5.0"));
	gui_geno.add(gui_MIN_FITNESS.setup("Min Fitness", "0.1"));
	gui_geno.add(gui_INIT_FITNESS.setup("Init Fitness", "100"));
	

	gui_geno.add(button_geno.setup("Set values and Restart"));
	button_geno.addListener(this, &ofApp::genoButtonPressed);

    // GUI 3
    gui_geno_2.setDefaultWidth(300);
    gui_geno_2.setup("Genotype 2", "Configuration System", ofGetWidth() * 0.7, ofGetHeight() * 0.6);
    
    gui_geno_2.add(gui_REBIRTH_MUTATE_RANGE.setup("Rebirth Mutate Range", "0.2"));
    gui_geno_2.add(gui_REBIRTH_NUM_SURVIVORS.setup("Rebirth Num Survivors", "3"));
    
    gui_geno_2.add(button_geno_2.setup("Set values"));
    button_geno_2.addListener(this, &ofApp::genoButtonPressed_2);
    
    // GUI 4
    gui_graphen.setDefaultWidth(300);
    gui_graphen.setup("Graphen", "Configuration System", ofGetWidth() * 0.5, ofGetHeight() * 0.6);
    
    gui_graphen.add(button_graph_1.setup("Weltbevoelkerung"));
    button_graph_1.addListener(this, &ofApp::graph1ButtonPressed);
    gui_graphen.add(button_graph_2.setup("Naturkatastrophen"));
    button_graph_2.addListener(this, &ofApp::graph2ButtonPressed);
    gui_graphen.add(button_graph_3.setup("CO2 Ausstoss"));
    button_graph_3.addListener(this, &ofApp::graph3ButtonPressed);
    gui_graphen.add(button_graph_4.setup("Kunstoffproduktion Weltweit"));
    button_graph_4.addListener(this, &ofApp::graph4ButtonPressed);
    gui_graphen.add(button_graph_5.setup("Kunstoffproduktion Europa"));
    button_graph_5.addListener(this, &ofApp::graph5ButtonPressed);
    
	population_size = 100;
	mutate_range = 5.0;
	min_fitness = 0.0;
	init_fitness = 100;
	rebirth_mutate_range = 0.2;
	rebirth_num_survivors = 3;

	ai = true;

	// Inputs: car_speed (3 Werte) + distances front, back, left, right (4 Werte);
	// Outputs: accelerate, decelerate, left, right
	num_inputs = 9;
	num_outputs = 2;

	nnSample.setupNN(num_inputs, num_outputs);
	nnSample.addHiddenLayer(4);
	nnSample.initNN(2, -0.1, 0.1);

	neat.initializePopulation(population_size, nnSample, mutate_range, min_fitness, init_fitness);

	car newcar;

	for (int n = 0; n < population_size; n++)
	{
		cars.push_back(newcar);
		resetCar(n);
	}

	for (int i = 0; i < roadL.size(); i++)
	{
		std::vector<glm::vec3> crossLines;

	}
}


void ofApp::phenoButtonPressed() 
{
	std::string str;

	str = gui_ROTATION_SPEED;
	rotation_speed = stof(str);
    rotation_speed = maxmin(0.01, 2.0, stof(str));
    gui_ROTATION_SPEED = to_string_with_precision(rotation_speed, 2);

	str = gui_ROTATION_MAX_SPEED;
	rotation_max_speed = stof(str);
    rotation_max_speed = maxmin(0.1, 2.0, stof(str));
    gui_ROTATION_MAX_SPEED = to_string_with_precision(rotation_max_speed, 2);

	str = gui_ROTATION_DETERIORATION_SPEED;
    rotation_deterioration_speed = maxmin(0.1, 1.0, stof(str));
    gui_ROTATION_DETERIORATION_SPEED = to_string_with_precision(rotation_deterioration_speed, 2);

	str = gui_SPEED_INCREMENT;
    speed_increment = maxmin(0.0001, 1.0, stof(str));
    gui_SPEED_INCREMENT = to_string_with_precision(speed_increment, 4);

	str = gui_SPEED_MAXIMUM;
    speed_maximum = maxmin(0.1, 5.0, stof(str));
    gui_SPEED_MAXIMUM = to_string_with_precision(speed_maximum, 2);

	str = gui_SPEED_DETERIORATION;
    speed_deterioration = maxmin(0.01, 1.0, stof(str));
    gui_SPEED_DETERIORATION = to_string_with_precision(speed_deterioration, 2);

	str = gui_SPEED_RATIO_FW_BW;
    speed_ratio_fw_bw = maxmin(1.0, 10.0, stof(str));
    gui_SPEED_RATIO_FW_BW = to_string_with_precision(speed_ratio_fw_bw, 1);

	str = gui_MAX_GENERATION_LIFE_SPAN;
    max_generation_life_span = (int)maxmin(100.0, 10000.0, stof(str));
    gui_MAX_GENERATION_LIFE_SPAN = std::to_string(max_generation_life_span);

	str = gui_DRIFT_STRENGTH;
    drift_strength = (int)maxmin(0.0, 200.0, stof(str));
    gui_DRIFT_STRENGTH = std::to_string(drift_strength);
    
}

void ofApp::genoButtonPressed()
{
	for (int n = 0; n < neat.populationSize(); n++)
	{
		neat.kill(n);
	}

	cars.clear();

	std::string str;

	str = gui_POPULATION_SIZE;
	population_size = stoi(str);
    population_size = (int)maxmin(1.0, 1000.0, stof(str));
    gui_POPULATION_SIZE = std::to_string(population_size);

	str = gui_MUTATE_RANGE;
    mutate_range = maxmin(0.0, 10.0, stof(str));
    gui_MUTATE_RANGE = to_string_with_precision(mutate_range, 2);

	str = gui_MIN_FITNESS;
    min_fitness = maxmin(0.1, 1000.0, stof(str));
    gui_MIN_FITNESS = to_string_with_precision(min_fitness, 2);

	str = gui_INIT_FITNESS;
    init_fitness = (int)maxmin(10.0, 2000.0, stof(str));
    gui_INIT_FITNESS = std::to_string(init_fitness);

	neat.initializePopulation(population_size, nnSample, mutate_range, min_fitness, init_fitness);

	car newcar;

	for (int n = 0; n < population_size; n++)
	{
		cars.push_back(newcar);
		resetCar(n);
	}
}

void ofApp::genoButtonPressed_2(){
    std::string str;
    
    str = gui_REBIRTH_MUTATE_RANGE;
    rebirth_mutate_range = maxmin(0.0, 10.0, stof(str));
    gui_REBIRTH_MUTATE_RANGE = to_string_with_precision(rebirth_mutate_range, 2);

    str = gui_REBIRTH_NUM_SURVIVORS;
    rebirth_num_survivors = (int)maxmin(1.0, 5.0, stof(str));
    gui_REBIRTH_NUM_SURVIVORS = std::to_string(rebirth_num_survivors);
}

void ofApp::graph1ButtonPressed(){
    changeMap(1);
}

void ofApp::graph2ButtonPressed(){
    changeMap(2);
}

void ofApp::graph3ButtonPressed(){
    changeMap(3);
}

void ofApp::graph4ButtonPressed(){
    changeMap(4);
}

void ofApp::graph5ButtonPressed(){
    changeMap(5);
}

void ofApp::changeMap(int map){
    if (map == 1 || map == 2 || map == 3 || map == 4 || map == 5)
    {
        for (int n = 0; n < neat.populationSize(); n++)
        {
            neat.kill(n);
        }

        for (int n = 0; n < neat.populationSize(); n++)
        {
            resetCar(n);
        }

        roadL.clear();
        roadR.clear();

        if (map == 1) roadIndex = 0;
        if (map == 2) roadIndex = 1;
        if (map == 3) roadIndex = 2;
        if (map == 4) roadIndex = 3;
        if (map == 5) roadIndex = 4;

        roadSelect(roadIndex);
    }
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


ofVec3f ofApp::crossLines(ofVec2f p1, ofVec2f p2, ofVec2f p3, ofVec2f p4)
{
	ofVec3f result;
    
    result.z = 10.0;
    
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
        
        neat.rebirth(neat.apocalypse(rebirth_num_survivors), rebirth_mutate_range);
        cars.clear();
        
        for (int n = 0; n < neat.numPopulationAlive(); n++)
        {
            cars.push_back(newcar);
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
        
        new_vector = (probes[0] + probes[2]) / 2;
        probes.push_back(new_vector);
        
        new_vector = (probes[0] - probes[2]) / 2;
        probes.push_back(new_vector);
        
        for (int directions = 0; directions < probes.size(); directions++)
        {
            intersections.clear();
            
            for (int a = 0; a < roadL.size() - 1; a++)
            {
                if (a == roadL.size() - 1)
                {
                    roadL[a].x += 2000;
                }
                
                intersection = crossLines(roadL[a], roadL[a + 1], cars[n].car_position, cars[n].car_position + probes[directions]);
                
                if (isIntersectionInLine(roadL[a], roadL[a + 1], intersection))
                {
                    intersections.push_back(intersection);
                }
                
                if (a == roadL.size() - 1)
                {
                    roadL[a].x -= 2000;
                }
            }
            
            for (int b = 0; b < roadR.size() - 1; b++)
            {
                if (b == roadR.size() - 1)
                {
                    roadR[b].x += 2000;
                }
                
                intersection = crossLines(roadR[b], roadR[b + 1], cars[n].car_position, cars[n].car_position + probes[directions]);
                
                if (isIntersectionInLine(roadR[b], roadR[b + 1], intersection))
                {
                    intersections.push_back(intersection);
                }
                
                if (b == roadR.size() - 1)
                {
                    roadR[b].x -= 2000;
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
                
                float distanceGoalResetPos = 0;
                distanceGoalResetPos = cars[n].car_position.distance(0.5 * (roadL[roadL.size() - 2] + roadR[roadR.size() - 2]));
                
                if (distanceGoalResetPos <= 30)
                {
                    cars[n].origin = { (roadL[1].x + roadR[1].x) / 2 + 10, (roadL[1].y + roadR[1].y) / 2, (roadL[1].z + roadR[1].z) / 2 };
                    cars[n].car_position.set(cars[n].origin);
                    
                    ofVec2f v = (roadL[2] + roadR[2]) / 2 - (roadL[1] + roadR[1]) / 2;
                    v = v / v.length();
                    
                    cars[n].car_direction.set(v.x, v.y, 0.0);
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
    
    int slacker_punishment;
    int fervor;
    
    if (ai)
    {
        aiControl();
        
        for (int n = 0; n < cars.size(); n++)
        {
            control(n);
            
            cars[n].score--;
            
            cars[n].score += round(difficulty * cars[n].car_speed);
            
            float distanceGoalScore = 0;
            distanceGoalScore = cars[n].car_position.distance(0.5 * (roadL[roadL.size() - 1] + roadR[roadR.size() - 1]));
            cars[n].score += 100.0 / distanceGoalScore;
            
            if (distanceGoalScore <= 100.0)
            {
                cars[n].score += 10000.0;
            }
            
            if (cars[n].score > max_score) max_score = cars[n].score;
            
            cars[n].previous_car_position = cars[n].car_position;
            cars[n].car_position += cars[n].drift_vector.at(0) * cars[n].car_speed;
            cars[n].drift_vector.erase(cars[n].drift_vector.begin());
            
            cars[n].car_direction.rotate(cars[n].car_rotation, ofVec3f(0, 0, 1));
            cars[n].drift_vector.push_back(cars[n].car_direction);
            
            fervor = (difficulty * (cars[n].car_position - cars[n].previous_car_position).length());
            cars[n].score += fervor;
            
            if (fervor <= 0.1)
            {
                slacker_punishment = 2;
            }
            else
            {
                slacker_punishment = 0;
            }
            
            cars[n].score -= slacker_punishment;
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
	float distance_front_right;
	float distance_front_left;

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
		distance_front_right = cars[n].car_position.distance(cars[n].intersections[4]);
		distance_front_left = cars[n].car_position.distance(cars[n].intersections[5]);

		input.push_back(car_speed_vec.x);
		input.push_back(car_speed_vec.y);
		input.push_back(car_speed_vec.z);
		input.push_back(distance_front);
		input.push_back(distance_back);
		input.push_back(distance_right);
		input.push_back(distance_left);
		input.push_back(distance_front_right);
		input.push_back(distance_front_left);

		output = neat.react(input, n);

		cars[n].car_rotation += rotation_speed * output[0];

		float speed_increase = output[1];

		if (speed_increase > 1) speed_increase = 1;
		else if (speed_increase < 1) speed_increase = -1;

		cars[n].car_speed += speed_increment * output[1];

		limitCars(n);
	}

	neat.receiveFitness(fitnesses);

	neat.judgement();
}

void ofApp::control(int num)
{
	if (a_pressed)
	{
		cars[num].car_rotation += rotation_speed;
	}

	if (d_pressed)
	{
		cars[num].car_rotation -= rotation_speed;
	}

	if (w_pressed)
	{
		cars[num].car_speed += speed_increment;
	}

	if (s_pressed)
	{
		cars[num].car_speed -= speed_increment;
	}

	limitCars(num);
}


void ofApp::limitCars(int num)
{
	if (cars[num].car_rotation < 0)
	{
		cars[num].car_rotation += rotation_deterioration_speed;
		if (cars[num].car_rotation > 0) cars[num].car_rotation = 0;
	}
	else if (cars[num].car_rotation > 0)
	{
		cars[num].car_rotation -= rotation_deterioration_speed;
		if (cars[num].car_rotation < 0) cars[num].car_rotation = 0;
	}

	if (cars[num].car_rotation > rotation_max_speed)
	{
		cars[num].car_rotation = rotation_max_speed;
	}
	else if (-cars[num].car_rotation > rotation_max_speed)
	{
		cars[num].car_rotation = -rotation_max_speed;
	}

	if (cars[num].car_speed > 0)
	{
		cars[num].car_speed -= speed_deterioration;
		if (cars[num].car_speed < 0) cars[num].car_speed = 0;
	}
	else if (cars[num].car_speed < 0)
	{
		cars[num].car_speed += speed_deterioration;
		if (cars[num].car_speed > 0) cars[num].car_speed = 0;
	}

	if (cars[num].car_speed > speed_maximum)
	{
		cars[num].car_speed = speed_maximum;
	}
	else if (cars[num].car_speed < -speed_maximum / speed_ratio_fw_bw)
	{
		cars[num].car_speed = -speed_maximum / speed_ratio_fw_bw;
	}
}


//--------------------------------------------------------------
void ofApp::draw() 
{
	ofSetColor(ofColor::white);

	ofDrawBitmapString("NOT FOR MONKEYS II", 50, 50);
	ofDrawBitmapString("Score: " + ofToString(cars[0].score), 50, 70);
	ofDrawBitmapString("Max score: " + ofToString(max_score), 50, 90);
	ofDrawBitmapString("Current generation: " + ofToString(neat.whichGeneration()), 50, 110);
	ofDrawBitmapString("Current generation lifetime left: " + ofToString(current_generation_lifetime), 50, 130);
	ofDrawBitmapString("Current statistic: " + mLoadAndDisplaySVGData[roadIndex].fileName, 50, 150);

	glDisable(GL_DEPTH_TEST);
	gui_pheno.draw();
	gui_geno.draw();
    gui_geno_2.draw();
    gui_graphen.draw();
	glEnable(GL_DEPTH_TEST);

	// Die Kamera ist für die Perspektivische Darstellung zuständig.
	camera.begin();

	for (int n = 0; n < cars.size(); n++)
	{
		if (neat.isIndividualAlive(n))
		{
			ofSetColor(ofColor::red);
		}
		else
		{
			ofSetColor(ofColor::azure);
		}

		ofDrawCircle(cars[n].car_position, 2);
		ofDrawLine(cars[n].car_position, cars[n].car_position + cars[n].car_direction * 8);

		ofSetColor(ofColor::blue);

		if (neat.isIndividualAlive(n))
		{
			for (int m = 0; m < cars[n].intersections.size(); m++)
			{
				ofDrawLine(cars[n].car_position, cars[n].intersections[m]);
			}
		}
	}

	drawStatisticValues(roadIndex);
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


void ofApp::drawStatisticValues(int pSelect)
{
	ofSetColor(ofColor::gray);

	for (int i = 0; i < mLoadAndDisplaySVGData[pSelect].vecArray.size() - 1; i++) 
	{
		ofDrawBitmapString(mLoadAndDisplaySVGData[pSelect].vecArray[i + 1].y, roadL[i].x, roadL[i].y);
		ofDrawBitmapString(mLoadAndDisplaySVGData[pSelect].vecArray[i + 1].x, roadR[i].x, roadR[i].y);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
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
	// cout << x << " __ " << y << endl;
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
