#include <glm/glm.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <random>
#include "GenerateCity.h"

// Procedurally generates a city model.

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
  vec3 building1(  0.5f, 0.5f, 0.57f );
	vec3 building2(  0.5f, 0.5f, 0.56f );
	vec3 building3(  0.5f, 0.5f, 0.55f );
	vec3 road(  0.2f, 0.2f, 0.2f );

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
	triangles.push_back( Triangle( C, B, A, road ) );
	triangles.push_back( Triangle( C, D, B, road ) );

	// ---------------------------------------------------------------------------
	// Buildings

	/*std::default_random_engine generator;
  std::normal_distribution<float> distribution(L/2,L/2);*/

	int x = 0;
	int z = 0;
	int width = 100;
	vec3 colour;

	while (x < L)
	{
		while (z < L)
		{
			if (x % 500 != 0 && z % 500 != 0) {
				srand(time(NULL) * x + z);

				int randHeight = rand() % 500 + 200;
				int randWidth = rand() % 40 + 60;
				int widthGap = (width - randWidth)/2;

				A = vec4(x+widthGap,0,z+width-widthGap,1);
				B = vec4(x+widthGap,0,z+widthGap,1);
				C = vec4(x+width-widthGap,0,z+width-widthGap,1);
				D = vec4(x+width-widthGap,0,z+widthGap,1);

				E = vec4(x+widthGap,randHeight,z+width-widthGap,1);
				F = vec4(x+widthGap,randHeight,z+widthGap,1);
				G = vec4(x+width-widthGap,randHeight,z+width-widthGap,1);
				H = vec4(x+width-widthGap,randHeight,z+widthGap,1);

				int randColour = rand() % 3;
				if (randColour == 0) colour = building1;
				if (randColour == 1) colour = building2;
				if (randColour == 2) colour = building3;


				// Front
				triangles.push_back( Triangle(E,B,A,colour) );
				triangles.push_back( Triangle(E,F,B,colour) );

				// Front
				triangles.push_back( Triangle(F,D,B,colour) );
				triangles.push_back( Triangle(F,H,D,colour) );

				// BACK
				triangles.push_back( Triangle(H,C,D,colour) );
				triangles.push_back( Triangle(H,G,C,colour) );

				// LEFT
				triangles.push_back( Triangle(G,E,C,colour) );
				triangles.push_back( Triangle(E,A,C,colour) );

				// TOP
				triangles.push_back( Triangle(G,F,E,colour) );
				triangles.push_back( Triangle(G,H,F,colour) );
			}
			z += width;
		}
		x += width;
		z = 0;
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
