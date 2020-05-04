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

    //called at each time step
    void processArea() {
        const vector<particle> origParticles;
        for (auto it = particles.begin(); it != particles.end(); it++) {
            it->process();
            if (it->jumping) {
                outgoingParticles.insert(pair<tuple<int, int, AreaTypes>, particle>(it->jumpLocation, *it));
                particles.erase(it);
                it--;
            }
        }
    }
};


#endif //CORONA_AREA_H
