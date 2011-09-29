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

void		Particle::reset() {
	m_Position.x = 	s3ePointerGetX();
	m_Position.y = 	s3ePointerGetY();
	m_Velocity.x =  ((IwRandMinMax(0, 1000) /1000.0f)- 0.5) * 5 ;
	m_Velocity.y = -(IwRandMinMax(0, 1000) /1000.0f)*2 ;
	
	life = 257;
	col.Set(IwRandMinMax(128, 255) , 0, 0, 255);
	
	
}

CIwSVec2 rippleCentre;
int32 g_RippleDuration = 0;
int32 g_TileSize=10;
void RippleFunc(CIwSVec2* v, CIwColour* c, int32 points)
{
	
	if (g_RippleDuration > 0) {
		
		//Get the centre of the ripple in subpixels (8 subpixels per pixel)
		//Apply the transform matrix to the centre, so it's in the correct place
		//= (CIwSVec2)(IwGxGetTransformMatrix().TransformVec(g_RippleCentre) << 3);
		
		while(points--)
		{
			CIwSVec2 dirToCentre = *v - rippleCentre;
			int32 distToCentre = dirToCentre.GetLength() / g_TileSize;
			
			int32 rippleHeight = distToCentre*20 + (g_RippleDuration)*7 + IW_GEOM_ONE/2;
			if( distToCentre && rippleHeight >= 0  )
			{
				dirToCentre.Normalise();
				int ofs = IW_FIXED_MUL(IW_FIXED_MUL(g_TileSize*(4), g_RippleDuration*2), IwGeomSin(rippleHeight));
				*v += dirToCentre * ofs;//MAX(0, ofs);
			}
			c++;
			v++;
		}
		g_RippleDuration--;
	}
	
}

void APIsetRipple(int32 x, int32 y) {
	rippleCentre.x = x;
	rippleCentre.y = y;
	g_RippleDuration=3000;
	
}

CGame::CGame()
: m_Position(0,0)

{
	particleTexture=  new CIwTexture();
	particleTexture->LoadFromFile("wabbit_alpha.png");
	particleTexture->Upload();	
}


CGame::~CGame()
{
	delete particleTexture;
}

void CGame::Update()
{
    // game logic goes here
	// for example, move a red square towards any touch event...
	if( s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_DOWN )
	{
		APIsetRipple(s3ePointerGetX(), s3ePointerGetY());
		CIwFVec2 target((float)s3ePointerGetX(), (float)s3ePointerGetY());		
		m_Position += (target - m_Position) * 0.05f;		
	}
	int32 maxCreated = FLOW_RATE;
	for (int32 i=0; i<NUM_PARTICLES;++i) {	
		
		if (maxCreated > 0 && m_particles[i].life ==0 ) {
			m_particles[i].reset();
			--maxCreated;
		}		
		
		if (m_particles[i].life > 0) {	
			if (m_particles[i].m_Position.x > IwGxGetDeviceWidth() || m_particles[i].m_Position.x < 0 || m_particles[i].m_Position.y > IwGxGetDeviceHeight() ) m_particles[i].reset();
			if (m_particles[i].life-- < 255) {
				m_particles[i].col.Set(255,0,0,m_particles[i].life);
			}		
			m_particles[i].m_Position.x +=  m_particles[i].m_Velocity.x;
			m_particles[i].m_Position.y +=  m_particles[i].m_Velocity.y;
			m_particles[i].m_Velocity.y += 0.05;
		}
	}
	
}


void CGame::Render()
{
    // game render goes here

	numImagesBatched=0;
    // for example, clear to black (the order of components is ABGR)
	IwGxSetColClear(0, 0, 0, 0);
	IwGxClear( IW_GX_COLOUR_BUFFER_F );
	
	DrawSprite(m_Position, &(m_particles[0].col)); 

	
	for (int32 i=0; i<NUM_PARTICLES;++i) {		
		DrawSprite(m_particles[i].m_Position, &(m_particles[i].col));   		
	}	
	
	 
	
	DrawBatched();
	IwGxSwapBuffers();
	

}
void CGame::DrawBatched(){
	
	if (numImagesBatched>0) {
		RippleFunc(screenVertexCoordinates, colourStream, numImagesBatched * 4);
		CIwMaterial *pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetAlphaMode( CIwMaterial::ALPHA_BLEND );	
		pMat->SetTexture( particleTexture );
		pMat->SetColAmbient( 0xFF, 0xFF, 0xFF, 0xFF );
		IwGxSetMaterial( pMat );
		IwGxSetVertStreamScreenSpace( screenVertexCoordinates, numImagesBatched * 4 );	
		
		IwGxSetUVStream( uvCoordinates );
		//IwGxSetColStream( colourStream, numImagesBatched * 4 );
		
		IwGxDrawPrims( IW_GX_TRI_LIST, pInds, numImagesBatched * 6 );	
		
		// show the surface
		IwGxFlush();
	}
	numImagesBatched=0;
	
}

void CGame::DrawSprite(CIwFVec2 p_Position, CIwColour*  col){
	uint16 index = numImagesBatched * 4;
	
	int x =int(p_Position.x);
	int y =int(p_Position.y);	
	
	// 0 = top-left
	screenVertexCoordinates[ index ].x = x;
	screenVertexCoordinates[ index ].y = y;
	// 1 = bottom-left
	screenVertexCoordinates[ index + 1 ].x = x;
	screenVertexCoordinates[ index + 1 ].y = y +32   ;
	// 2 = bottom-right
	screenVertexCoordinates[ index + 2 ].x = x + 32;
	screenVertexCoordinates[ index + 2 ].y = y + 32;
	// 3 = top-right
	screenVertexCoordinates[ index + 3 ].x = x + 32;
	screenVertexCoordinates[ index + 3 ].y = y;
	
	uint16 vindex = numImagesBatched * 6;
	
	//Set up indices for first triangle 
	pInds[vindex] = index;		
	pInds[vindex+1] = index+1;		
	pInds[vindex+2] = index+3;		
	
	//draw second triangle 
	pInds[vindex+3] = index+1;		
	pInds[vindex+4] = index+2;		
	pInds[vindex+5] = index+3;		
	
	
	

	// 0 = top-left
	uvCoordinates[ index ].x = IW_SFIXED( 0 );
	uvCoordinates[ index ].y = IW_SFIXED(0 );
	// 1 = bottom-left
	uvCoordinates[ index + 1 ].x = IW_SFIXED( 0 );
	uvCoordinates[ index + 1 ].y = IW_SFIXED(1 );
	// 2 = bottom-right
	uvCoordinates[ index + 2 ].x = IW_SFIXED( 1 );
	uvCoordinates[ index + 2 ].y = IW_SFIXED( 1 );
	// 3 = top-right
	uvCoordinates[ index + 3 ].x = IW_SFIXED( 1 );
	uvCoordinates[ index + 3 ].y = IW_SFIXED( 0 );
	
	/*
	colourStream[index] = *col;
	colourStream[index+1] = *col;
	colourStream[index+2] = *col;
	colourStream[index+3] = *col;
	*/
	
	numImagesBatched++;
	
	if (numImagesBatched==MAX_BATCHED_IMAGES) {
		DrawBatched();
	}
	

}


