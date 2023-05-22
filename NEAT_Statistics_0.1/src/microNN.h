#pragma once
#include "ofMain.h"

class microNN :  public ofBaseApp
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

	// This method creates a clone of this class' object.
	microNN clone();

	// This will mutate the weights, with a maximum amount.
	void mutate(float range);

	// This generates a random value between 0 and 1.
	float random();

	// This method is used by the clone() method.
	void hardSet(vector<int> new_layers, vector<float> new_nodes, map<int, map<int, float>> new_connections, map<int, map<int, float>> new_connections_backwards);

	float fitness;
	bool dead;

private:

	bool addWeight(int node1, int node2, float value);
	void initTotal(float weight_min_value, float weight_max_value);
	void initForward(float weight_min_value, float weight_max_value);
	vector<int> getNodesIndicesOfLayer(int layer_num);

	// Standard uses the sum as accumulation function, has no boundaries, and the activation function is 1.
	vector<float> feedInStandard(vector<float> inputs);

	int num_inputs;
	int num_outputs;


	vector<int> layers;
	vector<float> nodes;

	map<int, map<int, float>> connections;
	map<int, map<int, float>> connections_backwards;
};

