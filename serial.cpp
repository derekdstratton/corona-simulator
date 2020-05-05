//
// Created by derek on 4/30/20.
//

#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>

#include "area.h"

using namespace std;

//GLOBAL PARAMETERS
float RADIUS_OF_INFECTION_SQUARED; //
float PROBABILITY_OF_INFECTION; // Should be a float between 0 and 100
int NUMBER_OF_PARTICLES; //should be divisible by 8
int NUM_BOXES; //number of boxes to run things on.
float PROBABILITY_OF_JUMPING; // Should be a float between 0 and 100

int main(int argc, char ** argv) {
    srand(time(nullptr));

    //initialize stuff
    NUMBER_OF_PARTICLES = 1000;
    int personal_area_cnt = NUMBER_OF_PARTICLES / 2;
    int public_area_cnt = personal_area_cnt / 4;
    NUM_BOXES = 10;
    PROBABILITY_OF_JUMPING = 20;
    PROBABILITY_OF_INFECTION = 20;
    RADIUS_OF_INFECTION_SQUARED = 25;

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
//    j = 0;
//    for (int i = 0; i < public_area_cnt; i++)
//    {
//        area a;
//        public_areas[j].push_back(a);
//        // Give k (10) particles the ability to go to this public area.
//        for (int k = 0; k < 10; k++)
//        {
//            int random_person = rand() % NUMBER_OF_PARTICLES;
////            particles_temp[random_person].jumpLocationsAndChance.insert(
////                    make_pair(make_tuple(j, public_areas[j].size()-1, Public), PROBABILITY_OF_JUMPING));
//        }
//        j++;
//        j %= NUM_BOXES;
//    }

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
    map<tuple<int, int, AreaTypes>, particle> outgoingParticles;
    for (int t = 0; t < 1000; t++)
    {
        int totalSus = 0, totalInf = 0, totalRec = 0, totalDec = 0;
        //process personal areas
        for (int i = 0; i < NUM_BOXES; i++)
        {
            for (int j = 0; j < personal_areas[i].size(); j++)
            {
                personal_areas[i][j].processArea();
                totalSus += personal_areas[i][j].numSus;
                totalInf += personal_areas[i][j].numInf;
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
                outgoingParticles.insert(public_areas[i][j].outgoingParticles.begin(),
                                         public_areas[i][j].outgoingParticles.end());
            }
        }

        int lol1 = outgoingParticles.size();
        int lol2 = 0;

        //send outgoing particles where they need to go
        for (const auto& part : outgoingParticles)
        {
            if (get<2>(part.first) == Public)
            {
                public_areas[get<0>(part.first)][get<1>(part.first)].particles.push_back(part.second);
            }
            else
            {
                lol2++;
                personal_areas[get<0>(part.first)][get<1>(part.first)].particles.push_back(part.second);
//                cout << get<0>(part.first) << " " << get<1>(part.first) << endl;
            }
        }

        outgoingParticles.clear();

        //Debugging or data
        cout << "Time " << t << ": Suscpetible- " << totalSus << ", Infected- " << totalInf << endl;
    }

    return 0;
}
