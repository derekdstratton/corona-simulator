//
// Created by derek on 4/30/20.
//

#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "area.h"
#include "Timer.h"

using namespace std;

//NOTE: SINCE THE SIM STARTS WITH 1 INFECTED INDIVIDUAL, THEY MIGHT DIE OR RECOVER BEFORE THEY INFECT
//ANYONE ELSE. THEY NEED TO EITHER HEAL SLOWER OR SPREAD IT FASTER!

//GLOBAL PARAMETERS
float RADIUS_OF_INFECTION_SQUARED; //How close particles are to be infected. Square space is x, y {0:100}
//so theoretically values should go from 0 to 100^2
float PROBABILITY_OF_INFECTION; // Should be a float between 0 and 100
int NUMBER_OF_PARTICLES; //should it be divisible by 8? idk
int NUM_BOXES; //number of boxes to run things on.
float PROBABILITY_OF_JUMPING; // Should be a float between 0 and 100
float PROBABILITY_OF_CURE; //Should be a float between 0 and 100000
float PROBABILITY_OF_DEATH; //should be a float between 0 and 100000


int t; //time variable is global so it can be an extern

int main(int argc, char ** argv) {
	std::ofstream fout("serialData.csv");
	if(fout.fail())
	{
		std::cerr << "Could not open data file! Please close serialData.csv and try again" << std::endl;
		return -1;
	}
	std::cout << "Starting simulation" << std::endl;
	Timer timer(true);
    srand(time(nullptr));

    //initialize stuff, parameters are ALL_CAPS
    NUMBER_OF_PARTICLES = 1000;
    bool othersHaveBeenInfected = false;
    int personal_area_cnt = NUMBER_OF_PARTICLES / 2;
    int public_area_cnt = personal_area_cnt / 4;
    NUM_BOXES = 10;
    PROBABILITY_OF_JUMPING = 20;
    PROBABILITY_OF_INFECTION = 80;
    RADIUS_OF_INFECTION_SQUARED = 100;
    PROBABILITY_OF_CURE = 2;
    PROBABILITY_OF_DEATH = 1;
    int NUM_LOCATIONS_PEOPLE_VISIT = 10;

    //create initial particles before assigning them to their areas
    vector<particle> particles_temp;
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++)
    {
        particle p;
        particles_temp.push_back(p);
    }

    //create personal areas
    vector<area> personal_areas[NUM_BOXES];
    int j = 0;
    for (int i = 0; i < personal_area_cnt; i++)
    {
        area a;
        personal_areas[j].push_back(a);
        j++;
        j %= NUM_BOXES;
    }

    //create public areas
    vector<area> public_areas[NUM_BOXES];
    j = 0;
    for (int i = 0; i < public_area_cnt; i++)
    {
        area a;
        public_areas[j].push_back(a);
        // Give k (10) particles the ability to go to this public area.
        for (int k = 0; k < NUM_LOCATIONS_PEOPLE_VISIT; k++)
        {
            int random_person = rand() % NUMBER_OF_PARTICLES;
            particles_temp[random_person].jumpLocationsAndChance.insert(
                    make_pair(make_tuple(j, public_areas[j].size()-1, Public), PROBABILITY_OF_JUMPING));
        }
        j++;
        j %= NUM_BOXES;
    }

    //someone has to start infected!
    particles_temp[rand() % NUMBER_OF_PARTICLES].state = Infected;

    //assign the particles to personal areas
    int k = 0;
    for (int i = 0; i < NUM_BOXES; i++)
    {
        for (int j = 0; j < personal_areas[i].size(); j++)
        {
            //2 particles per personal area
            particles_temp[k].jumpLocationsAndChance.insert(
                    make_pair(make_tuple(i, j, Personal), PROBABILITY_OF_JUMPING));
            personal_areas[i][j].particles.push_back(particles_temp[k++]);
            particles_temp[k].jumpLocationsAndChance.insert(
                    make_pair(make_tuple(i, j, Personal), PROBABILITY_OF_JUMPING));
            personal_areas[i][j].particles.push_back(particles_temp[k++]);
        }
    }

    //main loop
    map<particle, tuple<int, int, AreaTypes>> outgoingParticles;
    vector<tuple<int, int, int, int, int>> data;
    for (t = 0; t < 1000; t++)
    {
        int totalSus = 0, totalInf = 0, totalRec = 0, totalDec = 0;

        //this will implement a lockdown mode
        if (t == 10)
        {
            PROBABILITY_OF_JUMPING = 1;
            //todo: people should also be sent back to their personal areas. ugh im lazy tho
        }

        //process personal areas
        for (int i = 0; i < NUM_BOXES; i++)
        {
            for (int j = 0; j < personal_areas[i].size(); j++)
            {
                personal_areas[i][j].processArea();
                totalSus += personal_areas[i][j].numSus;
                totalInf += personal_areas[i][j].numInf;
                totalRec += personal_areas[i][j].numRec;
                totalDec += personal_areas[i][j].numDec;
                outgoingParticles.insert(personal_areas[i][j].outgoingParticles.begin(),
                        personal_areas[i][j].outgoingParticles.end());
            }
        }

        //process public areas
        for (int i = 0; i < NUM_BOXES; i++)
        {
            for (int j = 0; j < public_areas[i].size(); j++)
            {
                public_areas[i][j].processArea();
                totalSus += public_areas[i][j].numSus;
                totalInf += public_areas[i][j].numInf;
                totalRec += public_areas[i][j].numRec;
                totalDec += public_areas[i][j].numDec;
                outgoingParticles.insert(public_areas[i][j].outgoingParticles.begin(),
                                         public_areas[i][j].outgoingParticles.end());
            }
        }

        //send outgoing particles where they need to go
        for (const auto& part : outgoingParticles)
        {
            if (get<2>(part.second) == Public)
            {
                public_areas[get<0>(part.second)][get<1>(part.second)].particles.push_back(part.first);
            }
            else
            {
                personal_areas[get<0>(part.second)][get<1>(part.second)].particles.push_back(part.first);
            }
        }
        outgoingParticles.clear();

        data.emplace_back(t, totalSus, totalInf, totalRec, totalDec);

        //Debugging
        cout << "Time " << t << ": Susceptible- " << totalSus << ", Infected- " << totalInf << ", Recovered- " <<
        totalRec << ", Deceased- " << totalDec << endl;

        //End the simulation because no one was infected
		if(totalInf > 1 and ! othersHaveBeenInfected)
			othersHaveBeenInfected = true;
		else if(totalInf == 0 and !othersHaveBeenInfected)
		{
			cout << "A pandemic did not occur! No one else was infected. Ending simulation..." << std::endl;
			std::chrono::seconds dura( 2);
			std::this_thread::sleep_for( dura );
			main(argc, argv);
			return -2;
		}
    }
    std::cout << "Elapsed time: " << timer.getElapsedTime() << std::endl;
	fout << "Time, Susceptible, Infected, Recovered, Deceased" << std::endl;
	for(auto point: data)
	{
//			cout << "Time " << get<0>(point) << ": Susceptible- " << get<1>(point) << ", Infected- " << get<2>(point) << ", Recovered- " << get<3>(point) << ", Deceased- " << get<4>(point) << endl;
		fout << get<0>(point) << ", " << get<1>(point) << ", " << get<2>(point) << ", " << get<3>(point) << ", " << get<4>(point) << std::endl;
	}
	fout.close();

    return 0;
}
