#pragma once
#include "Spine.h"
#include <fstream>
#include <string>

//=========================================================
// BUTTON CLASS
// Main Class =======
int mousePos[2];
const int worldButtons = 7;
const int saveButtons = 6;

class ClButton
{
protected:
	int PrPos[2]; //Position of the button
	int xSize; //X length of the button
	int ySize; //Y length of the button
public:
	void FnSetButtonPos(const int &buttonX, const int &buttonY);
	bool FnButtonState();
	virtual void FnSetButtonSize() = 0;
	virtual void Execute(const int &mapLimit, int &octaveCount, bool &textureMode, int &density, const int &mode) = 0;
	virtual void SaveWorld(const int &save, const int &mode) = 0;
};
// Member Functions -------
void ClButton::FnSetButtonPos(const int &buttonX, const int &buttonY)
{
	PrPos[0] = buttonX;
	PrPos[1] = buttonY;
}
bool ClButton::FnButtonState()
{
	if (mousePos[0] >= PrPos[0] && mousePos[0] <= PrPos[0] + xSize &&
		mousePos[1] >= PrPos[1] && mousePos[1] <= PrPos[1] + ySize)
		return true;
	else
		return false;
}
void ClButton::FnSetButtonSize() {}
void ClButton::Execute(const int &mapLimit, int &octaveCount, bool &textureMode, int &density, const int &mode) {}
void ClButton::SaveWorld(const int &save, const int &mode) {}

// World Sub-classes =======
class SCWorldButton : public ClButton
{ // Add octaves to the map
public:
	void FnSetButtonSize()
	{
		xSize = 160;
		ySize = 90;
	}
	void Execute(const int &mapLimit, int &octaveCount, bool &textureMode, int &density, const int &mode)
	{
	    switch (mode)
	    {
            case 0:
                CPerlin.AddOctaves(density);
                break;
            case 1:
                CPerlin.RemoveOctaves(density);
                break;
            case 2:
                srand(time(0));
                CPerlin.setAllValues(mapLimit << 1, mapLimit << 1, octaveCount, 300);
                CPerlin.Update(3, octaveCount, density);
                break;
            case 3:
                (textureMode == false) ? textureMode = true : textureMode = false;
                break;
            case 4:
                density++;
                if (density > 5)
                    density = 1;
                CPerlin.Update(3, octaveCount, density);
                break;
            case 5:
                CPerlin.Update(1, octaveCount, density);
                break;
            case 6:
                CPerlin.Update(2, octaveCount, density);
                break;
	    }
	}
	void SaveWorld(const int &save, const int &mode) {}
};

// Save Sub-class =======
class SCSaveGame : public ClButton
{ // Saving for world and biomes
	void FnSetButtonSize()
	{
		xSize = 100;
		ySize = 35;
	}
	void Execute(const int &mapLimit, int &octaveCount, bool &textureMode, int &density, const int &mode) {}
	void SaveWorld(const int &save, const int &mode)
	{
		ofstream saveFile("Saves/World " + to_string(save) + ".txt");
		if (saveFile.is_open())
		{ // - World -
			float *world = nullptr;
			world = new float[cube.size()];
			CPerlin.GetWorld(world);

			for (int i = 0; i != cube.size(); i++)
				saveFile << world[i] << " ";
			saveFile.close();
		}
		ofstream biomeFile("Saves/Biome " + to_string(save) + ".txt");
		if (biomeFile.is_open())
		{ // - Biome -
			float *biome = nullptr;
			biome = new float[cube.size()];
			CPerlin.GetBiome(biome);

			for (int i = 0; i != cube.size(); i++)
				biomeFile << biome[i] << " ";
			biomeFile.close();
		}
	}
};

// Load Sub-class =======
class SCLoadGame : public ClButton
{ // Loading for world and biomes
	void FnSetButtonSize()
	{
		xSize = 100;
		ySize = 35;
	}
	void Execute(const int &mapLimit, int &octaveCount, bool &textureMode, int &density, const int &mode) {}
	void SaveWorld(const int &save, const int &mode)
	{
		if (mode != 1)
		{
			ifstream saveFile("Saves/World " + to_string(save) + ".txt");
			float *world = nullptr;
			world = new float[cube.size()];

			if (saveFile.is_open())
			{ // - World -
				int i = 0;
				while (saveFile >> world[i])
					i++;
				CPerlin.SetWorld(world);
				saveFile.close();
			}
			else
				return;
		}

		if (mode != 0)
		{ // - Biome -
			ifstream biomeFile("Saves/Biome " + to_string(save) + ".txt");
			float *biome = nullptr;
			biome = new float[cube.size()];

			if (biomeFile.is_open())
			{
				int i = 0;
				while (biomeFile >> biome[i])
					i++;
				CPerlin.SetBiome(biome);
				biomeFile.close();
			}
			else
				return;
		}
	}
};

SCWorldButton addOctave;
SCWorldButton removeOctave;
SCWorldButton newSeed;
SCWorldButton heightMap;
SCWorldButton foliageDensity;
SCWorldButton addBias;
SCWorldButton removeBias;
ClButton* idxWorldButtons[worldButtons]
{
	&addOctave, &removeOctave, &newSeed, &heightMap, &foliageDensity, &addBias, &removeBias
};

SCSaveGame save0;
SCSaveGame save1;
SCSaveGame save2;
SCSaveGame save3;
SCSaveGame save4;
SCSaveGame save5;
ClButton* idxSaveButtons[saveButtons]
{
	&save0, &save1, &save2, &save3, &save4, &save5
};

SCLoadGame load0;
SCLoadGame load1;
SCLoadGame load2;
SCLoadGame load3;
SCLoadGame load4;
SCLoadGame load5;
ClButton* idxLoadButtons[saveButtons]
{
    &load0, &load1, &load2, &load3, &load4, &load5
};
