//
//  Fans.cpp
//  fan-controller
//
//  Created by Damian Stewart on 11/24/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Fans.h"
#include "ofMain.h"
#include <assert.h>
#include "ofxXmlSettings.h"


void Fans::setup()
{
#if defined TARGET_OSX
    serial.setup( "/dev/tty.usbserial-A5001abn", 9600 );
#elif defined TARGET_LINUX
    serial.setup( "/dev/ttyUSB0", 9600 );
#endif
    
    active_fan_indices.clear();
    for ( int i=0; i<NUM_FANS; i++ )
    {
        fans[i] = false;
        active_fan_indices.push_back( i );
    }
}

void Fans::load()
{
    ofxXmlSettings data;
    data.loadFile("fans.xml");
    data.pushTag("fans");
    int count = data.getNumTags( "fan" );
    assert( count == NUM_FANS && "wrong number of fans in xml" );
    active_fan_indices.clear();
    for ( int i=0; i<count; i++ )
    {
        data.pushTag("fan", i );
        if ( data.getValue( "active", 1 ) )
            active_fan_indices.push_back(i);
        angles[i] = data.getValue("angle", (i*(360.0f/count)) );
        radius[i] = data.getValue("radius", 0.5f );
        height[i] = data.getValue("height", 0.5f );
        data.popTag();
    }
}

void Fans::save()
{

    ofxXmlSettings data;
    data.addTag("fans");
    data.pushTag("fans");
    for ( int i=0; i<NUM_FANS; i++ )
    {
        data.addTag("fan");
        data.pushTag("fan", i );
        data.addValue("active", std::find(active_fan_indices.begin(), active_fan_indices.end(), i) != active_fan_indices.end() );
        data.addValue("angle", angles[i] );
        data.addValue("radius", radius[i] );
        data.addValue("height", height[i] );
        data.popTag();
    }
    data.popTag();
    
    data.saveFile( "fans.xml" );
}

bool Fans::get( int index )
{

    if ( index >= 0 && index < getNumFans() )
        return fans[active_fan_indices[index]];
    else 
        return false;
}

void Fans::set( int index, bool val )
{
    if ( index >= 0 && index < getNumFans() )
        fans[active_fan_indices[index]] = val;
}

void Fans::update()
{

}


void Fans::draw()
{

    float x = 10;
    float y = 10;
    float size = 10;
    for ( int i=0; i<NUM_FANS; i++ )
    {
        if ( fans[i] )
            ofFill();
        else
            ofNoFill();
        ofRect( x, y, size, size );
        x += 10;
        if ( x+size >= ofGetWidth() )
        {
            x = 10;
            y += 10+size;
        }
        else
            x += size;
    }
    

}


void Fans::send()
{
    // 
    static const int num_fan_bytes = NUM_FANS/8;
    unsigned char buffer[num_fan_bytes+1];
    // header char is 'U'
    buffer[0] = 'U'; // 0x55 == 0b01010101
    for ( int i=0; i<num_fan_bytes; i++ )
    {
        unsigned char byte = 0;
        for ( int j=0; j<8; j++ )
        {
            int which_fan = i*8+j;
            if ( fans[which_fan] )
            {
                byte |= (0b00000001 << j );
            }
        }
        buffer[i+1] = byte;
    }
    
    // write to the board
    serial.writeBytes(buffer, num_fan_bytes+1);
}






