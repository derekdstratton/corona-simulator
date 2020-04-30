//
// Created by derek on 4/30/20.
//

#ifndef CORONA_PARTICLE_H
#define CORONA_PARTICLE_H

#include <map>
#include <utility>

using namespace std;

enum states {
    Susceptible,
    Infected,
    Recovered,
    Deceased
};

class particle {
    public:
    states state;
    float x, y;
    map<pair<int, int>, float> jumpLocationsAndChance;
    bool jumping;
    pair<int, int> jumpLocation;
    //processes the particle at each time step
    void process(const vector<particle>& particlesInArea)
    {
        //todo: first move position.

        //now, transmit the disease
        switch (state)
        {
            case Susceptible:
            {
                for (auto nearby : particlesInArea)
                {
                    //transmit
                }
            }
            case Infected:
                break;
            case Recovered:
                break;
            case Deceased:
                break;
        }

        //todo: finally, check to see if jumping
    }
};


#endif //CORONA_PARTICLE_H
