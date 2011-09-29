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
#ifndef GAME_H
#define GAME_H
#include <iostream>
using namespace std;

#define NUM_PARTICLES 6000
#define FLOW_RATE 50
#define MAX_BATCHED_IMAGES 4000


class Particle {
public:
	Particle()   {
		life =0;
	
	};
	void		reset();
	CIwFVec2	m_Position;
	 CIwFVec2	m_Velocity;
	 int32		life;
	CIwColour  col;
};

class CGame
{
public:
    CGame();
    ~CGame();

    // update will be called a fixed number of times per second 
    // regardless of visual framerate
    void Update();
    // render will be called as fast as possible (but not faster 
    // than the update rate)
    void Render();
	void DrawSprite(CIwFVec2 p_Position, CIwColour* col);
	void DrawBatched();


private:
	CIwMaterial *pMat;
	CIwTexture*	particleTexture;
	Particle	m_particles[NUM_PARTICLES];
	CIwSVec2	screenVertexCoordinates[MAX_BATCHED_IMAGES * 4];
	CIwSVec2	uvCoordinates[MAX_BATCHED_IMAGES * 4];
	CIwColour	colourStream[MAX_BATCHED_IMAGES * 4];
	uint16		pInds [ MAX_BATCHED_IMAGES * 6 ];
	
    CIwFVec2	m_Position;
    CIwSVec2	m_Size;
	int			numImagesBatched;
};

#endif
