#include "microNEAT.h"

void microNEAT::initializePopulation(int num_pop, microNN sample, float mutate_range, float min_fitness, int init_fitness)
{
	population.clear();

	generation_number = 0;

	initial_fitness = init_fitness;

	microNN newNN;

	for (int n = 0; n < num_pop; n++)
	{
		newNN = sample.clone();
		newNN.mutate(mutate_range);
		newNN.fitness = init_fitness;

		population.push_back(newNN);
	}

	pop_size_max = num_pop;
	death_condition = min_fitness;
}

void microNEAT::kill(int which)
{
	//cout << "Oh no, I died! \n";
	population[which].dead = true;
}

int microNEAT::numPopulationAlive()
{
	int num = 0;

	for (int n = 0; n < population.size(); n++)
	{
		if (!population[n].dead) num++;
	}

	return num;
}

vector<float> microNEAT::react(vector<float> inputs, int which)
{
	vector<float> output;
	output = population[which].feedIn(inputs, 0);

	return output;
}

bool microNEAT::isIndividualAlive(int which)
{
	return !population[which].dead;
}

void microNEAT::receiveFitness(vector<float> current_fitnesses)
{
	
	for (int n = 0; n < population.size(); n++)
	{

		population[n].fitness = current_fitnesses[n];
	}
}

void microNEAT::judgement()
{
	for (int n = 0; n < population.size(); n++)
	{
		if (death_condition > population[n].fitness)
		{
			kill(n);
		}
	}
}

int microNEAT::findIndexOfSmallest(vector<float> vals)
{
	float value;
	int smallest_index = 0;

	value = vals[smallest_index];

	for (int n = 0; n < vals.size(); n++)
	{
		if (vals[n] < value)
		{
			value = vals[n];
			smallest_index = n;
		}
	}

	return smallest_index;
}


vector<microNN> microNEAT::apocalypse(int num_survivors)
{
	vector<float> best;
	vector<int> location_best;
	vector<microNN> bestNN;
	float worst = -10000000;
	int location_smallest;

	//cout << "\nApocalypse!\n\n";

	for (int n = 0; n < num_survivors; n++)
	{
		best.push_back(worst);
		location_best.push_back(-1);
	}

	//cout << "\npop size: " << population.size() << "\n";

	for (int x = 0; x < population.size(); x++)
	{
		location_smallest = findIndexOfSmallest(best);
		//cout << "\n fit best " << population[x].fitness << "\n";
		if (population[x].fitness > best[location_smallest])
		{
			best[location_smallest] = population[x].fitness;
			location_best[location_smallest] = x;
		}
	}

	//cout << "\nloc best: " << location_best[0] << "\n";

	for (int a = 0; a < num_survivors; a++)
	{
		bestNN.push_back(population[location_best[a]]);
	}

	population.clear();

	return bestNN;
}

int microNEAT::whichGeneration()
{
	return generation_number;
}

void microNEAT::rebirth(vector<microNN> survivors, float mutation)
{
	//cout << "\nRebirth!\n----------------\n\nsurvivors number: " << survivors.size() << "\n";
	microNN newborn;

	int culture_size = (int)ceil((float)pop_size_max / (float)survivors.size());

	for (int a = 0; a < survivors.size(); a++)
	{
		for (int b = 0; b < culture_size; b++)
		{
			newborn = survivors[a].clone();
			//cout << "\nMutating!\n";
			newborn.mutate(mutation);
			//cout << "\nMutating done.\n";

			newborn.fitness = initial_fitness;
			newborn.dead = false;

			population.push_back(newborn);
		}
	}

	generation_number++;

	cout << "\nRebirthing done. Current generation: " << generation_number << "\n";
}

int microNEAT::populationSize()
{
	return population.size();
}