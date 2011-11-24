//
//  Fans.h
//  fan-controller
//
//  Created by Damian Stewart on 11/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef fan_controller_Fans_h
#define fan_controller_Fans_h

#include "ofMain.h"

static const int NUM_FANS=8*2*8;



class Fans
{
public:
    
    void setup();
    void load();
    void save();
    
    int getNumFans() { return active_fan_indices.size(); }
    bool get( int index );
    void set( int index, bool val );
    
    void update();
    void draw();

    
    void send();

private:
    
    
    ofSerial serial;
    

    vector<int> active_fan_indices;
    bool fans[NUM_FANS];
    
    float angles[NUM_FANS];
    float radius[NUM_FANS];
    float height[NUM_FANS];
    
    
};


#endif
