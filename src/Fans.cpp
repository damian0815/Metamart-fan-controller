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
        angles[i] = (i*(360.0f/NUM_FANS));
        radius[i] = 0.5f;
        height[i] = 0.5f;
    }
    
    load();
    
    selected_base = -1;
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
        data.addValue("active", isActive(i) );
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
        if ( selected_base == i )
            ofSetHexColor( 0xff0000 );
        else if ( isActive(i) )
            ofSetHexColor( 0xffffff );
        else 
            ofSetHexColor( 0x333333 );

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
    
    x = 10;
    y += 10+size;
    ofSetHexColor(0x000000);
    float h = y;
    ofLine( 0, h, ofGetWidth(), h );
    h += 300;
    ofLine( 0, h, ofGetWidth(), h );
    float step = ofGetWidth()/getNumActiveFans();
    for ( int i=0; i<getNumActiveFans(); i++ )
    {
        int active_index = active_fan_indices[i];
        if ( selected_base == active_index )
            ofSetHexColor( 0xff0000 );
        else
            ofSetHexColor( 0xffffff );
        
        if ( get(i) )
            ofFill();
        else
            ofNoFill();

        ofRect( x, y+(1.0f-height[active_index])*300, size, size );
        x += step;
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
//            int which_fan = (i/2)*16 + (2-(i%2))*8 + (7-j);
            int which_fan = (i/2)*16;
            which_fan += 16-((i%2)*8 + j);
            
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


void Fans::setActive( int which_fan_base, bool active ) 
{ 
    if ( which_fan_base < 0 || which_fan_base >= getNumBaseFans() )
        return;
    
    if ( isActive(which_fan_base) && !active )
    {
        active_fan_indices.erase( std::find(active_fan_indices.begin(), 
                                            active_fan_indices.end(), 
                                            which_fan_base ) );
    }
    else if ( !isActive(which_fan_base) && active )
    {
        active_fan_indices.push_back( which_fan_base );
        std::sort( active_fan_indices.begin(), active_fan_indices.end() );
    }
}
    
void Fans::illuminateHeightRange( float low, float high )
{
    for ( int i=0; i<getNumBaseFans(); i++ )
    {
        if ( height[i] >= low && height[i] <= high )
            fans[i] = true;
        else
            fans[i] = false;
    }
    send();
}


void Fans::keyPressed( int key )
{
    if ( selected_base < 0 || selected_base >= getNumBaseFans() )
        return;
    
    if ( key == OF_KEY_UP )
    {
        height[selected_base] = min(height[selected_base]+0.01f, 1.0f);
    }
    else if ( key == OF_KEY_DOWN )
    {
        height[selected_base] = max(height[selected_base]-0.01f, 0.0f);
    }
    
}

void Fans::setGroupOf8( int which_base_id_start, unsigned char byte )
{
    if ( which_base_id_start >= 0 && which_base_id_start < getNumBaseFans()-8 )
    {
        for ( int i=0; i<8; i++ )
        {
            fans[which_base_id_start+i] = (byte>>i)&0b00000001;
        }
    }
}

