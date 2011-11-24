#include "testApp.h"
#include "ofxXmlSettings.h"

static const int NUM_TRADE_BITS = 4;

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate( 25.0f );

	ofSetLogLevel( OF_LOG_NOTICE );

	ofxXmlSettings s;
	s.loadFile("settings.xml");
	s.pushTag("settings");
	osc_host = s.getValue("host", "localhost");
	osc_port = s.getValue("port", 3333 );
	
	ofLog( OF_LOG_NOTICE, "opening OSC to %s on port %i", osc_host.c_str(), osc_port );
	osc_sender.setup( osc_host, osc_port );
    
    fans.setup();
    selected_fan = -1;
    
    status = "";
    timer = 0.0f;
    
    price.resize(NUM_TRADE_BITS);
    amount.resize(NUM_TRADE_BITS);
    
    fans.send();
    
    osc_receiver.setup( 9090 );
    
    hrange_l = 0;
    hrange_h = 0;
    
    state = Trade;
    nextState();
}

//--------------------------------------------------------------
void testApp::update(){
	
    while ( osc_receiver.hasWaitingMessages() )
    {
        ofxOscMessage m;
        osc_receiver.getNextMessage(&m);
        
        ofLog(OF_LOG_VERBOSE, "got osc: address %s", m.getAddress().c_str() );
	
		for ( int i=0; i<price.size(); i++ )
		{
			char buf[1024];
			sprintf( buf, "/mtgox/trades/%i/price",i );
			if ( m.getAddress() == buf  )
				price[i] = m.getArgAsFloat(0);
			sprintf( buf, "/mtgox/trades/%i/amount",i );
			if ( m.getAddress() == buf  )
				amount[i] = m.getArgAsFloat(0);

	    }
	}
    
    switch ( state )
    {
        case Trade:
            doTradeBits();
            break;
        case Hash:
            doHashBits();
            break;
        default:
            break;
            
    }

    /*
    timer -= ofGetLastFrameTime();
    if ( timer < 0 )
    {
        timer += 1.0f;
        for ( int i=fans.getNumFans()-1; i>0; i-- )
        {
            fans.set( i, fans.get(i-1) );
        }
        fans.set(0, fans.get(fans.getNumFans()-1));
        
        //fans.setBase(selected_fan, (ofGetElapsedTimeMillis()%500)>250 );
        
        fans.send();
    }*/
    
    /*
    if ( hrange_l != hrange_h )
    {
        fans.illuminateHeightRange(min(hrange_l,hrange_h), max(hrange_l,hrange_h));
    }*/
    //fans.illuminateHeightRange( 0.0f, sinf(ofGetElapsedTimef())*0.5f+0.5f );

    fans.send();
    
    
    /*
	ofxOscMessage m;
	m.setAddress("/tracking/activity");
	float total_act = 0;
	int count = 0;
	for ( int i=0; i<2; i++) 
	{
		if ( inited[i] ){
			total_act += out_act[i];
			count++;
		}
	}
	if ( count == 0 )
		m.addFloatArg( 1.0f );
	else
		m.addFloatArg( total_act/count );
	osc_sender.sendMessage( m );
     */

}

//--------------------------------------------------------------
void testApp::draw(){
    
    fans.draw();

	char buf[1024];
	sprintf( buf, "OSC %s:%i  // %s", osc_host.c_str(), osc_port, status.c_str() );
	ofSetHexColor( 0x000000 );
	ofDrawBitmapString( buf, 10, ofGetHeight()-13 );
	ofSetHexColor( 0xffffff );
	ofDrawBitmapString( buf, 10+1, ofGetHeight()-13+1 );
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if ( key == '0' )
    {
        fans.setBase(selected_fan, false);
    }
    
    else if ( key == '1' )
    {
        fans.setBase(selected_fan, true);
    }
    
    
    else if ( key == 'W' )
    {   
        fans.save();
        status = "saved";
    }
    else if ( key == 'L' )
    {
        fans.load();
        status = "loaded";
    }
    
    
    else if ( key == OF_KEY_RIGHT )
    {
        selected_fan++;
        if ( selected_fan >= fans.getNumBaseFans() )
            selected_fan = 0;
        fans.selectFromBase( selected_fan );
        status = "selected base "+ofToString(selected_fan);
    }
    else if ( key == OF_KEY_LEFT )
    {
        selected_fan--;
        if ( selected_fan < 0 )
            selected_fan = fans.getNumBaseFans()-1;
        fans.selectFromBase( selected_fan );
        status = "selected base "+ofToString(selected_fan);
    }

    else if ( key == 'A' )
        fans.setActive( selected_fan, !fans.isActive(selected_fan) );
    
    else if ( key == 'l' )
    {
        hrange_l -= 0.1f;
    }
    else if ( key == 'L' )
    {
        hrange_l += 0.1f;
    }
    else if ( key == 'h' )
    {
        hrange_h += 0.1f;
    }
    else if ( key == 'H' )
    {
        hrange_h -= 0.1f; 
    }
    else
        fans.keyPressed( key );
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}




void testApp::nextState()
{
    switch( state )
    {
        case Trade:
            timer = 5.0f;
            intern_timer = 0.0f;
            state = Hash;
            break;
        case Hash:
            timer = 5.0f;
            intern_timer = 0.0f;
            state = Trade;
            break;
            
    }
    
}

void testApp::doHashBits()
{
    timer -= ofGetLastFrameTime();
    if ( timer < 0 )
    {
        nextState();
        return;
    }
    
    intern_timer -= ofGetLastFrameTime();
    printf("intern_timer %f\n", intern_timer );
    
    if ( intern_timer < 0 )
    {
        fans.setGroupOf8( ofRandom( 0, fans.getNumBaseFans()/8 )*8, (unsigned char)ofRandom( 0, 256 ) );
        float t = ofRandomuf();
        t *= t;
        t *= 0.5f;
        intern_timer += t;
    }
}


void testApp::doTradeBits()
{
    timer -= ofGetLastFrameTime();
    if ( timer < 0 ) 
    {
        nextState();
        return;
    }   

    float low_amount = amount[0];
	float low_price = price[0];
	float high_amount = low_amount;
	float high_price = low_price;
	for ( int i=1; i<amount.size(); i++ )
	{
		low_amount = min(low_amount, amount[i] );
		low_price = min (low_price, price[i] );
		high_amount = max(high_amount, amount[i] );
		high_price = max (high_price, price[i] );
	}
	float last_price = price.back();
	float last_amount = amount.back();

    float target_h_l = (last_amount-low_amount)/(high_amount-low_amount);
    float target_h_h = (last_price-low_price)/(high_price-low_price);
    
    hrange_h = hrange_h*0.99f + target_h_h*0.01f;
    hrange_l = hrange_l*0.99f + target_h_l*0.01f;
    fans.illuminateHeightRange( hrange_l, hrange_h );
    
}



