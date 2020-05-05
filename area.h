//
// Created by derek on 4/30/20.
//

#ifndef CORONA_AREA_H
#define CORONA_AREA_H

enum AreaTypes {Public, Personal};

#include <vector>
#include "particle.h"

using namespace std;

class area {
    public:
    vector<particle> particles;
    AreaTypes type;
    //particles that are jumping are temporarily stored here.
    map<tuple<int, int, AreaTypes>, particle> outgoingParticles;
    int numSus, numInf, numRec, numDec;

    //called at each time step
    void processArea() {
        outgoingParticles.clear();
        numSus = 0, numInf = 0, numRec = 0, numDec = 0;
        const vector<particle> origParticles = particles;
        for (auto it = particles.begin(); it != particles.end(); it++) {
            it->process(origParticles);
            switch(it->state)
            {
                case Susceptible:
                {
                    numSus++;
                    break;
                }
                case Infected:
                {
                    numInf++;
                    break;
                }
                case Recovered:
                {
                    numRec++;
                    break;
                }
                case Deceased:
                {
                    numDec++;
                    break;
                }
            }
            if (it->jumping) {
                outgoingParticles.insert(pair<tuple<int, int, AreaTypes>, particle>(it->jumpLocation, *it));
                particles.erase(it);
                it--;
            }
        }
    }
};


#endif //CORONA_AREA_H
