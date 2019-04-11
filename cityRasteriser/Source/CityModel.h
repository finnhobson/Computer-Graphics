#ifndef PROCEDURAL_CITY_MODEL
#define PROCEDURAL_CITY_MODEL

// Procedurally generates a city model.

#include <glm/glm.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec4 v0;
	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 normal;
	glm::vec3 color;

	Triangle( glm::vec4 v0, glm::vec4 v1, glm::vec4 v2, glm::vec3 color )
		: v0(v0), v1(v1), v2(v2), color(color)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
	  glm::vec3 e1 = glm::vec3(v1.x-v0.x,v1.y-v0.y,v1.z-v0.z);
	  glm::vec3 e2 = glm::vec3(v2.x-v0.x,v2.y-v0.y,v2.z-v0.z);
	  glm::vec3 normal3 = glm::normalize( glm::cross( e2, e1 ) );
	  normal.x = normal3.x;
	  normal.y = normal3.y;
	  normal.z = normal3.z;
	  normal.w = 1.0;
	}
};

void GenerateModel( std::vector<Triangle>& triangles )
{
	using glm::vec3;
	using glm::vec4;

	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 white(  0.75f, 0.75f, 0.75f );
  vec3 grey(  0.5f, 0.5f, 0.5f );

	triangles.clear();
	triangles.reserve( 5*2*500 + 2 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 5000;

	vec4 A(L,0,0,1);
	vec4 B(0,0,0,1);
	vec4 C(L,0,L,1);
	vec4 D(0,0,L,1);

	vec4 E(L,L,0,1);
	vec4 F(0,L,0,1);
	vec4 G(L,L,L,1);
	vec4 H(0,L,L,1);

	// Floor:
	triangles.push_back( Triangle( C, B, A, green ) );
	triangles.push_back( Triangle( C, D, B, green ) );

	// ---------------------------------------------------------------------------
	// Short block

  for (int i = 0; i < 500; i++) {

    srand(time(NULL) * i);
    int randX = rand() % 4800 + 100;
    int randZ = rand() % 4800 + 100;
    int randWidth = rand() % 50 + 50;
    int randHeight = rand() % 700 + 200;

    A = vec4(randX,0,randZ+randWidth,1);
    B = vec4(randX,0, randZ,1);
    C = vec4(randX+randWidth,0,randZ+randWidth,1);
    D = vec4(randX+randWidth,0,randZ,1);

    E = vec4(randX,randHeight,randZ+randWidth,1);
    F = vec4(randX,randHeight,randZ,1);
    G = vec4(randX+randWidth,randHeight,randZ+randWidth,1);
    H = vec4(randX+randWidth,randHeight,randZ,1);

    // Front
    triangles.push_back( Triangle(E,B,A,grey) );
    triangles.push_back( Triangle(E,F,B,grey) );

    // Front
    triangles.push_back( Triangle(F,D,B,grey) );
    triangles.push_back( Triangle(F,H,D,grey) );

    // BACK
    triangles.push_back( Triangle(H,C,D,grey) );
    triangles.push_back( Triangle(H,G,C,grey) );

    // LEFT
    triangles.push_back( Triangle(G,E,C,grey) );
    triangles.push_back( Triangle(E,A,C,grey) );

    // TOP
    triangles.push_back( Triangle(G,F,E,grey) );
    triangles.push_back( Triangle(G,H,F,grey) );
  }


	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for( size_t i=0; i<triangles.size(); ++i )
	{
		triangles[i].v0 *= 2/L;
		triangles[i].v1 *= 2/L;
		triangles[i].v2 *= 2/L;

		triangles[i].v0 -= vec4(1,1,1,1);
		triangles[i].v1 -= vec4(1,1,1,1);
		triangles[i].v2 -= vec4(1,1,1,1);

		triangles[i].v0.x *= -1;
		triangles[i].v1.x *= -1;
		triangles[i].v2.x *= -1;

		triangles[i].v0.y *= -1;
		triangles[i].v1.y *= -1;
		triangles[i].v2.y *= -1;

		triangles[i].v0.w = 1.0;
		triangles[i].v1.w = 1.0;
		triangles[i].v2.w = 1.0;

		triangles[i].ComputeNormal();
	}
}

#endif
