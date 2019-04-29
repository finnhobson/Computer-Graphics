#include <glm/glm.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <random>
#include "GenerateCity.h"

using glm::vec3;
using glm::vec4;

// Procedurally generates a city model.

float L = 5000;
size_t numTriangles = 0;

void GenerateModel( std::vector<Triangle>& triangles, int cityX, int cityZ )
{
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

	//triangles.clear();
	//triangles.reserve( 5*2*500 + 2 );

	// ---------------------------------------------------------------------------
	// Room

	float outer = 7500;

	vec4 A(outer,0,0,1);
	vec4 B(0,0,0,1);
	vec4 C(outer,0,outer,1);
	vec4 D(0,0,outer,1);

	vec4 E, F, G, H, I;

	// Floor:
	//triangles.push_back( Triangle( C, B, A, road ) );
	//triangles.push_back( Triangle( C, D, B, road ) );

	// ---------------------------------------------------------------------------
	// Buildings

	/*std::default_random_engine generator;
  std::normal_distribution<float> distribution(L/2,L/2);*/

	int x = 0;
	int z = 0;
	int width = 150;
	vec3 colour;
	numTriangles = triangles.size();

	while (x < L+width)
	{
		while (z < L+width)
		{
			if (x % 750 != 0 && z % 750 != 0) {
				srand(time(NULL) * x + z);
				int normHeight = (L - abs(L/2 - x) - abs(L/2 - z)) / 6;
				int randWidth = rand() % 60 + 90;

				int randHeight = rand() % normHeight + 20;
				if (randHeight > normHeight && randHeight > 600) {
					randHeight += 200;
					randWidth = width;
				}

				int widthGap = (width - randWidth)/2;

				A = vec4(x+widthGap+cityX*(L+width*1.7f),0,z+width-widthGap+cityZ*(L+width*1.7f),1);
				B = vec4(x+widthGap+cityX*(L+width*1.7f),0,z+widthGap+cityZ*(L+width*1.7f),1);
				C = vec4(x+width-widthGap+cityX*(L+width*1.7f),0,z+width-widthGap+cityZ*(L+width*1.7f),1);
				D = vec4(x+width-widthGap+cityX*(L+width*1.7f),0,z+widthGap+cityZ*(L+width*1.7f),1);

				E = vec4(x+widthGap+cityX*(L+width*1.7f),randHeight,z+width-widthGap+cityZ*(L+width*1.7f),1);
				F = vec4(x+widthGap+cityX*(L+width*1.7f),randHeight,z+widthGap+cityZ*(L+width*1.7f),1);
				G = vec4(x+width-widthGap+cityX*(L+width*1.7f),randHeight,z+width-widthGap+cityZ*(L+width*1.7f),1);
				H = vec4(x+width-widthGap+cityX*(L+width*1.7f),randHeight,z+widthGap+cityZ*(L+width*1.7f),1);

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

				int pointedTop = rand() % 4;
				if (randHeight > 600 && pointedTop == 0) {
					I = vec4(x+width/2+cityX*(L+width*1.7f), randHeight + 50, z+width/2+cityZ*(L+width*1.7f), 1);

					triangles.push_back( Triangle(I,E,F,colour) );
					triangles.push_back( Triangle(I,E,G,colour) );
					triangles.push_back( Triangle(I,F,H,colour) );
					triangles.push_back( Triangle(I,G,H,colour) );
				}
			}
			z += width;
		}
		x += width;
		z = 0;
	}


	// ----------------------------------------------
	// Scale to the volume [-1,1]^3



	for( size_t i=numTriangles; i<triangles.size(); ++i )
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

	/*for( size_t i=0; i<2; ++i )
	{
		triangles[i].v0 += vec4(0.5,0,-0.5,0);
		triangles[i].v1 += vec4(0.5,0,-0.5,0);
		triangles[i].v2 += vec4(0.5,0,-0.5,0);
	}*/
}

void GenerateCars( std::vector<Car>& cars ) {
	for ( unsigned int i = 0; i < cars.size(); i++ ) {
    if (i < cars.size()*0.5f) {
			int randX = ((rand() % 28) -14) * 750;
			int randZ = rand() % 20000;
			cars[i].position = vec4(randX, 5, randZ, 0);
			if (i < cars.size() * 0.25f) {
				cars[i].colour = vec3(3,3,3);
				cars[i].position += vec4(50, 0, 0, 0);
				cars[i].movement = vec4(0, 0, -1, 0);
			}
			else {
				cars[i].colour = vec3(3,0,0);
				cars[i].position += vec4(100,0,0,0);
				cars[i].movement = vec4(0, 0, 1, 0);
			}
		}
		else {
			int randZ = (rand() % 21) * 750;
			int randX = rand() % 10000;
			cars[i].position = vec4(randX, 5, randZ, 0);
			if (i < cars.size() * 0.75f) {
				cars[i].colour = vec3(3,3,3);
				cars[i].position += vec4(0, 0, 50, 0);
				cars[i].movement = vec4(-1, 0, 0, 0);
			}
			else {
				cars[i].colour = vec3(3,0,0);
				cars[i].position += vec4(0,0,100,0);
				cars[i].movement = vec4(1, 0, 0, 0);
			}
		}
  }

	for( size_t i=0; i<cars.size(); ++i )
	{
		cars[i].position *= 2/L;
		cars[i].position -= vec4(1,1,1,1);
		cars[i].position.x *= -1;
		cars[i].position.y *= -1;
		cars[i].position.w = 1.0;
	}
}

void GenerateLights( std::vector<glm::vec4>& lights, int cityX, int cityZ ) {
	int numLights = lights.size();

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 35; j++ ) {
				lights.push_back( vec4(i*750 + 20 + cityX*(L+250), 5, L/33 * j + cityZ*(L+250), 1) );
				lights.push_back( vec4(i*750 + 130 + cityX*(L+250), 5, L/33 * j + cityZ*(L+250), 1) );
				lights.push_back( vec4(L/33 * j + cityX*(L+250), 5, i*750 + 20 + cityZ*(L+250), 1) );
				lights.push_back( vec4(L/33 * j + cityX*(L+250), 5, i*750 + 130 + cityZ*(L+250), 1) );
		}
	}

	for( size_t i=numLights; i<lights.size(); ++i )
	{
		lights[i] *= 2/L;
		lights[i] -= vec4(1,1,1,1);
		lights[i].x *= -1;
		lights[i].y *= -1;
		lights[i].w = 1.0;
	}
}
