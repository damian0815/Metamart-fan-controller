#include "testApp.h"
#include "ofxXmlSettings.h"

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
    
    fans.send();
    
    osc_receiver.setup( 9090 );
    
    hrange_l = 0;
    hrange_h = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	
    while ( osc_receiver.hasWaitingMessages() )
    {
        ofxOscMessage m;
        osc_receiver.getNextMessage(&m);
        
        ofLog(OF_LOG_VERBOSE, "got osc: address %s", m.getAddress().c_str() );
		amount.resize(4);
 		price.resize(4);		
		for ( int i=0; i<4; i++ )
		{
			char buf[1024];
			sprintf( buf, "/mtgox/trades/%i/price" );
			if ( m.getAddress() == buf  )
				price[i] = m.getFloatArg(0);
			sprintf( buf, "/mtgox/trades/%i/amount" );
			if ( m.getAddress() == buf  )
				amount[i] = m.getFloatArg(0);

	    }
	}
	low_amount = amount[0];
	low_price = price[0];
	high_amount = low_amount;
	high_price = low_price;
	for ( int i=1; i<amount.size(); i++ )
	{
		low_amount = min(low_amount, amount[i] );
		low_price = min (low_price, price[i] );
		high_amount = max(high_amount, amount[i] );
		high_price = max (high_price, price[i] );
	}
	last_price = price.back();
	last_amount = amount.back();

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
    fans.illuminateHeightRange( 0.0f, sinf(ofGetElapsedTimef())*0.5f+0.5f );

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








