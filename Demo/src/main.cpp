/*
 * This file is part of the Marmalade SDK Code Samples.
 *
 * Copyright (C) 2001-2011 Ideaworks3D Ltd.
 * All Rights Reserved.
 *
 * This source code is intended only as a supplement to Ideaworks Labs
 * Development Tools and/or on-line documentation.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */
#include "s3e.h"
#include "IwGx.h"
#include "game.h"
#include <iostream>
using namespace std;

// updates per second
#define UPS 60

// throttle frame time at 10 fps (i.e. the game will slow down rather
// than having very low frame rate)
#define MAX_UPDATES 60
int endTime=0;
int fps =0;
int ups =0;

int GetUpdateFrame()
{
	if ((int)s3eTimerGetMs() > endTime) {
		cout << fps << " " << ups << endl;
		
		endTime = s3eTimerGetMs()+1000;
		fps=0;
		ups=0;
	}
    return (int)(s3eTimerGetMs() / (1000/UPS));
}

// "main" is the S3E entry point
int main()
{
	
	IwGxInit();

    // create game object
    CGame* pGame = new CGame;

    int currentUpdate = GetUpdateFrame();
    int nextUpdate = currentUpdate;



    // to exit correctly, applications should poll for quit requests
	while(!s3eDeviceCheckQuitRequest())
	{
	    // run logic at a fixed frame rate (defined by UPS)
		// if an application uses polling input the application 
        // must call update once per frame
        s3ePointerUpdate();
        s3eKeyboardUpdate();
		
        // block until the next frame (don't render unless at 
        // least one update has occurred)
        while(!s3eDeviceCheckQuitRequest())
        {
            nextUpdate = GetUpdateFrame();
            if( nextUpdate != currentUpdate )
                break;
            s3eDeviceYield(1);
        }
        
        // execute update steps
        int frames = nextUpdate - currentUpdate;
        frames = MIN(MAX_UPDATES, frames);
		
        while(frames--)
        {
			ups++;
            pGame->Update();
        }
        currentUpdate = nextUpdate;
        
        // render the results
		fps++;
        pGame->Render();



        // S3E applications should yield frequently
		s3eDeviceYield();
	}

    // clear up game object
    delete pGame;

	IwGxTerminate();


	return 0;
}
