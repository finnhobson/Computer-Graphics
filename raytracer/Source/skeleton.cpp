#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include <stdint.h>

using namespace std;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;


#define SCREEN_WIDTH 200
#define SCREEN_HEIGHT 200
#define FULLSCREEN_MODE false

struct Intersection
{
  vec4 position;
  float distance;
  int triangleIndex;
};


/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

bool Update(vec4& cameraPos);
void Draw(screen* screen, const vector<Triangle>& triangles, vec4& cameraPos);
bool ClosestIntersection( vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection);

int main( int argc, char* argv[] )
{

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  vector<Triangle> triangles;
  LoadTestModel(triangles);

  vec4 cameraPos( 0.0, 0.0, -3.0, 1.0 );

  while( Update(cameraPos) )
    {
      Draw(screen, triangles, cameraPos);
      SDL_Renderframe(screen);
    }

  SDL_SaveImage( screen, "screenshot.bmp" );

  KillSDL(screen);
  return 0;
}

/*Place your drawing here*/
void Draw(screen* screen, const vector<Triangle>& triangles, vec4& cameraPos)
{
  float focalLength = SCREEN_HEIGHT;
  //float cameraDist = -1.0 - focalLength;
  Intersection intersection;

  /* Clear buffer */
  memset(screen->buffer, 0, screen->height*screen->width*sizeof(uint32_t));

  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      vec4 dir(x - SCREEN_WIDTH/2, y - SCREEN_HEIGHT/2, focalLength, 1.0);
      ClosestIntersection(cameraPos, dir, triangles, intersection);
      int index = intersection.triangleIndex;
      vec3 color = triangles[index].color;
      PutPixelSDL(screen, x, y, color);
    }
  }
}

/*Place updates of parameters here*/
bool Update(vec4& cameraPos)
{
  /* Compute frame time */
  static int t = SDL_GetTicks();
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;

  /*Good idea to remove this*/
  std::cout << "Render time: " << dt << " ms." << std::endl;

  /* Update variables*/
  SDL_Event e;
  while(SDL_PollEvent(&e))
  {
    if (e.type == SDL_QUIT)
    {
      return false;
    }
    else if (e.type == SDL_KEYDOWN)
    {
      int key_code = e.key.keysym.sym;
      switch(key_code)
      {
        case SDLK_UP:
          /* Move camera forward */
          std::cout << "Forward." << std::endl;
          cameraPos.z += 0.1;
          break;
        case SDLK_DOWN:
          /* Move camera backwards */
          std::cout << "Back." << std::endl;
          cameraPos.z -= 0.1;
          break;
        case SDLK_LEFT:
          /* Move camera left */
          std::cout << "Left." << std::endl;
          cameraPos.x -= 0.1;
          break;
        case SDLK_RIGHT:
          /* Move camera right */
          std::cout << "Right." << std::endl;
          cameraPos.x += 0.1;
          break;
        case SDLK_ESCAPE:
          /* Move camera quit */
          return false;
        }
      }
    }
  return true;
}


bool ClosestIntersection( vec4 start, vec4 dir, const vector<Triangle>& triangles, Intersection& closestIntersection)
{
  bool intersects = false;
  float m = std::numeric_limits<float>::max();
  closestIntersection.distance = m;

  for (unsigned int i = 0; i < triangles.size(); i++)
  {
    vec4 v0 = triangles[i].v0;
    vec4 v1 = triangles[i].v1;
    vec4 v2 = triangles[i].v2;

    vec3 e1 = vec3(v1.x-v0.x, v1.y-v0.y, v1.z-v0.z);
    vec3 e2 = vec3(v2.x-v0.x, v2.y-v0.y, v2.z-v0.z);
    vec3 b = vec3(start.x-v0.x, start.y-v0.y, start.z-v0.z);

    vec3 d = vec3(dir.x, dir.y, dir.z);

    mat3 A( -d, e1, e2 );
    vec3 x = glm::inverse( A ) * b;

    float t = x.x;
    float u = x.y;
    float v = x.z;

    if (u > 0 && v > 0 && (u+v) < 1 && t >= 0 && t < closestIntersection.distance)
    {
      closestIntersection.position.x = v0.x + u*e1.x + v*e2.x;
      closestIntersection.position.y = v0.y + u*e1.y + v*e2.y;
      closestIntersection.position.z = v0.z + u*e1.z + v*e2.z;
      closestIntersection.position.w = v0.w;

      closestIntersection.distance = t;
      closestIntersection.triangleIndex = i;

      intersects = true;
    }
  }

  return intersects;
}
