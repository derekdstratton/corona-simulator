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
extern float PROBABILITY_OF_CURE;
extern float PROBABILITY_OF_DEATH;
extern float PROBABILITY_OF_JUMPING;
extern int t;

using namespace std;

enum states {
    Susceptible,
    Infected,
    Recovered,
    Deceased
};

long var = 0;

class particle {
    public:
    static int cnt;
    states state;
    float x, y;
    map<tuple<int, int, AreaTypes>, float> jumpLocationsAndChance;
    //ignore the 2nd parameter in this. i decided to just make JUMP CHANCE an extern global
    bool jumping;
    tuple<int, int, AreaTypes> jumpLocation;
    int id;
    int toi; //time of infection

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
        //first move position.
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
                        toi = t;
                        break;
                    }
                }
                break;
            }
            case Infected:
            {
                //probability of transitioning to recovered or deceased
                if (rand() % 10000 < PROBABILITY_OF_CURE + (t - toi)) {
                    state = Recovered;
                } else if (rand() % 10000 < PROBABILITY_OF_DEATH + (t - toi)) {
                    state = Deceased;
                }
                break;
            }
            case Recovered:
            {
                break;
            }
            case Deceased:
            {
                break;
            }
        }

        //finally, check to see if jumping
        for (auto item : jumpLocationsAndChance)
        {
            if (PROBABILITY_OF_JUMPING >= rand() % 100)
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
