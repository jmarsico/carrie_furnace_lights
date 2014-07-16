#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(20);
    
    ////////set up the gui
    gui.setup();
    gui.add(bLoadCells.setup("Load Cells"));
    gui.add(bSaveCells.setup("Save Cells"));
    gui.add(cellReset.setup("Clear Cells"));
    gui.add(reset.setup("Reset Background"));
    gui.add(bReady.setup("Ready for Cells", false));
    gui.add(bLinkCells.setup("Link Cells", true));
    gui.add(lightAmp.setup("Gain", 1.5, 0.5, 10.0));
    gui.add(avgAmt.setup("Smoothing", 5, 1, 100));
    gui.add(frameRate.set("Framerate", 0));
    gui.add(currentFrame.set("Current Frame", 0));
    
    gui.setPosition(700, 0);
    gui.loadFromFile("settings.xml");
    
    
    ////////initialize the LED value vector
    for(int i = 0; i < numLEDs; i++)
    {
        brightVals.push_back(0);
    }
    ofLog() << "size of brightVals array: " << brightVals.size();
    
    
    
    vid1.loadMovie("redcar.mp4");
    vid1.play();
    vid1.setFrame(2);
    vid1.setLoopState(OF_LOOP_PALINDROME);
    ofLog() << "size: " << vid1.getWidth() << " " << vid1.getHeight();
    
    
    
    //////////initialize all the cells
    for(int i = 0; i < numLEDs; i++)
    {
        cells[i].init(i);
    }
    
    bIsSetting = true;
    bShowVideo = true;
    
    ////////set up XML for points access
    if(pointsXML.load("cellPoints.xml")) ofLog() << "XML loaded successfully";
    else ofLog() << "XML did not load, check data/ folder";
    
    ////////set up XML for color recording
    if(cellColors.load("cellColors.xml"))
    {
        ofLog() << "XML cell colors loaded";
        cellColors.clear();
    }
    else ofLog() << "XML for cell colors did not load!!!";
    
    
    //create the socket and set to send to 169.254.0.2:11999
	udpConnection.Create();
	udpConnection.Connect("169.254.0.2",11999);
	udpConnection.SetNonBlocking(true);
    
}


//--------------------------------------------------------------
void testApp::update(){
    frameRate = ofGetFrameRate();
    
    //load cells
    if(bSaveCells) saveCellsToXml();
    if(bLoadCells) loadCellsFromXml();
    
    //reset and re-init cells
    if(cellReset)
    {
        for(int i = 0; i < numLEDs; i ++)
        {
            cells[i].reset();
            cells[i].init(i);
            brightVals[i] = 0;
        }
        bReady = false;
    }

    vid1.update();
    currentFrame = vid1.getCurrentFrame();

    if(vid1.isFrameNew() && vid1.getWidth() > 200)
    {
        vidPix = vid1.getPixelsRef();
        vidPix.resize(vid1.getWidth(), vid1.getHeight());
    }
    else if (vid1.getWidth() < 200)
    {
        ofSetColor(255);
        ofDrawBitmapString("no data", 10,10);
    }
    
    
    
    //----------------- cells -------------------------
    if(bReady)
    {
        //if the first cell in the array is not set, start setting it also, wait for threshPix to have data
        if(!cells[0].isPointsSet() && !cells[0].isSettingPoints())
        {
            cells[0].setPointsFirst(vidPix, start);
        }
        
        //if the first cell is set, use the second setup function
        if(cells[0].isPointsSet())
        {
            //go through each cell, starting with the second
            for(int i = 1; i < numLEDs; i++)
            {
                //if the previous cell is set.
                //if this cell is not in the process of setting points
                //if this cell is not already set up
                if(cells[i-1].isPointsSet() && !cells[i].isSettingPoints() && !cells[i].isPointsSet())
                {
                    //set the first two points of this new cell to the last two points of previous cell
                    if(bLinkCells)
                    {
                        cells[i].setPoints(cells[i-1].p[2], cells[i-1].p[3], vidPix, start);
                    }
                    else
                    {
                        cells[i].setPointsFirst(vidPix, start);
                    }
                }
            }
        }
    }
    
    int averageAmount = avgAmt;
    
    //if a cell is set, go ahead and start getting its brightness or color
    for(int i = 0; i < numLEDs; i++)
    {
        if(cells[i].isPointsSet()){
            //cells[i].getCellBrightness(vidPix);
            //brightVals[i] = cells[i].getAverageBrightness(averageAmount);
            cells[i].setCellColor(vidPix);
        }
    }
    writeColorstoXml();
    //sendDMX();
}

