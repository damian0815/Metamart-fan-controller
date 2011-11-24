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
    
    int getNumFans() { return getNumActiveFans(); }
    int getNumActiveFans() { return active_fan_indices.size(); }
    bool get( int index );
    void set( int index, bool val );
    
    int getNumBaseFans() { return NUM_FANS; }
    int selectFromBase( int index ) { selected_base = index; }
    void setBase( int index, bool val ) { fans[index] = val; }
    
    void setGroupOf8( int which_base_start_id, unsigned char byte );
    
    void update();
    void draw();

    
    /// turn on all the fans between low and high (0..1)
    void illuminateHeightRange( float low, float high );
    
    
    void setActive( int which_fan_base, bool active );
    bool isActive( int which_fan_base ) { 
        return std::find(active_fan_indices.begin(), active_fan_indices.end(), which_fan_base) 
        != active_fan_indices.end(); 
    };
    

    void keyPressed( int key );
    
    void send();

private:
    
    
    
    ofSerial serial;
    

    int selected_base;
    
    vector<int> active_fan_indices;
    bool fans[NUM_FANS];
    
    float angles[NUM_FANS];
    float radius[NUM_FANS];
    float height[NUM_FANS];
    
    
};


#endif
