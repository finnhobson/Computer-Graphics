#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModelH.h"
#include <stdint.h>

using namespace std;
using glm::vec2;
using glm::ivec2;
using glm::vec3;
using glm::mat3;
using glm::vec4;
using glm::mat4;

SDL_Event event;

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define FULLSCREEN_MODE false


/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */
vector<Triangle> triangles;

vec3 currentColor;

vec4 cameraPos( 0, 0, -3.001, 1 );
mat4 yRotation = mat4(1.0f);
float yAngle = 0;


/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

bool Update();
void Draw(screen* screen);
void VertexShader( const vec4& v, ivec2& p );
void Interpolate( ivec2 a, ivec2 b, vector<ivec2>& result );
vector<ivec2> DrawLineSDL( screen* screen, ivec2 a, ivec2 b, vec3 color);
vector<ivec2> DrawPolygonEdges( screen* screen, const vector<vec4>& vertices );
void UpdateRotation();
void ComputePolygonRows( const vector<ivec2>& edgePixels,
    vector<ivec2>& leftPixels,
    vector<ivec2>& rightPixels );
void DrawRows( screen* screen, const vector<ivec2>& leftPixels,
    const vector<ivec2> rightPixels );
void DrawPolygon( screen* screen, const vector<vec4>& vertices );


int main( int argc, char* argv[] )
{

  screen *screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT, FULLSCREEN_MODE );

  LoadTestModel(triangles);

  while ( Update())
    {
      Draw(screen);
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

  for( uint32_t i=0; i<triangles.size(); ++i ){
    currentColor = triangles[i].color;

    vector<vec4> vertices(3);
    vertices[0] = triangles[i].v0;
    vertices[1] = triangles[i].v1;
    vertices[2] = triangles[i].v2;

    // Rotate points around the y-axis
    vertices[0] = yRotation * vertices[0];
    vertices[1] = yRotation * vertices[1];
    vertices[2] = yRotation * vertices[2];

    DrawPolygon( screen, vertices );
  }
}

/*Place updates of parameters here*/
bool Update()
{
  /*static int t = SDL_GetTicks();
  // Compute frame time
  int t2 = SDL_GetTicks();
  float dt = float(t2-t);
  t = t2;
  std::cout << "Render time: " << dt << " ms." << std::endl;*/

  SDL_Event e;
  while(SDL_PollEvent(&e))
  {
    if (e.type == SDL_QUIT) return false;
    else if (e.type == SDL_KEYDOWN)
	  {
	    int key_code = e.key.keysym.sym;
	    switch(key_code)
      {
        case SDLK_UP:
          // Move camera forward
          cameraPos.z += 0.1;
          break;
        case SDLK_DOWN:
          // Move camera backwards
          cameraPos.z -= 0.1;
          break;
        case SDLK_LEFT:
          // Move camera left
          yAngle -= 0.1;
          UpdateRotation();
          break;
        case SDLK_RIGHT:
          // Move camera right
          yAngle += 0.1;
          UpdateRotation();
          break;
        case SDLK_ESCAPE:
          // Quit
          return false;
	      }
	    }
    }
  return true;
}


void VertexShader( const vec4& v, ivec2& projPos ) {
  float bigX = v.x - cameraPos.x;
  float bigY = v.y - cameraPos.y;
  float bigZ = v.z - cameraPos.z;

  projPos.x = (SCREEN_HEIGHT * bigX / bigZ) + (SCREEN_WIDTH/2);
  projPos.y = (SCREEN_HEIGHT * bigY / bigZ) + (SCREEN_HEIGHT/2);

}


void Interpolate( ivec2 a, ivec2 b, vector<ivec2>& result )
{
  int N = result.size();
  vec2 step = vec2(b-a) / float(max(N-1,1));
  vec2 current( a );
  for( int i=0; i<N; ++i )
  {
    result[i] = current;
    current += step;
  }
}

vector<ivec2> DrawLineSDL( screen* screen, ivec2 a, ivec2 b, vec3 color)
{
  ivec2 delta = glm::abs( a - b );
  int pixels = glm::max( delta.x, delta.y ) + 1;
  vector<ivec2> line(pixels);
  Interpolate(a, b, line);
  /*for (uint32_t i = 0; i < line.size(); i++) {
    PutPixelSDL( screen, line[i].x, line[i].y, color );
  }*/
  return line;
}

