#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofEvents.h"
#include "sampleCell.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxGenericDmx.h"

#define DMX_DATA_LENGTH 513

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void loadCellsFromXml();
    void saveCellsToXml();
    void sendDMX();
    
    void writeColorstoXml();
    
    
    static const int numLEDs = 8;
    vector <int> brightVals;
    
    ofVideoPlayer vid1;
    ofImage vid1Pix;
    ofImage vidPix;
    int vidInc;
    
    ofPoint start;
    
    bool bIsSetting;
    bool bShowVideo;
   
    
    sampleCell cells[numLEDs];
    
    ofxXmlSettings pointsXML;
    ofxXmlSettings cellColors;
    
    ofxUDPManager udpConnection;
    
    
    DmxDevice* dmxInterface_;
    unsigned char dmxData_[DMX_DATA_LENGTH];
    
    
    
    ofxPanel gui;
    ofxButton bLoadCells;
    ofxButton bSaveCells;
    ofxButton reset;
    ofxButton cellReset;
    ofxToggle bReady;
    ofxToggle bLinkCells;
    ofxToggle bUseLocalVid;
    ofxFloatSlider lightAmp;
    ofxIntSlider avgAmt;
    ofParameter<float> frameRate;
    ofParameter<int> currentFrame;

};
