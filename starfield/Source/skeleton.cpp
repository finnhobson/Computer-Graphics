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

void Update(vector<vec3>& stars);
void Draw(screen* screen, vector<vec3>& stars);
void Interpolate( vec3 a, vec3 b, vector<vec3>& result);

int main( int argc, char* argv[] )
{
  vector<vec3> stars( 1000 );

  for ( unsigned int i = 0; i < stars.size(); i++ )
  {
    stars[i].x = float(rand()) / float(RAND_MAX) * 2 - 1;
    stars[i].y = float(rand()) / float(RAND_MAX) * 2 - 1;
    stars[i].z = float(rand()) / float(RAND_MAX);
    //cout << "( " << stars[i].x << ", " << stars[i].y << ", " << stars[i].z << " ) \n";
  }

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );
  t = SDL_GetTicks();	/*Set start value for timer.*/

  while( NoQuitMessageSDL() )
    {
      Update(stars);
      Draw(screen, stars);
      SDL_Renderframe(screen);
    }

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen, vector<vec3>& stars)
{
  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  vec3 white(1,1,1);

  for ( unsigned int i = 0; i < stars.size(); ++i )
  {
    float uStar = SCREEN_HEIGHT/2 * stars[i].x/stars[i].z + SCREEN_WIDTH/2;
    float vStar = SCREEN_HEIGHT/2 * stars[i].y/stars[i].z + SCREEN_HEIGHT/2;
    vec3 colour = 0.2f * white / (stars[i].z*stars[i].z);
    PutPixelSDL(screen, uStar, vStar, colour);
  }

  /* Bilinear Interpolation of Colours

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
  }*/
}

/*Place updates of parameters here*/
void Update(vector<vec3>& stars)
{
  // Compute frame time
  //static int t = SDL_GetTicks();
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;

  float velocity = 0.0003f;

  /* Update variables*/
  for ( unsigned int i = 0; i < stars.size(); ++i )
  {
    stars[i].z = stars[i].z - velocity * dt;
    if (stars[i].z <= 0) stars[i].z += 1;
    if (stars[i].z > 1) stars[i].z -= 1;
  }
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
