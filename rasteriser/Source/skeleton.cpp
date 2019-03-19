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

struct Pixel
{
  int x;
  int y;
  float zinv;
  vec4 pos3d;
};

struct Vertex
{
   vec4 position;
};

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */
vector<Triangle> triangles;

float depthBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

vec4 cameraPos( 0, 0, -3.001, 1 );
mat4 yRotation = mat4(1.0f);
float yAngle = 0;

vec4 lightPos( 0, -0.5, -0.7, 1.0 );
float intensity = 12.f;
float red = 1.0f;
float green = 1.0f;
float blue = 1.0f;
vec3 lightColor = intensity * vec3( red, green, blue );
vec3 indirectLight = 0.5f * vec3(red, green, blue);

vec3 currentColor;
vec4 currentNormal;


/* ---------------------------------------------------------------------------- */
/* FUNCTIONS                                                                    */
/* ---------------------------------------------------------------------------- */

bool Update();
void Draw(screen* screen);
void VertexShader( const Vertex& v, ivec2& p );
void UpdateRotation();
void ComputePolygonRows( const vector<Pixel>& vertexPixels,
    vector<Pixel>& leftPixels,
    vector<Pixel>& rightPixels );
void DrawRows( screen* screen, const vector<Pixel>& leftPixels,
    const vector<Pixel> rightPixels );
void PixelShader(screen* screen, const Pixel& p );
void DrawPolygon( screen* screen, const vector<Vertex>& vertices );
void InterpolatePixel( Pixel a, Pixel b, vector<Pixel>& result );
vec3 DirectLight( const Vertex & v );
vector<Pixel> InterpolateLine( Pixel a, Pixel b );
void UpdateLightColour();


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

  for( int y=0; y<SCREEN_HEIGHT; ++y )
    for( int x=0; x<SCREEN_WIDTH; ++x )
      depthBuffer[y][x] = 0;

  for( uint32_t i=0; i<triangles.size(); ++i ){
    currentColor = triangles[i].color;
    currentNormal = triangles[i].normal;

    vector<Vertex> vertices(3);
    vertices[0].position = triangles[i].v0;
    vertices[1].position = triangles[i].v1;
    vertices[2].position = triangles[i].v2;


    // Rotate points around the y-axis
    vertices[0].position = yRotation * vertices[0].position;
    vertices[1].position = yRotation * vertices[1].position;
    vertices[2].position = yRotation * vertices[2].position;

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
          case SDLK_w:
            // Move light forward
            lightPos.z += 0.1;
            break;
          case SDLK_s:
            // Move light forward
            lightPos.z -= 0.1;
            break;
          case SDLK_a:
            // Move light left
            lightPos.x -= 0.1;
            break;
          case SDLK_d:
            // Move light right
            lightPos.x += 0.1;
            break;
          case SDLK_q:
            // Move light up
            lightPos.y -= 0.1;
            break;
          case SDLK_e:
            // Move light down
            lightPos.y += 0.1;
            break;
          case SDLK_r:
            // Increase light intensity
            intensity += 1.0;
            UpdateLightColour();
            break;
          case SDLK_f:
            //Decrease light intensity
            if (intensity > 0) intensity -= 1.0;
            UpdateLightColour();
            break;
          case SDLK_t:
            // Increase light red value
            if (red < 1) red += 0.05;
            UpdateLightColour();
            break;
          case SDLK_g:
            // Decrease light red value
            if (red > 0) red -= 0.05;
            UpdateLightColour();
            break;
          case SDLK_y:
            // Increase light green value
            if (green < 1) green += 0.05;
            UpdateLightColour();
            break;
          case SDLK_h:
            // Decreae light green value
            if (green > 0) green -= 0.05;
            UpdateLightColour();
            break;
          case SDLK_u:
            // Increase light blue value
            if (blue < 1) blue += 0.05;
            UpdateLightColour();
            break;
          case SDLK_j:
            if (blue > 0) blue -= 0.05;
            UpdateLightColour();
            break;
        case SDLK_ESCAPE:
          // Quit
          return false;
	      }
	    }
    }
  return true;
}


void VertexShader( const Vertex& v, Pixel& p ) {
  float bigX = v.position.x - cameraPos.x;
  float bigY = v.position.y - cameraPos.y;
  float bigZ = v.position.z - cameraPos.z;

  p.x = (SCREEN_HEIGHT * bigX / bigZ) + (SCREEN_WIDTH/2);
  p.y = (SCREEN_HEIGHT * bigY / bigZ) + (SCREEN_HEIGHT/2);
  p.zinv = 1.0f / bigZ;

  p.pos3d.x = v.position.x * p.zinv;
  p.pos3d.y = v.position.y * p.zinv;
  p.pos3d.z = v.position.z * p.zinv;
  p.pos3d.w = v.position.w * p.zinv;
}

