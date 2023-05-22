#pragma once
#include "microNN.h"

class microNEAT :  public microNN
{
public:
	void initializePopulation(
		int num_pop,
		microNN sample,
		float mutate_range,
		float min_fitness,
		int init_fitness
	);

	vector<float>			react(vector<float> inputs, int which);
	void					receiveFitness(vector<float> current_fitnesses);
	void					judgement();
	vector<microNN>			apocalypse(int num_survivors);
	void					rebirth(vector<microNN> survivors, float mutation);


	void					kill(int which);

	int						populationSize();
	int						numPopulationAlive();
	bool					isIndividualAlive(int which);
	int						whichGeneration();

private:

	int findIndexOfSmallest(vector<float>vals);

	vector <microNN> population;

	float death_condition;
	int pop_size_max;
	int initial_fitness;

	int generation_number;
};

