//
// Created by derek on 4/30/20.
//

#ifndef CORONA_PARTICLE_H
#define CORONA_PARTICLE_H

#include <map>
#include <utility>
#include <cmath>
extern float RADIUS_OF_INFECTION_SQUARED;
extern float PROBABILITY_OF_INFECTION;

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
    map<tuple<int, int, AreaTypes>, float> jumpLocationsAndChance;
    bool jumping;
    tuple<int, int, AreaTypes> jumpLocation;

    particle()
    {
        state = Susceptible;

    }
    //processes the particle at each time step
    void process(const vector<particle>& particlesInArea)
    {
        //todo: first move position.

        //now, transmit the disease
        switch (state)
        {
            case Susceptible:
            {
                for (const auto& nearby : particlesInArea)
                {
                    //transmitting the disease
                    if (nearby.state == Infected &&
                    dist_squared(nearby) <= RADIUS_OF_INFECTION_SQUARED &&
                    rand() % 100 < PROBABILITY_OF_INFECTION) {
                        state = Infected;
                        break;
                    }
                }
                break;
            }
            case Infected:
            {
                //todo: probability of transitioning to recovered or deceased
                break;
            }
            case Recovered:
                break;
            case Deceased:
                break;
        }

        //todo: finally, check to see if jumping
    }

    float dist_squared(particle other) {
        float dx = other.x - x;
        float dy = other.y - y;
        return dx * dx + dy * dy;
    }
};


#endif //CORONA_PARTICLE_H
