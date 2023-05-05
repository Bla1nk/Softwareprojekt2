#include "ofNeat.h"

int ofNeat::setupNN(int number_of_inputs, int number_of_outputs)
{
	num_inputs = number_of_inputs;
	num_outputs = number_of_outputs;

	layers.clear();
	weights.clear();

	float rand_val;
	rand_val = random() * (weights_max - weights_min) + weights_min;

	for (int a = 0; a < num_inputs; a++)
	{
		nodes.push_back(0.0);
	}

	layers.push_back(num_inputs);

	return 0;
}

int ofNeat::addHiddenLayer(int number_of_nodes)
{
	layers.push_back(number_of_nodes);

	for (int c = 0; c < number_of_nodes; c++)
	{
		nodes.push_back(0.0);
	}

	return 0;
}

int ofNeat::initNN(int initial_configuration, float weight_min_value, float weight_max_value)
{
	for (int b = 0; b < num_outputs; b++)
	{
		nodes.push_back(0.0);
	}

	layers.push_back(num_outputs);

	switch(initial_configuration)
	{
	case 1:
		initTotal(weight_min_value, weight_max_value);
		break;

	case 2:
		initForward(weight_min_value, weight_max_value);
		break;

	default:
		return 0;
	}

	return 1;
}


void ofNeat::initTotal(float weight_min_value, float weight_max_value)
{
	connections.clear();

	for (int a = 0; a < nodes.size(); a++)
	{
		for (int b = 0; b < nodes.size(); b++)
		{
			if (a == b) continue;

			connections[a][b] = (weight_min_value - weight_max_value) * random() + weight_min_value;
		}
	}
}


vector<int> ofNeat::getNodesIndicesOfLayer(int layer_num)
{
	vector<int> nodes_indices;
	int position = 0;

	for (int n = 0; n < layer_num; n++)
	{
		position += layers[n];
	}

	nodes_indices.push_back(position);
	nodes_indices.push_back(position + layers[layer_num]);

	return nodes_indices;
}

void ofNeat::initForward(float weight_min_value, float weight_max_value)
{
	connections.clear();
	
	int nodes_starting_index = 0;

	cout << "\n\nLayers size: " << layers.size() << "\n";

	vector<int> nodes_indices_a;
	vector<int> nodes_indices_b;


	for (int x = 0; x < layers.size() - 1; x++)
	{
		nodes_indices_a = getNodesIndicesOfLayer(x);
		nodes_indices_b = getNodesIndicesOfLayer(x + 1);

		for (int a = nodes_indices_a[0]; a < nodes_indices_a[1]; a++)
		{
			for (int b = nodes_indices_b[0]; b < nodes_indices_b[1]; b++)
			{
				connections[a][b] = random() * (weight_max_value - weight_min_value) + weight_min_value;
			}
		}
	}

	for (auto x = connections.begin(); x != connections.end(); x++)
	{
		cout << "\n" << x->first << "\n    ";
		for (auto y = x->second.begin(); y != x->second.end(); y++)
		{
			cout << "   " << y->first << ": " << y->second;
		}
		cout << "\n";
	}

	//cout << "\n\n";
	//vector <int> ind;

	//for (int test = 0; test < layers.size(); test++)
	//{
	//	ind = getNodesIndicesOfLayer(test);
	//	cout << "\nLayer: " << test << " nodes indices: " << ind[0] << " : " << ind[1] << "\n";
	//}
}

vector<float> ofNeat::feedIn(vector<float> inputs, int type)
{
	vector<float>output;



	return output;
}

bool ofNeat::addWeight(int node1, int node2, float value)
{
	connections[node1][node2] = value;

	return true;
}

float ofNeat::random()
{
	float test = rand();
	test = test / RAND_MAX;

	return test;
}