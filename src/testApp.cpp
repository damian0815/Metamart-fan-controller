#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate( 25.0f );

	ofSetLogLevel( OF_LOG_NOTICE );

	cams[0].listDevices();
	
	for ( int i=0; i<2; i++ )
	{
		ofLog(OF_LOG_NOTICE, "*** initing camera %i", i );
		cams[i].setDeviceID(i);
		inited[i] = cams[i].initGrabber( 640, 480 );
		if ( !inited[i] )
			ofLog( OF_LOG_ERROR, "*** couldn't open cam %i", i );
		else
		{
			ofLog(OF_LOG_NOTICE, "*** inited camera %i %fx%f", i, cams[i].getWidth(), cams[i].getHeight() );
	}
		
		
		max_act[i] = 0;
		min_act[i] = 0;
		low_quartile[i] = 0;
		high_quartile[i] = 255;
		snap_background[i] = true;
		out_act[i] = 0;
		ofLog(OF_LOG_NOTICE, "*** finished with camera %i", i );
	}
    for(int i=0; i<2; i++ )
    {
        if ( !inited[i] )
            continue;

        working[i].allocate( cams[i].getWidth(), cams[i].getHeight() );
        curr_8bit[i].allocate( cams[i].getWidth(), cams[i].getHeight() );
        diff_8bit[i].allocate( cams[i].getWidth(), cams[i].getHeight() );
        accum[i].allocate( cams[i].getWidth(), cams[i].getHeight() );
        //		accum_8bit[i].allocate( cams[i].getWidth(), cams[i].getHeight() );
    }	
    
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
	
	for ( int i=0; i<2; i++ )
	{
		if ( !inited[i] )
			continue;

		cams[i].update();
		
		if ( cams[i].isFrameNew() )
		{
			working[i].setFromPixels( cams[i].getPixelsRef() );

			curr_8bit[i] = working[i];
			if ( snap_background[i] && ofGetFrameNum()>10 )
			{
				accum[i] = curr_8bit[i];
				snap_background[i] = false;
			}
			
			diff_8bit[i] = accum[i];
			diff_8bit[i].absDiff(curr_8bit[i]);
			
			float accum_factor = 0.999f;
			accum[i].addWeighted( curr_8bit[i], 1.0f-accum_factor );

			
			
			//float* p = accum[i].getPixelsAsFloats();
			//float total = 0;

			// 
			//diff_8bit[i].brightnessContrast(1.0, 2.0f);
			/*//diff_8bit[i].adaptiveThreshold(16);
			unsigned char* pixels = diff_8bit[i].getPixels();
			int total = 0;
			for ( int j=0; j<num_pixels; j++ )
			{
				total += *(pixels++);
			}
			float average = total/num_pixels;
			low_quartile[i] = average/4;
			high_quartile[i] = average*3.0f/4;
		
			diff_8bit[i].threshold( high_quartile[i], false );
*/			
			// count up the activity -> act			
			unsigned char* pixels = diff_8bit[i].getPixels();
			int num_pixels = diff_8bit[i].getWidth()*diff_8bit[i].getHeight();
			int total = 0;
			for ( int j=0; j<num_pixels; j++ )
			{
				total += *(pixels++);
			}
			
			// if act > max_act then 
			//   out_act <- 1
			//   max_act <- max_act * 0.8f + act * 0.2f
			// if act < max_act then 
			//   out_act <- act/max_act
			//   max_act <- max_act * 0.99f
			if ( total > max_act[i] )
			{
				max_act[i] = total*1.2f;
			}
			else
			{
				max_act[i] = max_act[i]*0.990f + total*1.2f*0.001f;
			}
			
			if ( total < min_act[i] )
			{	
				min_act[i] = total/1.2f;
			}
			else
			{
				min_act[i] = min_act[i]*0.999f + (total/1.2f)*0.001f;
			}
			
			
			// calculate 
			out_act[i] = ofMap( total, min(min_act[i],max_act[i]), max(min_act[i],max_act[i]), 
				  0.0f, 1.0f, true);
			
		}
		
	}

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

}

//--------------------------------------------------------------
void testApp::draw(){
	for ( int i=0; i<2; i++ )
	{
		if ( !inited[i] )
			continue;
		ofSetHexColor( 0xffffff );
		float half_screen_w = ofGetWidth()/2;
		float scale = half_screen_w/cams[i].getWidth();
		float w = cams[i].getWidth()*scale; // == half_screen_w
		float h = cams[i].getHeight()*scale;
		
		cams[i].draw( w*i, 0, w, h );
		diff_8bit[i].draw( w*i, h, w, h );
		
		char buf[1024];
		sprintf(buf, "min %5.0f, max %5.0f, out %5.3f", min_act[i], max_act[i], out_act[i] );
		ofSetHexColor( 0x000000 );
		ofDrawBitmapString( buf, i*w, 13 );
		ofSetHexColor( 0xffffff );
		ofDrawBitmapString( buf, i*w+1, 13+1 );
	

		
		
	}

	char buf[1024];
	sprintf( buf, "OSC %s:%i", osc_host.c_str(), osc_port );
	ofSetHexColor( 0x000000 );
	ofDrawBitmapString( buf, 10, ofGetHeight()-13 );
	ofSetHexColor( 0xffffff );
	ofDrawBitmapString( buf, 10+1, ofGetHeight()-13+1 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if ( key == 'b' )
	{
		snap_background[0] = true;
		snap_background[1] = true;
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
