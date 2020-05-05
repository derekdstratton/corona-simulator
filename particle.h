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

int var = 0;

class particle {
    public:
    static int cnt;
    states state;
    float x, y;
    map<tuple<int, int, AreaTypes>, float> jumpLocationsAndChance;
    bool jumping;
    tuple<int, int, AreaTypes> jumpLocation;
    int id;

    particle()
    {
        state = Susceptible;
        x = rand() % 100;
        y = rand() % 100;
        id = var++;

    }

    bool operator <( const particle &rhs ) const
    {
        return ( id < rhs.id );
    }

    //processes the particle at each time step
    void process(const vector<particle>& particlesInArea)
    {
        //todo: first move position.
        x += (rand() % 6) - 3.0;
        if (x < 0) {
            x = 0;
        } else if (x >= 100) {
            x = 100;
        }
        y += (rand() % 6) - 3.0;
        if (y < 0) {
            y = 0;
        } else if (y >= 100) {
            y = 100;
        }

        jumping = false;

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
        for (auto item : jumpLocationsAndChance)
        {
            if (item.second >= rand() % 100)
            {
                jumping = true;
                jumpLocation = item.first;
                break;
            }
        }
    }

    float dist_squared(particle other) {
        float dx = other.x - x;
        float dy = other.y - y;
        return dx * dx + dy * dy;
    }
};


#endif //CORONA_PARTICLE_H