vec3 DirectLight( const Pixel& p )
{
  vec4 position = p.pos3d / p.zinv;
  vec4 normal = currentNormal;

  // Vector from intersection point to light source
  vec4 lightDir = vec4(lightPos.x-position.x, lightPos.y-position.y, lightPos.z-position.z, position.w);
  vec4 unitLightDir = glm::normalize(lightDir);

  float projection = glm::dot(unitLightDir, normal);
  //Distance from intersection point to light source
  float radius = glm::length(vec3(lightDir.x, lightDir.y, lightDir.z));

  vec3 D = lightColor * max(projection, 0.0f) / (4.0f * float(M_PI) * radius * radius);
  return D;
}


void InterpolatePixel( Pixel a, Pixel b, vector<Pixel>& result )
{
  int N = result.size();
  vec3 vecA(a.x, a.y, a.zinv);
  vec3 vecB(b.x, b.y, b.zinv);
  vector<vec3> vecResult( N );

  vec3 step = (vecB - vecA) / float(max(N-1,1));
  vec4 posStep = (b.pos3d - a.pos3d) / float(max(N-1,1));

  vec3 current( vecA );
  vec4 currentPos( a.pos3d );
  for( int i=0; i<N; ++i )
  {
    vecResult[i] = current;
    result[i].x = round(vecResult[i].x);
    result[i].y = round(vecResult[i].y);
    result[i].zinv = vecResult[i].z;
    result[i].pos3d = currentPos;
    current += step;
    currentPos += posStep;
  }
}


vector<Pixel> InterpolateLine( Pixel a, Pixel b ) {
  int xDelta = glm::abs( a.x - b.x );
  int yDelta = glm::abs( a.y - b.y );
  int pixels = glm::max( xDelta, yDelta ) + 1;
  vector<Pixel> line(pixels);
  InterpolatePixel(a, b, line);
  return line;
}


// Updates rotation matrix R with new y rotation value
void UpdateRotation()
{
  yRotation[0][0] = cos(yAngle);
  yRotation[0][2] = sin(yAngle);
  yRotation[2][0] = -sin(yAngle);
  yRotation[2][2] = cos(yAngle);
}

void ComputePolygonRows( const vector<Pixel>& vertexPixels,
    vector<Pixel>& leftPixels,
    vector<Pixel>& rightPixels )
{
  vector<Pixel> edgePixels;
  int V = vertexPixels.size();
  for (int i = 0; i < V; i++) {
    int j = (i+1)%V;
    vector<Pixel> line = InterpolateLine( vertexPixels[i], vertexPixels[j] );
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
    if (edgePixels[i].x < leftPixels[row].x) {
      leftPixels[row].x = edgePixels[i].x;
      leftPixels[row].zinv = edgePixels[i].zinv;
      leftPixels[row].pos3d = edgePixels[i].pos3d;
    }
    if (edgePixels[i].x > rightPixels[row].x) {
      rightPixels[row].x = edgePixels[i].x;
      rightPixels[row].zinv = edgePixels[i].zinv;
      rightPixels[row].pos3d = edgePixels[i].pos3d;
    }
  }
}


void DrawRows( screen* screen, const vector<Pixel>& leftPixels,
               const vector<Pixel> rightPixels )
{
  int ROWS = leftPixels.size();
  for ( int i = 0; i < ROWS; i++ ) {
    int pixels = rightPixels[i].x - leftPixels[i].x + 1;
    vector<Pixel> row(pixels);
    InterpolatePixel( leftPixels[i], rightPixels[i], row );
    for ( int j = 0; j < pixels; j++ ) {
      // printf("%lf\n", row[j].zinv);
      PixelShader(screen, row[j]);
      }
    }
  }

void PixelShader(screen* screen, const Pixel& p )
   {
       int x = p.x;
       int y = p.y;
       if( p.zinv > depthBuffer[y][x] )
       {
           depthBuffer[y][x] = p.zinv;
           vec3 directLight = DirectLight(p);
           vec3 illumination = currentColor * (directLight + indirectLight);
           PutPixelSDL( screen, x, y, illumination );
       }
}

void DrawPolygon( screen* screen, const vector<Vertex>& vertices )
{
    int V = vertices.size();

    vector<Pixel> vertexPixels( V );
    for (int i = 0; i < V; i++) {
      VertexShader( vertices[i], vertexPixels[i] );
    }

    vector<Pixel> leftPixels;
    vector<Pixel> rightPixels;
    ComputePolygonRows( vertexPixels, leftPixels, rightPixels );
    DrawRows( screen, leftPixels, rightPixels );
}

void UpdateLightColour()
{
  lightColor = intensity * vec3(red, green, blue);
  indirectLight = intensity/28.0f * vec3(red, green, blue);
}
