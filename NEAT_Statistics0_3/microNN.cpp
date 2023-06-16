#pragma once
#include "microNN.h"

int microNN::setupNN(int number_of_inputs, int number_of_outputs)
{
	dead = false;
	fitness = 0;

	num_inputs = number_of_inputs;
	num_outputs = number_of_outputs;

	layers.clear();
	connections.clear();
	connections_backwards.clear();

	for (int a = 0; a < num_inputs; a++)
	{
		nodes.push_back(0.0);
	}

	layers.push_back(num_inputs);

	return 0;
}

int microNN::addHiddenLayer(int number_of_nodes)
{
	layers.push_back(number_of_nodes);

	for (int c = 0; c < number_of_nodes; c++)
	{
		nodes.push_back(0.0);
	}

	return 0;
}

int microNN::initNN(int initial_configuration, float weight_min_value, float weight_max_value)
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

void microNN::mutate(float range)
{
	float rand_test = ((float)random());
	//cout << "\nRandom value: \n" << rand_test << " Range: " << range << "\n";
	
	//cout << "\nMutating...\n";
	for (auto x = connections.begin(); x != connections.end(); x++)
	{
		//cout << "\n" << x->first << "\n    ";
		for (auto y = connections[x->first].begin(); y != connections[x->first].end(); y++)
		{
			//cout << "   " << y->first << ": " << y->second;
			//cout << "\nconnection before: " << connections[x->first][y->first] << "\n";
			connections[x->first][y->first]           += (random() - 0.5) * range;
			//cout << "\nconnection after: " << connections[x->first][y->first] << "\n";
			connections_backwards[y->first][x->first] = connections[x->first][y->first];
		}
		//cout << "\n";
	}
	//cout << "\n...Done mutating.\n";
}



void microNN::initTotal(float weight_min_value, float weight_max_value)
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


vector<int> microNN::getNodesIndicesOfLayer(int layer_num)
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

void microNN::initForward(float weight_min_value, float weight_max_value)
{
	connections.clear();
	
	int nodes_starting_index = 0;

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
				connections_backwards[b][a] = connections[a][b];
			}
		}
	}

}

vector<float> microNN::feedIn(vector<float> inputs, int type)
{
	vector<float> output;

	if (type == 0)
	{
		return feedInStandard(inputs);
	}

	return output;
}

vector<float> microNN::feedInStandard(vector<float> inputs)
{
	vector<float> output;
	vector<int> nodes_indices;

	float value;

	for (int i = 0; i < inputs.size(); i++)
	{
		nodes[i] = inputs[i];
	}

	for (int x = 1; x < layers.size(); x++)
	{
		nodes_indices = getNodesIndicesOfLayer(x);

		for (int y = nodes_indices[0]; y < nodes_indices[1]; y++)
		{

			value = 0.0;

			for (auto z = connections_backwards[y].begin(); z != connections_backwards[y].end(); z++)
			{
				value += nodes[z->first] * connections_backwards[y][z->first];
			}

			nodes[y] = value;
		}
	}

	nodes_indices = getNodesIndicesOfLayer(layers.size() - 1);

	// cout << "\n\nOutputs: ";

	for (int j = nodes_indices[0]; j < nodes_indices[1]; j++)
	{
		output.push_back(nodes[j]);
		//cout << nodes[j] << " ";
	}

	// cout << "\n";

	return output;
}

bool microNN::addWeight(int node1, int node2, float value)
{
	connections[node1][node2] = value;

	return true;
}

float microNN::random()
{
	float test = rand();
	test = test / RAND_MAX;

	return test;
}

microNN microNN::clone()
{
	microNN baby;

	baby.hardSet(layers, nodes, connections, connections_backwards);

	return baby;
}

void microNN::hardSet(vector<int> new_layers, vector<float> new_nodes, map<int, map<int, float>> new_connections, map<int, map<int, float>> new_connections_backwards)
{
	int n;

	layers.clear();


	for(n = 0; n < new_layers.size(); n++)
		layers.push_back(new_layers[n]);

	nodes.clear();
	for (n = 0; n < new_nodes.size(); n++)
	{
		nodes.push_back(new_nodes[n]);
		//cout << nodes[n] << "\n";
	}

	connections.clear();
	connections_backwards.clear();

	connections.insert(new_connections.begin(), new_connections.end());
	connections_backwards.insert(new_connections_backwards.begin(), new_connections_backwards.end());


	dead = false;
}