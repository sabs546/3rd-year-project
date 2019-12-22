#pragma once
#include <TL-Engine.h>
#include <vector>
#include <time.h>

using namespace tle;

//------- Quick array -------//
const int x = 0; // 0 | Used for X in pos array
const int y = 1; // 1 | Used for Y in pos array
const int z = 2; // 2 | Used for Z in pos array

// Structures -------
struct player
{
	float xSpeed = 0.0f; // 0.0f | Current X-Axis speed of the cube
	float zSpeed = 0.0f; // 0.0f | Current Z-Axis speed of the cube
	float pos[3]; // [X][Y][Z] = [0][1][2] | The cubes co-ordinates
};

struct cubes
{
	int   zBlock;   // Z position represented in relation to cube sizes
	int   xBlock;   // X position represented in relation to cube sizes
	int   yTier;    // Y position but grouped up and into 5 broad tiers
	float yPos;     // Y position is stored here after generating noise
	int   biome;    // 0/1/2 = Green, Normal, Snowy | Setting the biome
	int   tree;     // 0/1/2 = The chance out of 10 for a tree to spawn
	int   grass;    // Variable decides if grass spawns on a coordinate
};

// Objects -------
player playerCube;

cubes cubeN;
vector<cubes> cube;

I3DEngine* myEngine = New3DEngine(kTLX);

//---------------------------------------------------------
// Perlin Noise
// Main Class -------
class CCPerlinNoiseMap
{ //Class to make perlin noise a little more portable
private:
	int sizeX;                   // Width of the area ------------------ //
	int sizeZ;                   // Height of the area ----------------- //
	int octaves;                 // Complexity ------------------------- //
	int biomeOctaves;            // Biomes Complexity ------------------ //
	float scalingBias;           // Amplification ---------------------- //
	float biomeScaling;          // Amplificaiton ---------------------- //
	float *noiseSeed = nullptr;  // Random seed ------------------------ //
	float *perlinSeed = nullptr; // Perlin noise version of the seed --- //
	float *noiseSeed2 = nullptr; // Random seed for the biome ---------- //
	float *biomeSeed = nullptr;  // Biome noise version of the seed ---- //
// --------------------------------------------------------------------- //
public:
	void setAllValues(const int &x, const int &y, const int &octs, const float &scbias)
	{ // Allows you to set all values before you begin
		sizeX = x;
		sizeZ = y;
		octaves = octs;
		biomeOctaves = 7;
		scalingBias = scbias;
		biomeScaling = 50;
		// Fill the seeds with empty values
		noiseSeed = new float[x * y];
		perlinSeed = new float[x * y];
		noiseSeed2 = new float[x * y];
		biomeSeed = new float[x * y];
		srand(time(0));

		for (int i = 0; i < x * y; i++)
		{ // Fills the seed with real values
			noiseSeed[i] = (float)rand() / (float)RAND_MAX;
		}
		for (int i = 0; i < x * y; i++)
		{ // Fills the seed with real values
			noiseSeed2[i] = rand() % 30;
		}
	}

	void perlinNoise(float *nseed, float *output, const int &nOctaves, const float &scb, const bool &mode = false)
	{ // Where it becomes perlin noise instead of standard noise
		for (int X = 0; X < sizeX; X++)
		{ // Goes along the X-Axis
			for (int Z = 0; Z < sizeZ; Z++)
			{ // Goes along the Z-Axis
				float noise = 0.0f;
				float scale = 1.0f; // Amplitude of the world
				for (int o = 0; o < nOctaves; o++)
				{ // Loops by octave count
					int pitch = sizeX >> o; // Divides by octaves to decide how many samples there are
					int sx = (X / pitch) * pitch; // Divides then multiplies by pitch to get the sample while still accounting for integers sometimes losing information when halved
					int sz = (Z / pitch) * pitch;

					int sx2 = (sx + pitch) % sizeX; // Gets the next sample by adding pitch, otherwise uses the remainder to wrap around
					int sz2 = (sz + pitch) % sizeZ;

					float blendX = (float)(X - sx) / (float)pitch; // Get a result between 0 and the pitch, dividing this by the pitch gets the value between 0 and 1
					float blendZ = (float)(Z - sz) / (float)pitch;

					float sampleT;
					float sampleB;

					if (!mode)
					{
						sampleT = (cos((1 - blendX) * nseed[sz * sizeX + sx] + blendX * nseed[sz * sizeX + sx2])) / 2;
						sampleB = (cos((1 - blendX) * nseed[sz2 * sizeX + sx] + blendX * nseed[sz2 * sizeX + sx2])) / 2;
					}
					else
					{
						sampleT = (1.0f - blendX) * nseed[sz * sizeX + sx] + blendX * nseed[sz * sizeX + sx2]; // Taking the last value, then multiplying by two values added together in the noise array
						sampleB = (1.0f - blendX) * nseed[sz2 * sizeX + sx] + blendX * nseed[sz2 * sizeX + sx2];
					}

					noise += (blendZ * (sampleB - sampleT) + sampleT) * scb; // Stacks the noise onto itselfite
					//noise += (sampleT * (1 - blendX) + sampleB + sampleB * blendZ) * scb;

					scale /= scb / 2; // The scale get divided by the bias and halved
				}
				output[Z * sizeX + X] = noise; // Put the value into the output array
			}
		}
	}