vector<ivec2> InterpolateLine( ivec2 a, ivec2 b ) {
  ivec2 delta = glm::abs( a - b );
  int pixels = glm::max( delta.x, delta.y ) + 1;
  vector<ivec2> line(pixels);
  Interpolate(a, b, line);
  return line;
}

vector<ivec2> DrawPolygonEdges( screen* screen, const vector<vec4>& vertices )
{
  int V = vertices.size();

  vector<ivec2> projectedVertices( V );
  for ( int i = 0; i < V; i++ ){
    VertexShader( vertices[i], projectedVertices[i] );
  }

  vector<ivec2> edgePixels;
  for (int i = 0; i < V; i++) {
    int j = (i+1)%V;
    vec3 color( 1, 1, 1 );
    vector<ivec2> line = DrawLineSDL( screen, projectedVertices[i], projectedVertices[j], color );
    edgePixels.insert(edgePixels.end(), line.begin(), line.end());
  }
  return edgePixels;
}

// Updates rotation matrix R with new y rotation value
void UpdateRotation()
{
  yRotation[0][0] = cos(yAngle);
  yRotation[0][2] = sin(yAngle);
  yRotation[2][0] = -sin(yAngle);
  yRotation[2][2] = cos(yAngle);
}

void ComputePolygonRows( const vector<ivec2>& vertexPixels,
    vector<ivec2>& leftPixels,
    vector<ivec2>& rightPixels )
{
  vector<ivec2> edgePixels;
  int V = vertexPixels.size();
  for (int i = 0; i < V; i++) {
    int j = (i+1)%V;
    vec3 color( 1, 1, 1 );
    vector<ivec2> line = InterpolateLine( vertexPixels[i], vertexPixels[j] );
    edgePixels.insert(edgePixels.end(), line.begin(), line.end());
  }

  int yMax = -numeric_limits<int>::max();
  int yMin = numeric_limits<int>::max();

  for ( uint32_t i = 0; i < edgePixels.size(); i++) {
    if (edgePixels[i].y > yMax) yMax = edgePixels[i].y;
    if (edgePixels[i].y < yMin) yMin = edgePixels[i].y;
  }

  int ROWS = yMax - yMin + 1;

  leftPixels.resize(ROWS);
  rightPixels.resize(ROWS);

  for ( int i = 0; i < ROWS; i++ ) {
    leftPixels[i].x = numeric_limits<int>::max();
    rightPixels[i].x = -numeric_limits<int>::max();
    leftPixels[i].y = yMin + i;
    rightPixels[i].y = yMin + i;
  }

  for ( uint32_t i = 0; i < edgePixels.size(); i++ ) {
    int row = edgePixels[i].y - yMin;
    if (edgePixels[i].x < leftPixels[row].x) leftPixels[row].x = edgePixels[i].x;
    if (edgePixels[i].x > rightPixels[row].x) rightPixels[row].x = edgePixels[i].x;
  }
}


void DrawRows( screen* screen, const vector<ivec2>& leftPixels,
               const vector<ivec2> rightPixels )
{
  int ROWS = leftPixels.size();
  for ( int i = 0; i < ROWS; i++ ) {
    int pixels = rightPixels[i].x - leftPixels[i].x + 1;
    vector<ivec2> row(pixels);
    Interpolate( leftPixels[i], rightPixels[i], row );
    for ( int j = 0; j < pixels; j++ ) {
      PutPixelSDL( screen, row[j].x, row[j].y, currentColor );
    }
  }
}

void DrawPolygon( screen* screen, const vector<vec4>& vertices )
{
    int V = vertices.size();

    vector<ivec2> vertexPixels( V );
    for (int i = 0; i < V; i++) {
      VertexShader( vertices[i], vertexPixels[i] );
    }

    vector<ivec2> leftPixels;
    vector<ivec2> rightPixels;
    ComputePolygonRows( vertexPixels, leftPixels, rightPixels );
    DrawRows( screen, leftPixels, rightPixels );
}
