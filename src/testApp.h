#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "Fans.h"

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
		
	
	
	string osc_host;
	int osc_port;
	
	ofxOscSender osc_sender;
    
    
    Fans fans;

    int selected_fan;
    
    
    string status;
    float timer;
    
    
};
