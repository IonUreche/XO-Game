
#include <windows.h>		// Header File For Windows
#include <Windowsx.h>
#include <WinUser.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <stdio.h>			// Header File For Standard Input/Output
#include <string>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <list>
#include "SOIL.h"
#include "XO.h"
#include "Firework.h"
#include <irrKlang.h>
			
using namespace irrklang;
using namespace std;

const int XSize=800, YSize=600;
const int startx=400, starty=150;
const int GridX=200, GridY=200;

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

GLfloat	xrot;				// X Rotation ( NEW )
GLfloat	yrot;				// Y Rotation ( NEW )
GLfloat	zrot;				// Z Rotation ( NEW )

GLuint	OTexture[4], XTexture[4], NewGameButtonTexture, StyleButtonTextures[3][3];			// Storage For One Texture ( NEW )
GLuint FireTexture[76], AudioButtonTexture[14], DifficultyButtonTextures[6];
GLuint FireWorkTextures[8];

GLfloat R[3],G[3],B[3], Rline=1, Gline=0, Bline=0, WinR=0.3, WinG=0.1, WinB=0.5;
GLfloat BackGroundR=0.0, BackGroundG=0.0, BackGroundB=0.0;

string path;

UINT TimmerID, TimmerID2;

int currentTexture=2, currentTime=0, precTime=0, fiveMilliseconds=0, TimeUntilMove;
int MouseXPos=0, MouseYPos=0, MXPos, MYPos, MDX, MDY, X1, Y1, pX, pY, tp, lastAdd=-31, EnemyTurn=0, PlayerTurn=1;
int nrDrawings=0, MaxDrawings = 30;
unsigned int TX;

int table[3][3], config[20000], state[3], N;
int p3[10], Cod, r1;
int NextTurnX, NextTurnY, TypeTurn, v, Marked, FireTime=0, FireFrame=-1, LineFrame;
int AudioPlayerButtonState[7], DifficultyButtonState[3];
float volume=0.2f;
bool Pressed = 0, Paused = 1, FirstTimeMarked = 1, Finished, Stopped = 0;
POINT p;

int AudioButtonX[7] = { 640, 680, 720, 660, 700, 660, 700};
int AudioButtonY[7] = { 290, 290, 290, 330, 330, 250, 250 };
int AudioButtonWidth[7] = { 40, 40, 40, 40, 40, 40, 40};
int AudioButtonHeight[7] = { 40, 40, 40, 40, 40, 40, 40 };

int DifficultyButtonX[3] = { 610, 670, 730 };
int DifficultyButtonY[3] = { 385, 385, 385 };
int DifficultyButtonWidth[3] = { 60, 60, 60 };
int DifficultyButtonHeight[3] = { 60, 60, 60 };

int xFW[8] = { 5, 5, 5, 5, 5, 5, 5, 5 };
int yFW[8] = { 4, 4, 6, 5, 10, 8, 8, 5 };
int dFW[8] = { 1, 1, 1, 1, 1, 1, 1, 1};

int currentSong = 0, Probability = 3;
int NWin, NDraw, NLose;
int xWin[9], yWin[9], xDraw[9], yDraw[9], xLose[9], yLose[9];
int Winner, xWnr[3], yWnr[3], X1Line, Y1Line, X2Line, Y2Line, DX, DY, LineTime, XF, YF, rnd, Xx, Yy;
float Tx, Ty, Sx, Sy;

list<XO> XOlist;
list<XO>::iterator it;

list<Firework> FWlist;
list<Firework>::iterator fwIt;

ISoundEngine* engine;
ISoundSource* Songs[3];


LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

int LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
    static int i,j;
	// Load Player Button Textures
	for(i=1;i<=14;++i)
	{
		path="Content/PlayerButtons/B";
		if(i<=7)
		{
		  path+=char(i + 48);
		  path+="Normal.png";
		}
		else
		{
          path+=char(i-7 + 48);
		  path+="Pressed.png";
		}

		AudioButtonTexture[i-1] = SOIL_load_OGL_texture
		(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

		if(AudioButtonTexture[i-1] == 0)
		return false;

		glBindTexture(GL_TEXTURE_2D, AudioButtonTexture[i-1]);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}

	// Load Fireworks Textures
	path = "Content/Fireworks/effectX.png";
	for (i = 0; i < 8; ++i)
	{
		path[path.length() - 5] = char(i + 1 + 48);
		FireWorkTextures[i] = SOIL_load_OGL_texture
			(
			path.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y
			);

		if (FireWorkTextures[i] == 0)
			return false;

		glBindTexture(GL_TEXTURE_2D, FireWorkTextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// Load Difficulty Button Textures
	for (i = 0; i < 6; ++i)
	{
		if (i == 0 || i == 3) path = "Content/Pawn";
		if (i == 1 || i == 4) path = "Content/Tower";
		if (i == 2 || i == 5) path = "Content/King";
		if (i >= 3) path += 'P';
		path += ".png";

		DifficultyButtonTextures[i] = SOIL_load_OGL_texture
			(
			path.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y
			);

		if (DifficultyButtonTextures[i] == 0)
			return false;

		glBindTexture(GL_TEXTURE_2D, DifficultyButtonTextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}


	// Fire Textures	
	for(i=1;i<=75;++i)
	{
		path="Content/Fire/fire_";
		path+=char(i/10 + 48);
		path+=char(i%10 + 48);
		path+=".png";

		FireTexture[i-1] = SOIL_load_OGL_texture
		(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

		if(FireTexture[i-1] == 0)
		return false;

		glBindTexture(GL_TEXTURE_2D, FireTexture[i-1]);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}


	// Buttons Texture
	for (i = 0; i < 3; ++i)
	{
		if (i == 0) path = "Content/RedStyle";
		if (i == 1) path = "Content/WhiteStyle";
		if (i == 2) path = "Content/BlackStyle";

		for (j = 0; j < 3; ++j)
		{
			if (j == 0)
			{
				path += char(j + 48);
				path += ".png";
			}
			else path[path.length() - 5] = char(j + 48);

			StyleButtonTextures[i][j] = SOIL_load_OGL_texture
				(
				path.c_str(),
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_INVERT_Y
				);

			if (StyleButtonTextures[i][j] == 0)
				return false;

			glBindTexture(GL_TEXTURE_2D, StyleButtonTextures[i][j]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}
	}

	
	NewGameButtonTexture = SOIL_load_OGL_texture
	(
	"Content/ButtonT1.png",
	SOIL_LOAD_AUTO,
	SOIL_CREATE_NEW_ID,
	SOIL_FLAG_INVERT_Y
	);

	if (NewGameButtonTexture == 0)
	return false;

	glBindTexture(GL_TEXTURE_2D, NewGameButtonTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// X & O Textures
	
	for(i=0;i<3;++i)
	{
		if(i==0) path="Content/XBlack.png";
		if(i==1) path="Content/XWhite.png";
		if(i==2) path="Content/XRed.png";

		XTexture[i] = SOIL_load_OGL_texture
		(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

		if(XTexture[i] == 0)
		return false;

		glBindTexture(GL_TEXTURE_2D, XTexture[i]);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}

	for(i=0;i<3;++i)
	{
		if(i==0) path="Content/OBT1.png";
		if(i==1) path="Content/OWW.png";
		if(i==2) path="Content/OBLT2.png";

		OTexture[i] = SOIL_load_OGL_texture
		(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

		if(OTexture[i] == 0)
		return false;

		glBindTexture(GL_TEXTURE_2D, OTexture[i]);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
	}
	 

	return true;										// Return Success
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	glOrtho (0, width, height, 0, 0, 1);
	gluPerspective(0,(GLfloat)width/(GLfloat)height,0,0);

	glMatrixMode (GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void ReInitGame()
{
  int i,j;
  for(i=0;i<3;++i)
	 for(j=0;j<3;++j)
		 table[i][j]=0;
  XOlist.clear();
  PlayerTurn=1;
  EnemyTurn=0;
  Marked=0;
  FirstTimeMarked = 1;
  MouseXPos=MouseYPos=900;
  Winner = 0;
  FWlist.clear();
  srand(int(time(NULL)));
  //X1=Y1=;
}

int EndGame()
{
	int i;
	for(i=0;i<3;++i)
	{
		if (table[i][0] && table[i][0] == table[i][1] && table[i][1] == table[i][2])
		{
			Winner = table[i][0];
			xWnr[0] = xWnr[1] = xWnr[2] = i;
			yWnr[0] = 0; yWnr[1] = 1; yWnr[2] = 2;
			return (table[i][0] == 1) ? 1 : 2;
		}
			
		if (table[0][i] && table[0][i] == table[1][i] && table[1][i] == table[2][i])
		{
			Winner = table[0][i];
			yWnr[0] = yWnr[1] = yWnr[2] = i;
			xWnr[0] = 0; xWnr[1] = 1; xWnr[2] = 2;
			return (table[0][i] == 1) ? 1 : 2;
		}		
	}

	if (table[0][0] && table[0][0] == table[1][1] && table[1][1] == table[2][2])
	{
		Winner = table[0][0];
		xWnr[0] = 0; xWnr[1] = 1; xWnr[2] = 2;
		yWnr[0] = 0; yWnr[1] = 1; yWnr[2] = 2;
		return (table[0][0] == 1) ? 1 : 2;
	}
			
	if (table[0][2] && table[0][2] == table[1][1] && table[1][1] == table[2][0])
	{
		Winner = table[0][2];
		xWnr[0] = 0; xWnr[1] = 1; xWnr[2] = 2;
		yWnr[0] = 2; yWnr[1] = 1; yWnr[2] = 0;
		return (table[0][2] == 1) ? 1 : 2;
	}
	return 0;
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	if (!LoadGLTextures())								// Jump To Texture Loading Routine ( NEW )
	{
		return FALSE;									// If Texture Didn't Load Return FALSE
	}

	ReInitGame();

	glMatrixMode (GL_PROJECTION);
 
    glLoadIdentity ();
 
    glOrtho (0, XSize, YSize, 0, 0, 1);
 
    glMatrixMode (GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
				
	glClearColor(1.0,1.0, 1.0, 0);				// Black Background						
	glDisable(GL_DEPTH_TEST);		

	ifstream fi("Content/configs.txt");
	N=0;
	while(fi>>config[N]) ++N;
	fi.close();
	p3[0]=1;
    for(int i=1;i<=9;++i)
      p3[i]=p3[i-1]*3;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 		
	//glBlendFunc(// The Type Of Depth Testing To Do

	// start irrKlang with default parameters
    engine = createIrrKlangDevice();

    if (!engine)
    return 0; // error starting up the engine
 
    // play some sound stream, looped
	Songs[0] = engine->addSoundSourceFromFile("AudioContent/Primi passi.mp3");
	Songs[1] = engine->addSoundSourceFromFile("AudioContent/03 - Marilyn Manson - Mechanical Animals.mp3");
	Songs[2] = engine->addSoundSourceFromFile("AudioContent/Finale.mp3"); 
	currentSong = rand() % 3;
	engine->play2D(Songs[currentSong]);
	if (engine)
    // engine->setVolume(0);
	engine->setSoundVolume(volume);
	//engine->drop(); // delete engine

	return TRUE;										// Initialization Went OK
}
/*
void DisplayFPS(int FPS, int nrBullets){
	 int q=0,aux=FPS,nrcif=0;

		strcpy_s(path,"FPS:"); 
	
		while(aux){ ++nrcif; aux/=10; }
	
		aux=FPS; q=nrcif;
		while(aux){ s2[--q]=(char)(aux%10+48); aux/=10; }
		s2[nrcif]='\0';
		strcat_s(path,s2); 

		strcat_s(path," Bullets: "); 

		nrcif=0; aux=nrBullets; 
		while(aux){ ++nrcif; aux/=10; }
		q=nrcif; aux=nrBullets;
		
		while(aux){ s2[--q]=(char)(aux%10+48); aux/=10; }
		if(nrcif == 0){ s2[0]='0'; s2[1]='\0'; } else s2[nrcif]='\0';

		strcat_s(path,s2);

		strcat_s(path," Explosions: ");
		nrcif=0; aux=nrExplosions; 
		while(aux){ ++nrcif; aux/=10; }
		q=nrcif; aux=nrExplosions;
		while(aux){ s2[--q]=(char)(aux%10+48); aux/=10; }
		if(nrcif == 0){ s2[0]='0'; s2[1]='\0'; } else s2[nrcif]='\0';
		strcat_s(path,s2);

		SetWindowText(hWnd,path);
}
*/

void UpdateScene()
{
	static unsigned int i,j;
	// Update Fire
	if(fiveMilliseconds - FireTime > 1)
	{
		if(FireFrame==-1) FireFrame=0;
		else
		{
			++FireFrame;
			if(FireFrame >= 75) FireFrame=1;
		}
	    FireTime = fiveMilliseconds;
	}

	// Get Cursor Position
	if (GetCursorPos(&p))
	{
		if (ScreenToClient(hWnd, &p))
		{
			MXPos = p.x;
			MYPos = p.y;
		}	
	}

	if (620 <= MDX && MDX <= 780 && 90 <= MDY && MDY <= 130)
	{
		state[0] = 2;
	}
	else
	if (620 <= MDX && MDX <= 780 && 140 <= MDY && MDY <= 180)
	{
		state[1] = 2;
	}
	else
	if (620 <= MDX && MDX <= 780 && 190 <= MDY && MDY <= 230)
	{
		state[2] = 2;
	}
	else
	{
		for (i = 0; i < 7; ++i)
		if (AudioButtonX[i] <= MDX && MDX < AudioButtonX[i] + AudioButtonWidth[i] && AudioButtonY[i] <= MDY && MDY < AudioButtonY[i] + AudioButtonHeight[i])
		{
			if (AudioPlayerButtonState[i] == 0)
			{
				AudioPlayerButtonState[i] = 1;
			}
		}
		else
		{
			AudioPlayerButtonState[i] = 0;
		}

		for (i = 0; i < 3; ++i)
		if (DifficultyButtonX[i] <= MDX && MDX < DifficultyButtonX[i] + DifficultyButtonWidth[i] && DifficultyButtonY[i] <= MDY && MDY < DifficultyButtonY[i] + DifficultyButtonHeight[i])
		{
			if (DifficultyButtonState[i] == 0)
			{
				DifficultyButtonState[i] = 1;
			}
		}
		else
		{
			DifficultyButtonState[i] = 0;
		}
	}
	

	for (i = 0; i < 7; ++i)
	if (AudioButtonX[i] <= MouseXPos && MouseXPos < AudioButtonX[i] + AudioButtonWidth[i] && AudioButtonY[i] <= MouseYPos && MouseYPos < AudioButtonY[i] + AudioButtonHeight[i])
	{
		switch(i)
		{
		  case 0: 
			if (!Paused || !engine->isCurrentlyPlaying(Songs[currentSong]))
			{
				if (!engine->isCurrentlyPlaying(Songs[currentSong])) engine->play2D(Songs[currentSong], 0, 0, 1, 0);
				else engine->setAllSoundsPaused(Paused);
				Paused = 1;
				Stopped = 0;
			}	  
			break;

		  case 1: if (Paused) engine->setAllSoundsPaused(Paused);
				else engine->setAllSoundsPaused(Paused);
				Paused = !Paused;
				break;

		  case 2: Paused = 1; Stopped = 1;  engine->stopAllSounds(); break;

		  case 3: --currentSong; engine->stopAllSounds(); if (currentSong < 0) currentSong += 3; engine->play2D(Songs[currentSong], 0, 0, 1, 0); break;

		  case 4: ++currentSong; engine->stopAllSounds(); if (currentSong > 2) currentSong -= 3; engine->play2D(Songs[currentSong], 0, 0, 1, 0); break;

		  case 5: if (volume < 1) volume += 0.05f;  engine->setSoundVolume(volume); break;

		  case 6: if (volume > 0) volume -= 0.05f;  engine->setSoundVolume(volume); break;
		}
	}

	for (i = 0; i < 3; ++i)
	if (DifficultyButtonX[i] <= MouseXPos && MouseXPos <DifficultyButtonX[i] + DifficultyButtonWidth[i] && DifficultyButtonY[i] <= MouseYPos && MouseYPos < DifficultyButtonY[i] + DifficultyButtonHeight[i])
	{
		switch (i)
		{
		  case 0: ReInitGame(); Probability = 3; break;
		  case 1: ReInitGame(); Probability = 2; break;
		  case 2: ReInitGame(); Probability = 1; break;
		}
	}

	if (!engine->isCurrentlyPlaying(Songs[currentSong]) && !Stopped) // The song ended
	{
		++currentSong;
		if (currentSong > 2) currentSong -= 3;
		engine->stopAllSounds();
		engine->play2D(Songs[currentSong], 0, 0, 1, 0);
	}

	if (!Pressed)
	{
		if (620 <= MXPos && MXPos <= 780 && 90 <= MYPos && MYPos <= 130)
		{
			if (state[0] == 0)
			{
				state[0] = 1;
			}
		}
		else
		{
			state[0] = 0;
		}

		if (620 <= MXPos && MXPos <= 780 && 140 <= MYPos && MYPos <= 180)
		{
			if (state[1] == 0)
			{
				state[1] = 1;
			}
		}
		else
		{
			state[1] = 0;
		}

		if (620 <= MXPos && MXPos <= 780 && 190 <= MYPos && MYPos <= 230)
		{
			if (state[2] == 0)
			{
				state[2] = 1;
			}
		}
		else
		{
			state[2] = 0;
		}	
	}

	if(MouseXPos >= 0) //if was clicked
	{
		X1=MouseXPos / GridX;
		Y1=MouseYPos / GridY;
		if(620<=MouseXPos && MouseXPos<=780 && 30<=MouseYPos && MouseYPos <=70)
		{
			ReInitGame();
		}
		else
		if(620<=MouseXPos && MouseXPos<=780 && 90<=MouseYPos && MouseYPos <=130)
		{
			BackGroundR=BackGroundG=BackGroundB=0.0f;
			currentTexture=2;
			Rline=1; Gline=Bline=0;
			WinR = 0.15; WinG = 0.2; WinB = 0.5;
		}
		else
        if(620<=MouseXPos && MouseXPos<=780 && 140<=MouseYPos && MouseYPos <=180)
		{
			BackGroundR=BackGroundG=BackGroundB=1.0f;
			currentTexture=0;
			Rline=Gline=Bline=0;
			WinR = 0.5; WinG = 0.7; WinB = 0.2;
		}
	    else
        if(620<=MouseXPos && MouseXPos<=780 && 190<=MouseYPos && MouseYPos <=230)
		{
			BackGroundR=BackGroundG=BackGroundB=0.0f;
			currentTexture=1;
			Rline=Gline=Bline=1;
			WinR = 0.8; WinG = 0.5; WinB = 0.3;
		}
	    else
		if(!table[X1][Y1] && X1<3 && Y1<3 && Marked!=9 && PlayerTurn && currentTime-lastAdd > 30)
		{
			table[X1][Y1]=1;
			XO H(X1, Y1, 0, XTexture[currentTexture]);
			XOlist.push_front(H);
			++Marked;
			lastAdd=currentTime;
			EnemyTurn = 1;
			TimeUntilMove = currentTime + 15; // 0.3 seconds
			PlayerTurn = 0;
			if(EndGame()>0) Marked=9;
		}
		MouseXPos=-1;
	}

	if(EnemyTurn && TimeUntilMove<=currentTime)
	{
		Cod=0;
		// Get current configuration
		for(i=0;i<3;++i)
          for(j=0;j<3;++j)
          {         
            Cod+=p3[i*3+j] * table[i][j];  
          }
        
		if(Marked != 9)
		{
		// Look for the best posible next configuration
		 NextTurnX=NextTurnY=-1;

		 NWin = NDraw = NLose = 0;

		for(i=0;i<3;++i)
		  for(j=0;j<3;++j)
		  if(!table[i][j])
		  {
             v = Cod + p3[i*3+j] * 2;

			 if (config[v] == 2)
			 {
				 xWin[NWin] = i;
				 yWin[NWin] = j;
				 ++NWin;
			 }
			 if (config[v] == 3)
			 {
				 xDraw[NDraw] = i;
				 yDraw[NDraw] = j;
				 ++NDraw;
			 }
			 if (config[v] == 1)
			 {
				 xLose[NLose] = i;
				 yLose[NLose] = j;
				 ++NLose;
			 }			
		  }
        
		  if (NWin > 0 && (rand() % Probability == 0 || (NDraw == 0 && NLose == 0)))
		  {
			  r1 = rand() % NWin;
			  NextTurnX = xWin[r1];
			  NextTurnY = yWin[r1];
		  }
		  else
		  if (NDraw > 0 && (rand() % Probability == 0 || NLose == 0))
		  {
			  r1 = rand() % NDraw;
			  NextTurnX = xDraw[r1];
			  NextTurnY = yDraw[r1];
		  }
		  else
		  {
			  r1 = rand() % NLose;
			  NextTurnX = xLose[r1];
			  NextTurnY = yLose[r1];
		  }

		// Add the next mark of O to the table
		table[NextTurnX][NextTurnY] = 2;
		XO ABC(NextTurnX, NextTurnY, 1, OTexture[currentTexture]);
		XOlist.push_front(ABC);
		++Marked;
		if(EndGame()>0) Marked=9;

		EnemyTurn = 0;
		PlayerTurn = 1;
	
		TimeUntilMove = currentTime + 15; // 0.3 seconds
		}
	}

	if (Marked == 9 && Winner==3)
	{
		if (FirstTimeMarked)
		{
			DX = (abs(xWnr[2] - xWnr[0]) * 200) / MaxDrawings;
			if (xWnr[2] - xWnr[0] < 0) DX = -DX;

			DY = (abs(yWnr[2] - yWnr[0]) * 200) / MaxDrawings;
			if (yWnr[2] - yWnr[0] < 0) DY = -DY;

			X1Line = 100 + xWnr[0] * 200  ;
			Y1Line = 100 + yWnr[0] * 200  ;
			X2Line = X1Line;
			Y2Line = Y1Line;

			XF = 100 + xWnr[2] * 200;
			YF = 100 + yWnr[2] * 200;

			LineTime = currentTime;
			FirstTimeMarked = 0;
			Finished = 0;
			nrDrawings = 0;
		}
		
		if (currentTime != LineTime && !Finished)
		{
			X2Line += DX;
			Y2Line += DY;
			nrDrawings++;
			if (nrDrawings == MaxDrawings) Finished = 1;
			LineTime = currentTime;
		}
	}

	if (Winner == 1)
	{
		r1 = 50 + rand() % 50;
		for (i = 1; i <= r1; ++i)
		{
			rnd = rand() % 8;
			Firework F(rand() % 670, rand() % 470, xFW[rnd], yFW[rnd], currentTime + rand()%150, dFW[rnd], rnd);
			FWlist.push_front(F);
		}
		Winner = 3; // just for holding the condition if(Winner) in conditions above
	}
	else if (Winner == 2) Winner = 3;

	for (fwIt = FWlist.begin(); fwIt != FWlist.end(); ++fwIt)
		(*fwIt).Update(currentTime);
	
	for (fwIt = FWlist.begin(); fwIt != FWlist.end(); ++fwIt)
	if ((*fwIt).IsDead())
	{
		FWlist.erase(fwIt);
		break;
	}
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	static unsigned int i,j;

	UpdateScene();

	//++frames;
	glClearColor(BackGroundR, BackGroundG, BackGroundB, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	// Menu Rectangle
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2i(601, 0); glVertex2i(800,0);
	glVertex2i(800, 600); glVertex2i(601, 600);
	glEnd();
	
	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw Button
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NewGameButtonTexture);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f); glVertex2i(620, 30);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(780, 30);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(780, 70);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(620, 70);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, StyleButtonTextures[0][state[0]]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f); glVertex2i(620, 90);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(780, 90);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(780, 130);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(620, 130);

	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, StyleButtonTextures[1][state[1]]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f); glVertex2i(620, 140);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(780, 140);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(780, 180);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(620, 180);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, StyleButtonTextures[2][state[2]]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f); glVertex2i(620, 190);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(780, 190);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(780, 230);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(620, 230);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	// Draw Fire
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FireTexture[FireFrame]);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 1.0f); glVertex2i(640, 440);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(768, 440);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(768, 568);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(640, 568);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// Draw Grid Lines
	glColor4f(Rline, Gline, Bline, 1.0);

	glBegin(GL_LINE_LOOP);
	glVertex2i(1, 1); glVertex2i(600, 1);
	glVertex2i(600, 600); glVertex2i(1, 600);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2i(2, 2); glVertex2i(599, 2);
	glVertex2i(599, 599); glVertex2i(2, 599);
	glEnd();
	
	glBegin(GL_LINES);
    glVertex2i(200, 1); glVertex2i(200, 600);
	glVertex2i(400, 1); glVertex2i(400, 600);
	glVertex2i(0, 200); glVertex2i(600, 200);
	glVertex2i(0, 400); glVertex2i(600, 400);
	glVertex2i(199, 1); glVertex2i(199, 600);
	glVertex2i(399, 1); glVertex2i(399, 600);
	glVertex2i(0, 199); glVertex2i(600, 199);
	glVertex2i(0, 399); glVertex2i(600, 399);
	glEnd();

	// Draw Xs and Os
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	for(it=XOlist.begin(); it!=XOlist.end(); ++it)
	{
		pX=(*it).getX();
		pY=(*it).getY();
		tp=(*it).getT();
		TX=(*it).getTex();
		if(tp == 0) glBindTexture(GL_TEXTURE_2D, XTexture[currentTexture]);
		else glBindTexture(GL_TEXTURE_2D, OTexture[currentTexture]);
		
		glBegin(GL_QUADS);
	      glTexCoord2f(0.0f, 1.0f); glVertex2i((pX*GridX)+2, (pY*GridY)+2);
          glTexCoord2f(1.0f, 1.0f); glVertex2i(((pX+1)*GridX)-2 , (pY*GridY)+2);
	      glTexCoord2f(1.0f, 0.0f); glVertex2i(((pX+1)*GridX)-2 , ((pY+1)*GridY)-2);
	      glTexCoord2f(0.0f, 0.0f); glVertex2i((pX*GridX)+2, ((pY+1)*GridY)-2);
	   glEnd();
	}
	glDisable(GL_TEXTURE_2D);

	// Draw Audio Player Buttons
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	for(i=0;i<7;++i)
	{	
        glBindTexture(GL_TEXTURE_2D, AudioButtonTexture[7*AudioPlayerButtonState[i]+i]);
		glBegin(GL_QUADS);
		  glTexCoord2f(0.0f, 1.0f); glVertex2i(AudioButtonX[i], AudioButtonY[i]);
		  glTexCoord2f(1.0f, 1.0f); glVertex2i(AudioButtonX[i] + AudioButtonWidth[i] - 1, AudioButtonY[i]);
		  glTexCoord2f(1.0f, 0.0f); glVertex2i(AudioButtonX[i] + AudioButtonWidth[i]- 1, AudioButtonY[i] + AudioButtonHeight[i] - 1);
		  glTexCoord2f(0.0f, 0.0f); glVertex2i(AudioButtonX[i], AudioButtonY[i] + AudioButtonHeight[i] - 1);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	for (i = 0; i<3; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, DifficultyButtonTextures[3 * DifficultyButtonState[i] + i]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(DifficultyButtonX[i], DifficultyButtonY[i]);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(DifficultyButtonX[i] + DifficultyButtonWidth[i] - 1, DifficultyButtonY[i]);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(DifficultyButtonX[i] + DifficultyButtonWidth[i] - 1, DifficultyButtonY[i] + DifficultyButtonHeight[i] - 1);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(DifficultyButtonX[i], DifficultyButtonY[i] + DifficultyButtonHeight[i] - 1);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

	if (Marked == 9 && Winner)
	{
		glColor3f(WinR, WinG, WinB);
		glBegin(GL_QUADS);

		if (yWnr[0] == yWnr[2])
		{
			glVertex2i(X1Line, Y1Line - 5);
			glVertex2i(X1Line , Y1Line + 5);

			glVertex2i(X2Line, Y2Line + 5);
			glVertex2i(X2Line, Y2Line - 5);
		}
		else
		{
			glVertex2i(X1Line - 5, Y1Line);
			glVertex2i(X1Line + 5, Y1Line);

			glVertex2i(X2Line + 5, Y2Line);
			glVertex2i(X2Line - 5, Y2Line);
		}
		
		glEnd();
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	for (fwIt = FWlist.begin(); fwIt != FWlist.end(); ++fwIt)
	{
		glBindTexture(GL_TEXTURE_2D, FireWorkTextures[(*fwIt).getType()]);

		Tx = (*fwIt).getTx();
		Ty = (*fwIt).getTy();
		Sx = (*fwIt).getSx();
		Sy = (*fwIt).getSy();
		Xx = (*fwIt).getX();
		Yy = (*fwIt).getY();
		
		glBegin(GL_QUADS);

		  glTexCoord2f(Tx, Ty + Sy);         glVertex2i(Xx, Yy + 129);
		  glTexCoord2f(Tx + Sx, Ty + Sy);    glVertex2i(Xx + 129, Yy + 129);
		  glTexCoord2f(Tx + Sx, Ty);         glVertex2i(Xx + 129, Yy);
	  	  glTexCoord2f(Tx, Ty);              glVertex2i(Xx, Yy);
		   
		glEnd();
	  
	}
	glDisable(GL_TEXTURE_2D);
	return TRUE;										// Keep Going
} 

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	}

		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		//dwStyle= (WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION) & (~ WS_THICKFRAME) ;						// Windows Style
		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								startx, starty,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	// set up our timer
   TimmerID = SetTimer( hWnd , 101 , 20 , NULL);
   TimmerID2 = SetTimer( hWnd , 102 , 5 , NULL);

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_TIMER:
			{
				if(wParam == 101) currentTime++;
				if(wParam == 102) fiveMilliseconds++;
				break;
			}
			

		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}

		case WM_LBUTTONDOWN:
		{
			MDX = GET_X_LPARAM(lParam);
            MDY = GET_Y_LPARAM(lParam);
			Pressed = 1;
			break;
		}

		case WM_LBUTTONUP:
		{
			MouseXPos = GET_X_LPARAM(lParam); 
            MouseYPos = GET_Y_LPARAM(lParam);
			if (MouseXPos != MDX && MouseYPos != MDY)
			{
				MouseXPos = MouseYPos = -1;
			}
			MDX = MDY = -1;
			Pressed = 0;
			state[0] = state[1] = state[2] = 0;
			break;
		}

	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
	fullscreen=FALSE;							// Windowed Mode


	// Create Our OpenGL Window
	if (!CreateGLWindow("XO Game",XSize,YSize,32,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
