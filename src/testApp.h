#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
	
	ofVideoGrabber cams[2];
	bool inited[2];
	
	ofxCvColorImage working[2];

	
	ofxCvFloatImage accum[2];
	
	ofxCvGrayscaleImage diff_8bit[2];
	ofxCvGrayscaleImage curr_8bit[2];
	
	
	float max_act[2];
	float min_act[2];
	
	float out_act[2];
	
	float low_quartile[2], high_quartile[2];
	
	bool snap_background[2];
	
	string osc_host;
	int osc_port;
	
	ofxOscSender osc_sender;
};
