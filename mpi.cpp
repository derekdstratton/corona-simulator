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
#include <mpi.h>
#include "omp.h"
#include "area.h"
#include "Timer.h"

using namespace std;

//NOTE: SINCE THE SIM STARTS WITH 1 INFECTED INDIVIDUAL, THEY MIGHT DIE OR RECOVER BEFORE THEY INFECT
//ANYONE ELSE. THEY NEED TO EITHER HEAL SLOWER OR SPREAD IT FASTER
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
cout << omp_get_max_threads() << endl;
#pragma omp parallel 
{
	cout << omp_get_thread_num() << endl;
}
    std::ofstream fout("serialData.csv");
    if(fout.fail())
    {
        std::cerr << "Could not open data file! Please close serialData.csv and try again" << std::endl;
        return -1;
    }
    std::cout << "Starting simulation" << std::endl;
    Timer timer(true);
    srand(time(nullptr));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &NUM_BOXES);

    int RANK;
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);

    MPI_Datatype MPI_SIGNAL;
    MPI_Type_contiguous(7, MPI_INT, &MPI_SIGNAL);
    MPI_Type_commit(&MPI_SIGNAL);

    //initialize stuff, parameters are ALL_CAPS
    NUMBER_OF_PARTICLES = 100000;
    bool othersHaveBeenInfected = false;
    int personal_area_cnt = NUMBER_OF_PARTICLES / 2;
    int public_area_cnt = personal_area_cnt / 4;
//    NUM_BOXES = 10;
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

    struct signal {
        int a, b, c, d, e, f, g;
    };

    vector<signal> mpiSignal;

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
        int i = RANK;
//        for (int i = 0; i < NUM_BOXES; i++)
//        {
#pragma omp for
            for (int j = 0; j < personal_areas[i].size(); j++)
            {
                personal_areas[i][j].processArea();
                totalSus += personal_areas[i][j].numSus;
                totalInf += personal_areas[i][j].numInf;
                totalRec += personal_areas[i][j].numRec;
                totalDec += personal_areas[i][j].numDec;
//                outgoingParticles.insert(personal_areas[i][j].outgoingParticles.begin(),
//                                         personal_areas[i][j].outgoingParticles.end());
                for (auto part : personal_areas[i][j].outgoingParticles)
                {

                    auto tup = make_tuple(i, j, 1, get<0>(part.second), part.first.id, get<1>(part.second),
                            get<2>(part.second) == Personal ? 1 : 2);
                    signal stru{};
                    stru.a = i;
                    stru.b = j;
                    stru.c = 1;
                    stru.d = part.first.id;
                    stru.e = get<0>(part.second);
                    stru.f = get<1>(part.second);
                    stru.g = get<2>(part.second) == Personal ? 1 : 2;
                    mpiSignal.push_back(stru);
                }
            }
//        }

//        //process public areas
//        for (int i = 0; i < NUM_BOXES; i++)
//        {
#pragma omp for
            for (int j = 0; j < public_areas[i].size(); j++)
            {
                public_areas[i][j].processArea();
                totalSus += public_areas[i][j].numSus;
                totalInf += public_areas[i][j].numInf;
                totalRec += public_areas[i][j].numRec;
                totalDec += public_areas[i][j].numDec;
//                outgoingParticles.insert(public_areas[i][j].outgoingParticles.begin(),
//                                         public_areas[i][j].outgoingParticles.end());
                for (auto part : public_areas[i][j].outgoingParticles)
                {
                    auto tup = make_tuple(i, j, 2, part.first.id, get<0>(part.second), get<1>(part.second),
                                          get<2>(part.second) == Personal ? 1 : 2);
                    signal stru{};
                    stru.a = i;
                    stru.b = j;
                    stru.c = 2;
                    stru.d = part.first.id;
                    stru.e = get<0>(part.second);
                    stru.f = get<1>(part.second);
                    stru.g = get<2>(part.second) == Personal ? 1 : 2;
                    mpiSignal.push_back(stru);
                }
            }