//--------------------------------------------------------------
void testApp::draw(){
    //ofBackground(0);
    
    if(bShowVideo){
        gui.draw();
        ofSetColor(255);
        
        vidPix.draw(0,0);
        
        
        //draw the cells
        for(int i = 0; i < numLEDs; i++)
        {
            cells[i].draw(i);
        }
    }
    
    
    //draw boxes
    int boxSize = ofGetWidth() / (numLEDs + 1);
    ofPushMatrix();
        ofTranslate(boxSize/2, ofGetHeight()-boxSize - 20);
        for(int i = 0; i < numLEDs; i++)
        {
            ofPushMatrix();
                ofTranslate(boxSize*i, 0);
            
                ofSetColor(cells[i].getCellColor());
                ofRect(0,0,boxSize, boxSize);
                ofSetColor(255);
                ofDrawBitmapString(ofToString(i), 0,0);
            ofPopMatrix();
        }
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::writeColorstoXml(){
    
    cellColors.addTag("FRAME");
    cellColors.pushTag("FRAME");
    
    for(int i = 0; i<numLEDs; i++)
    {
        if(cells[i].isPointsSet())
        {
            cellColors.addTag("CELL");
            cellColors.pushTag("CELL", i);
            
            cellColors.setValue("R", cells[i].getCellColor().r);
            cellColors.setValue("G", cells[i].getCellColor().g);
            cellColors.setValue("B", cells[i].getCellColor().b);
            
            cellColors.popTag();
            cellColors.popTag();
        }
    }
    
    cellColors.popTag();
    
}


void testApp::exit(){
    cellColors.save("cellColors.xml");
}

//--------------------------------------------------------------
void testApp::loadCellsFromXml(){
   
    ofLog() << "Starting to Load cells from XML...";
 
    for(int i = 0; i < numLEDs; i++)
    {
        pointsXML.pushTag("CELL", i);
        cells[i].setPointsFirst(vidPix, start);
        
        for(int j = 0; j < 4; j++)
        {
            
            int x = pointsXML.getValue("PT:X", 0, j);
            int y = pointsXML.getValue("PT:Y", 0, j);
            
            //set it to the points of that cell and add to the polyline
            cells[i].tempPoint.x = x;
            cells[i].tempPoint.y = y;
            cells[i].addPoint();
            ofLogVerbose() << "loadFromXML: cell[" << i << "]: point.x = " << x;
            ofLogVerbose() << "loadFromXML: cell[" << i << "]: point.y = " << y;
        }
        
        cells[i].bSettingPoints = false;
        cells[i].bIsSet = true;
        cells[i].getPixLocations();
        pointsXML.popTag();
    }
}

//--------------------------------------------------------------
void testApp::saveCellsToXml(){
    pointsXML.clear();

    for(int i = 0; i < numLEDs; i++)
    {
        if(cells[i].isPointsSet())
        {
            //create a new cell child
            pointsXML.addTag("CELL");
            //set location to that cell child
            pointsXML.pushTag("CELL", i);
   
            //go through points
            for(int j = 0; j < cells[i].p.size(); j++)
            {
                pointsXML.addTag("PT");
                pointsXML.pushTag("PT", j);
                pointsXML.setValue("X", cells[i].p[j].x);
                pointsXML.setValue("Y", cells[i].p[j].y);
                pointsXML.popTag();
            }
            pointsXML.popTag();
        }
    }
    pointsXML.save("cellPoints.xml");
}


//////////////////////////// RUN LIGHTS //////////////////////////////////
void testApp::sendDMX(){
   
    //force first byte to zero (it is not a channel but DMX type info - start code)
	dmxData_[0] = 0;
    
	if ( ! dmxInterface_ || ! dmxInterface_->isOpen() ) {
		printf( "Not updating, enttec device is not open.\n");
	}
	else{
		//send the data to the dmx interface
		dmxInterface_->writeDmx( dmxData_, DMX_DATA_LENGTH );
	}
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' ')
    {
        bShowVideo = !bShowVideo;
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
