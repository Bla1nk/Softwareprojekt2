#pragma once
#include "ofMain.h"

class ofNeat :  public ofBaseApp
{
public:
	int setupNN(int number_of_inputs, int number_of_outputs);
	int addHiddenLayer(int number_of_nodes);
	
	
	// Initial configuration:
	// 1: Total connection, including feedbacks.
	// 2: Forward connection only.
	int initNN(int initial_configuration, float weight_min_value, float weight_max_value);


	// This feeds an input in, and generates an output.
	vector<float> feedIn(vector<float> inputs, int type = 0);


	static float random();

private:

	bool addWeight(int node1, int node2, float value);
	void initTotal(float weight_min_value, float weight_max_value);
	void initForward(float weight_min_value, float weight_max_value);
	vector<int> getNodesIndicesOfLayer(int layer_num);

	int num_inputs;
	int num_outputs;
	float weights_min;
	float weights_max;

	vector<int> layers;
	vector<float> nodes;
	vector<float> weights;
	map<int, map<int, float>> connections;
};