//        }
//cout << "MPI SIGNALS: " << mpiSignal.size() << endl;

        //todo: send mpiSignals to everyone else
        for (int r = 0; r < NUM_BOXES; r++)
        {
            if (RANK == r)
            {
                for (int r2 = 0; r2 < NUM_BOXES; r2++) {
                    int size = mpiSignal.size();
//                    cout << "1- t: " << t << ", rank: " << r << ", size: " << size << endl;
                    MPI_Send(&size, 1, MPI_INT, r2, 1, MPI_COMM_WORLD);
//                    cout << "2- t: " << t << ", rank: " << r << ", size: " << size << endl;
                    if (size > 0) {
//                        cout << "3- t: " << t << ", rank: " << r << ", size: " << size << endl;
                        MPI_Send(&mpiSignal[0], size, MPI_SIGNAL, r2, 1, MPI_COMM_WORLD);
//                        cout << "4- t: " << t << ", rank: " << r << ", size: " << size << endl;
                    }
                }

            }
            MPI_Status status;
//            else //this should be fine as long as sending to self is fine.
            {
                int size;
//                cout << "5- t: " << t << ", rank: " << r << ", size: " << size << endl;
                MPI_Recv(&size, 1, MPI_INT, r, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//                cout << "6- t: " << t << ", rank: " << r << ", size: " << size << endl;
                vector<signal> incomingSignal;
                incomingSignal.resize(size);
                if (size > 0) {
//                    cout << "7- t: " << t << ", rank: " << r << ", size: " << size << endl;
                    MPI_Recv(&incomingSignal[0], size, MPI_SIGNAL, r, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//                    cout << "8- t: " << t << ", rank: " << r << ", size: " << size << endl;
                }
                //todo: move this outside for loop for efficiency?
                for (auto stru : incomingSignal)
                {
//                    cout << "signal: " << stru.a << ", " << stru.b << ", " << stru.c << ", " << stru.d << ", " <<
//                    stru.e << ", " << stru.f << ", " << stru.g << endl;
                    tuple<int, int, int, int, int, int, int> sig = make_tuple(stru.a, stru.b, stru.c,
                    stru.d, stru.e, stru.f, stru.g);
                    if (get<2>(sig) == 2) { //public
//                        cout << "ok" << endl;
//                        auto parts = public_areas[get<0>(sig)][get<1>(sig)].particles;
                        for (auto it = public_areas[get<0>(sig)][get<1>(sig)].particles.begin();
                            it != public_areas[get<0>(sig)][get<1>(sig)].particles.end();
                            it++)
                        {
//                            cout << part.id << endl;
                            if (it->id == get<3>(sig))
                            {
//                                cout << "boomer" << endl;
                                outgoingParticles.insert(make_pair(*it,
                                        make_tuple(get<4>(sig), get<5>(sig), get<6>(sig) == 1 ? Personal : Public)));
                                public_areas[get<0>(sig)][get<1>(sig)].particles.erase(it);
                                it--;
                            }
                        }
                    } else {
//                        cout << "pasta" << endl;
//                        auto parts = personal_areas[get<0>(sig)][get<1>(sig)].particles;
                        for (auto it = personal_areas[get<0>(sig)][get<1>(sig)].particles.begin();
                             it != personal_areas[get<0>(sig)][get<1>(sig)].particles.end();
                             it++)
                        {
//                            cout << part.id << endl;
                            if (it->id == get<3>(sig))
                            {
//                                cout << "roni" <<endl;
                                outgoingParticles.insert(make_pair(*it,
                                                                   make_tuple(get<4>(sig), get<5>(sig), get<6>(sig) == 1 ? Personal : Public)));
                                personal_areas[get<0>(sig)][get<1>(sig)].particles.erase(it);
                                it--;
                            }
                        }
                    }
                }
                incomingSignal.clear();
            }
        }
//        cout << "MPI sigs done sending" << endl;
        mpiSignal.clear();

//        cout << "OUTGONG PARTS: " << outgoingParticles.size() << endl;

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

        //todo: send data back to rank 0 to process

        data.emplace_back(t, totalSus, totalInf, totalRec, totalDec);

        //Debugging
        cout << "Time " << t << ": Susceptible- " << totalSus << ", Infected- " << totalInf << ", Recovered- " <<
             totalRec << ", Deceased- " << totalDec << endl;

        //End the simulation because no one was infected
//        if(totalInf > 1 and ! othersHaveBeenInfected)
//            othersHaveBeenInfected = true;
//        else if(totalInf == 0 and !othersHaveBeenInfected)
//        {
//            cout << "A pandemic did not occur! No one else was infected. Ending simulation..." << std::endl;
//            std::chrono::seconds dura( 2);
//            std::this_thread::sleep_for( dura );
//            main(argc, argv);
//            return -2;
//        }
    }
    std::cout << "Elapsed time: " << timer.getElapsedTime() << std::endl;
//    fout << "Time, Susceptible, Infected, Recovered, Deceased" << std::endl;
//    for(auto point: data)
//    {
////			cout << "Time " << get<0>(point) << ": Susceptible- " << get<1>(point) << ", Infected- " << get<2>(point) << ", Recovered- " << get<3>(point) << ", Deceased- " << get<4>(point) << endl;
//        fout << get<0>(point) << ", " << get<1>(point) << ", " << get<2>(point) << ", " << get<3>(point) << ", " << get<4>(point) << std::endl;
//    }
//    fout.close();

    return 0;
}
