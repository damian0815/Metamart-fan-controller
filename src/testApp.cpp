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
}

//--------------------------------------------------------------
void testApp::update(){
	
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
        fans.set(1,false);
        fans.set(0,false);
    }
    
    else if ( key == '1' )
    {
        fans.set(1,true);
        fans.set(0,true);
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
        fans.setBase( selected_fan, false );
        selected_fan++;
        if ( selected_fan >= fans.getNumBaseFans() )
            selected_fan = 0;
        fans.selectFromBase( selected_fan );
        status = "selected base "+ofToString(selected_fan);
    }
    else if ( key == OF_KEY_LEFT )
    {
        fans.setBase( selected_fan, false );
        selected_fan--;
        if ( selected_fan < 0 )
            selected_fan = fans.getNumBaseFans()-1;
        fans.selectFromBase( selected_fan );
        status = "selected base "+ofToString(selected_fan);
    }
    
    else if ( key == OF_KEY_RETURN )
    {
        fans.setActive( selected_fan, !fans.isActive(selected_fan) );
    }
    
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