	void Update(const int &Key, int &octCount, const int &density)
	{
		if (Key == 1) // The keys decide if the function performs any extra tasks
			scalingBias += 50.0f;
		if (Key == 2) // Scaling bias will amplify the world
			scalingBias -= 50.0f;

		if (scalingBias < 0.0f)
			scalingBias = 0.0f;

		// New calculations need to be made for the map when the values are changed
		perlinNoise(noiseSeed, perlinSeed, octaves, scalingBias);
		perlinNoise(noiseSeed2, biomeSeed, biomeOctaves, biomeScaling, true);

		for (int i = 0; i != cube.size(); i++)
		{
			// Setting seeds
			cube[i].yPos = perlinSeed[i];
			cube[i].biome = biomeSeed[i];

			// Setting biomes
			if (cube[i].biome >= 4000 && cube[i].biome < 5800)
			{ // Set the forest biome
				if (cube[i].biome > 5300)
				{ // Spawn less trees when closing in on the normal biome
					cube[i].tree = rand() % 10;
					cube[i].grass = rand() % 5;
				}
				else if (cube[i].biome > 5200)
				{ // Slowly upping the number as it gets deeper into the forest biome
					cube[i].tree = rand() % 32;
					cube[i].grass = rand() % 20;
				}
				else
				{
					cube[i].tree = rand() % 64;
					cube[i].grass = rand() % 50;
				}
				cube[i].biome = 1;
			}
			else if (cube[i].biome >= 5800)
			{ // Plains biome
				cube[i].biome = 2;
				cube[i].tree = rand() % 4;
				cube[i].grass = 0;
			}
			else
			{ // Snowy biome
				if (cube[i].biome > 3700)
				{
					cube[i].tree = rand() % 32;
					cube[i].biome = 3;
				}
				else if (cube[i].biome > 3500)
				{
					cube[i].tree = rand() % 10;
					cube[i].biome = 3;
				}
				else
				{
					cube[i].biome = 3;
					cube[i].tree = rand() % 2;
				}
				cube[i].grass = 0;
			}

			if (rand() % 200 * density < cube[i].tree)
			{ // Decide if the tree spawns based off the number given by the biome
				cube[i].tree = cube[i].biome;
			}
			else
			{ // If not, see if there is a chance for the grass to spawn
				cube[i].tree = 0;
				if (rand() % 200 * density < cube[i].grass)
				{
					cube[i].grass = 1;
				}
			}

			// Setting height tiers
			float minPos = 1100;
			float maxPos = 1150;
			for (int y = 0; y < 5; y++, maxPos += 25, minPos += 25)
			{ // Change the colour based off of the height
				if (cube[i].yPos >= minPos && cube[i].yPos < maxPos)
				{
					cube[i].yTier = y;
					break;
				}
				else if (cube[i].yPos < 1100)
					cube[i].yTier = 0;
				else
					cube[i].yTier = 4;
			}
		}
	}

	int GetOctaves()
	{
		return octaves;
	}

	float GetBias()
	{
		return scalingBias;
	}

	void GetWorld(float world[])
	{
		for (int i = 0; i < sizeX * sizeZ; i++)
			world[i] = noiseSeed[i];
	}

	void SetWorld(float world[])
	{
		for (int i = 0; i < sizeX * sizeZ; i++)
			noiseSeed[i] = world[i];
	}

	void GetBiome(float foliage[])
	{
		for (int i = 0; i < sizeX * sizeZ; i++)
			foliage[i] = noiseSeed2[i];
	}

	void SetBiome(float foliage[])
	{
		for (int i = 0; i < sizeX * sizeZ; i++)
			noiseSeed2[i] = foliage[i];
	}

	void GetSeed(float world[], float foliage[])
	{
		for (int i = 0; i < sizeX * sizeZ; i++)
			world[i] = noiseSeed[i];

		for (int i = 0; i < sizeX * sizeZ; i++)
			foliage[i] = noiseSeed2[i];
	}

	void SetSeed(float world[], float foliage[])
	{
		for (int i = 0; i < sizeX * sizeZ; i++)
			noiseSeed[i] = world[i];

		for (int i = 0; i < sizeX * sizeZ; i++)
			noiseSeed2[i] = foliage[i];
	}

	void AddOctaves(const int &density)
	{ // Octaves can not go over 10 to avoid crashing
		if (octaves < 10)
			++octaves;
		Update(3, octaves, density);
	}

	void RemoveOctaves(const int &density)
	{ // Not having enough octaves looks too flat
		if (octaves > 7)
			--octaves;
		Update(3, octaves, density);
	}
};
CCPerlinNoiseMap CPerlin;
