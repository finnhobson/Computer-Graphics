#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <stdint.h>

using namespace std;
using glm::vec3;
using glm::mat3;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 256
#define FULLSCREEN_MODE false


/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */
int t;

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw(screen* screen);
void Interpolate( vec3 a, vec3 b, vector<vec3>& result);

int main( int argc, char* argv[] )
{
  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );
  t = SDL_GetTicks();	/*Set start value for timer.*/

  while( NoQuitMessageSDL() )
    {
      Draw(screen);
      Update();
      SDL_Renderframe(screen);
    }

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen)
{
  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  vec3 topLeft(1,0,0); //red
  vec3 topRight(0,0,1); //blue
  vec3 bottomRight(0,1,0); //green
  vec3 bottomLeft(1,1,0); //yellow

  vector<vec3> leftSide( SCREEN_HEIGHT );
  vector<vec3> rightSide( SCREEN_HEIGHT );

  vector<vec3> row( SCREEN_WIDTH );

  Interpolate( topLeft, bottomLeft, leftSide );
  Interpolate( topRight, bottomRight, rightSide );

  for(int j=0; j<screen->height; j++) {
    Interpolate( leftSide[j], rightSide[j], row );
    for (int i=0; i<screen->width; i++) {
      PutPixelSDL(screen, i, j, row[i]);
    }
  }
}

/*Place updates of parameters here*/
void Update()
{
  /* Compute frame time
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;*/
  /*Good idea to remove this*/
  //std::cout << "Render time: " << dt << " ms." << std::endl;
  /* Update variables*/
}

void Interpolate( vec3 a, vec3 b, vector<vec3>& result) {
  float diffX = (b.x - a.x)/(result.size()-1);
  float diffY = (b.y - a.y)/(result.size()-1);
  float diffZ = (b.z - a.z)/(result.size()-1);
  if (result.size() == 1) {
    result[0].x = (a.x + b.x)/2;
    result[0].y = (a.y + b.y)/2;
    result[0].z = (a.z + b.z)/2;
  }
  else {
    for (unsigned int i = 0; i < result.size(); i++) {
      result[i].x = a.x + diffX * i;
      result[i].y = a.y + diffY * i;
      result[i].z = a.z + diffZ * i;
    }
  }
}
