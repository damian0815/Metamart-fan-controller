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
	
}

//--------------------------------------------------------------
void testApp::update(){
	

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

	char buf[1024];
	sprintf( buf, "OSC %s:%i", osc_host.c_str(), osc_port );
	ofSetHexColor( 0x000000 );
	ofDrawBitmapString( buf, 10, ofGetHeight()-13 );
	ofSetHexColor( 0xffffff );
	ofDrawBitmapString( buf, 10+1, ofGetHeight()-13+1 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
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
