//
//  sampleCell.h
//  traffic_lights_rtsp
//
//  Created by Jakob Marsico on 3/18/14.
//
//

#ifndef __traffic_lights_rtsp__sampleCell__
#define __traffic_lights_rtsp__sampleCell__

#include <iostream>
#include "ofMain.h"

class sampleCell{
public:
    vector <ofPoint> p;
    //p0, p1, p2, p3, tempPoint;
    ofPoint tempPoint;
    vector <ofPoint> pixIn;
    int redVal;
    ofPolyline shape;
    int ID;
    
    sampleCell();
    
    void init(int _ID);
    
    void setPoints(ofPoint oldP0, ofPoint oldP1, const ofPixels &_pix, ofPoint _startPoint);
    void setPointsFirst(const ofPixels &_pix, ofPoint _startPoint);
    void draw(int alphaCoeff);
    void reset();
    bool isPointsSet();
    bool isSettingPoints();
    void addPoint();
    void setPixIn(const ofPixels &_pix);
    int getCellBrightness(const ofPixels &_pix);
    int getAverageBrightness(int _numSamples);
    
    ofColor getCellColor(const ofPixels &_pix);
    ofColor getAverageColor(int _numSamples);
    
    
    int total;
    ofPixels pix;
    int startX;
    int startY;
    bool bSettingPoints;
    
    void getPixLocations();
    
    void mouseMoved(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    bool bIsSet;
    
    
protected:
    
    bool bRegisteredEvents;
    
    int increment;
    int brightness;
    ofColor cellColor;
    int alpha;
    bool secondSet;
    
    vector <int> average;
    
    int avgCounter;
    int runningTotal;
    int numSamples;
    
    
};



#endif /* defined(__traffic_lights_rtsp__sampleCell__) */
