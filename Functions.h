#pragma once
#include "Spine.h"
#include "Buttons.h"

void SpawnBlocksOptimised(IMesh* &cubeMesh, vector<IModel*> &cubeModel, IModel* &cubeModelNode,
							IMesh* &treeMesh, IMesh* &darkTreeMesh, IMesh* &snowTreeMesh, vector<IModel*> &treeModel, vector<IModel*> &darkTreeModel, vector<IModel*> &snowTreeModel, IModel* &treeModelNode,
							IMesh* &grassMesh, vector<IModel*> &grassModel,
							  const int &mapDistance,    //Needed so convert the negative values to 0 so we know where the player is
							  const int &renderDistance, //So the algorithm knows where to start looking
							  const int &zModifier,      //How far along the Z-Axis is the player currently
							  const int &xModifier       //How far along the X-Axis is the player currently
						  )
{ // Only used at startup to spawn in all the necessary blocks
	int Z = zModifier - renderDistance; //10 blocks behind your current spot on the Z-axis
	int X = xModifier - renderDistance; //10 blocks behind your current spot on the X-axis
	int zLimit = Z + (renderDistance << 1); //10 blocks ahead of your current spot on the Z-axis
	int xLimit = X + (renderDistance << 1); //10 blocks ahead of your current spot on the X-axis
	int j = 0;
	int calc;
	for (; Z < zLimit; Z++)
	{ //Spawning along the Z axis
		if (Z < -mapDistance) // Allows the map to loop along the Z-Axis
		{
			Z += mapDistance << 1;
			zLimit += mapDistance << 1;
		}
		else if (Z > mapDistance)
		{
			Z -= mapDistance << 1;
			zLimit -= mapDistance << 1;
		}
		for (; X < xLimit; X++, j++)
		{ //Spawning along the X axis
			if (X < -mapDistance) // Allows the map to loop along the X-Axis
			{
				X += mapDistance << 1;
				xLimit += mapDistance << 1;
			}
			else if (X > mapDistance)
			{
				X -= mapDistance << 1;
				xLimit -= mapDistance << 1;
			}
			calc = ((mapDistance << 1) * (Z + mapDistance)) + (X + mapDistance); // Convert the array to the map co-ordinate
			cubeModelNode = cubeMesh->CreateModel((float)cube[calc].xBlock * 10, cube[calc].yPos, (float)cube[calc].zBlock * 10);
			cubeModel.push_back(cubeModelNode);

			if (cube[calc].tree == 1 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{ // Each one of these spawns a tree or grass depending on what is needed
				treeModelNode = darkTreeMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				darkTreeModel.push_back(treeModelNode);
			}
			else if (cube[calc].tree == 2 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{
				treeModelNode = treeMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				treeModel.push_back(treeModelNode);
			}
			else if (cube[calc].tree == 3 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{
				treeModelNode = snowTreeMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				snowTreeModel.push_back(treeModelNode);
			}
			else if (cube[calc].grass == 1 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{
				treeModelNode = grassMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				grassModel.push_back(treeModelNode);
			}

			switch (cube[calc].biome)
			{ // Set grass texture based off of biome
			case 1:
				cubeModel[j]->SetSkin("grass.png");
				break;
			case 2:
				cubeModel[j]->SetSkin("darkGrass.png");
				break;
			case 3:
				cubeModel[j]->SetSkin("snow.png");
				break;
			}
		}
		X -= (renderDistance << 1);
	}
}

void OptimisedTerrain(vector<IModel*> &cubeModel,
	const int &mapDistance,    //Needed so convert the negative values to 0 so we know where the player is
	const int &renderDistance, //So the algorithm knows where to start looking
	const int &zModifier,      //How far along the Z-Axis is the player currently
	const int &xModifier,      //How far along the X-Axis is the player currently
	const int &oldZModifier,   //How far along the Z-Axis was the player before
	const int &oldXModifier,   //How far along the X-Axis was the player before
	const float &halfCubeSize, //Used to measure out the distance before getting to another cube position
	const bool &heightMap,     //Used to check what kind of texture mode the cube is on
	const int &direction = 5   //0, 1, 2, 3, 5 = Left, Down, Right, Up, Still | Which way did the player move?
)
{ // Move the terrain with the player
	int Z = zModifier - renderDistance; //All visible blocks behind your current spot on the Z-axis vector to grid conversion
	int X = xModifier - renderDistance; //All visible blocks behind your current spot on the X-axis vector to grid conversion
	int zLimit = Z + (renderDistance << 1); //10 blocks ahead of your current spot on the Z-axis
	int xLimit = X + (renderDistance << 1); //10 blocks ahead of your current spot on the X-axis

	int oldZ = oldZModifier - renderDistance; //All visible blocks behind your current spot on the Z-axis vector to grid conversion
	int oldX = oldXModifier - renderDistance; //All visible blocks behind your current spot on the X-axis vector to grid conversion

	int j = 0;
	int oldCalc;
	int calc;
	for (; Z < zLimit; Z++, oldZ++)
	{
		if (Z < -mapDistance) // Repeats the map along the Z-Axis
		{
			Z += mapDistance << 1;
			zLimit += mapDistance << 1;
		}
		else if (Z > mapDistance)
		{
			Z -= mapDistance << 1;
			zLimit -= mapDistance << 1;
		}
		for (; X < xLimit; X++, oldX++, j++)
		{
			if (X < -mapDistance) // Repeats the map along the X-Axis
			{
				X += mapDistance << 1;
				xLimit += mapDistance << 1;
			}
			else if (X > mapDistance)
			{
				X -= mapDistance << 1;
				xLimit -= mapDistance << 1;
			}
			oldCalc = ((mapDistance << 1) * (oldZ + mapDistance)) + (oldX + mapDistance); // Calculate the position of the cube in the array before movement
			calc = ((mapDistance << 1) * (Z + mapDistance)) + (X + mapDistance); // Calculate the position of the cube in the array
			cubeModel[j]->SetY((cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4));

			if (cube[calc].biome != cube[oldCalc].biome && !heightMap)
			{ // Set the cubes biome skin
				skin:
				switch (cube[calc].biome)
				{
				case 1:
					cubeModel[j]->SetSkin("darkGrass.png");
					break;
				case 2:
					cubeModel[j]->SetSkin("grass.png");
					break;
				case 3:
					cubeModel[j]->SetSkin("snow.png");
					break;
				}
			}
			else if (cube[calc].yTier != cube[oldCalc].yTier && heightMap)
			{ // Set the cubes height skin
				hSkin:
				switch (cube[calc].yTier)
				{
				case 0:
					cubeModel[j]->SetSkin("HeightMap_0.png");
					break;
				case 1:
					cubeModel[j]->SetSkin("HeightMap_1.png");
					break;
				case 2:
					cubeModel[j]->SetSkin("HeightMap_2.png");
					break;
				case 3:
					cubeModel[j]->SetSkin("HeightMap_3.png");
					break;
				case 4:
					cubeModel[j]->SetSkin("HeightMap_4.png");
					break;
				}
			}
		}
		oldX -= renderDistance << 1;
		X -= renderDistance << 1;
	}

	switch (direction)
	{ // Move the cubes based on where the player moved
	case 5:
		return;
	case 0: // Left
		for (int i = 0; i != cubeModel.size(); i++)
			cubeModel[i]->MoveLocalX(-halfCubeSize);
		break;
	case 1: // Down
		for (int i = 0; i != cubeModel.size(); i++)
			cubeModel[i]->MoveLocalZ(-halfCubeSize);
		break;
	case 2: // Right
		for (int i = 0; i != cubeModel.size(); i++)
			cubeModel[i]->MoveLocalX(halfCubeSize);
		break;
	case 3: // Up
		for (int i = 0; i != cubeModel.size(); i++)
			cubeModel[i]->MoveLocalZ(halfCubeSize);
		break;
	}
}

void PrepareResize(IMesh* &cubeMesh, vector<IModel*> &cubeModel, IModel* &cubeModelNode, const int &mapDistance, const int &renderDistance, const int &zModifier, const int &xModifier)
{
	int Z = zModifier - renderDistance; //All visible blocks behind your current spot on the Z-axis vector to grid conversion
	int X = xModifier - renderDistance; //All visible blocks behind your current spot on the X-axis vector to grid conversion
	int zLimit = Z + (renderDistance << 1); //10 blocks ahead of your current spot on the Z-axis
	int xLimit = X + (renderDistance << 1); //10 blocks ahead of your current spot on the X-axis
	int j = 0;
	int calc;

	for (; Z < zLimit; Z++)
	{
		if (Z < -mapDistance) // Repeats the map along the Z-Axis
		{
			Z += mapDistance << 1;
			zLimit += mapDistance << 1;
		}
		else if (Z > mapDistance)
		{
			Z -= mapDistance << 1;
			zLimit -= mapDistance << 1;
		}
		for (; X < xLimit; X++, j++)
		{
			if (X < -mapDistance) // Repeats the map along the X-Axis
			{
				X += mapDistance << 1;
				xLimit += mapDistance << 1;
			}
			else if (X > mapDistance)
			{
				X -= mapDistance << 1;
				xLimit -= mapDistance << 1;
			}
			calc = ((mapDistance << 1) * (Z + mapDistance)) + (X + mapDistance); // Calculate the position of the cube in the array
			if (j == cubeModel.size())
			{
				cubeModelNode = cubeMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				cubeModel.push_back(cubeModelNode);
			}
			else
				cubeModel[j]->SetPosition((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
		}
		X -= (renderDistance << 1);
	}

	while (j < cubeModel.size())
	{
		cubeMesh->RemoveModel(cubeModel.back());
		cubeModel.pop_back();
	}
}

void TextureSwap(vector<IModel*> &cubeModel,
					const int &mapDistance,    //Needed so convert the negative values to 0 so we know where the player is
					const int &renderDistance, //So the algorithm knows where to start looking
					const int &zModifier,      //How far along the Z-Axis is the player currently
					const int &xModifier,      //How far along the X-Axis is the player currently
					const bool &heightMap      //Used to check what kind of texture mode the cube is on
				)
{
	int Z = zModifier - renderDistance; //10 blocks behind your current spot on the Z-axis
	int X = xModifier - renderDistance; //10 blocks behind your current spot on the X-axis
	int zLimit = Z + (renderDistance << 1); //10 blocks ahead of your current spot on the Z-axis
	int xLimit = X + (renderDistance << 1); //10 blocks ahead of your current spot on the X-axis

	int j = 0;
	int calc;
	for (; Z < zLimit; Z++)
	{ //Spawning along the Z axis
		if (Z < -mapDistance)
		{
			Z += mapDistance << 1;
			zLimit += mapDistance << 1;
		}
		else if (Z > mapDistance)
		{
			Z -= mapDistance << 1;
			zLimit -= mapDistance << 1;
		}
		for (; X < xLimit; X++, j++)
		{ //Spawning along the X axis
			if (X < -mapDistance)
			{
				X += mapDistance << 1;
				xLimit += mapDistance << 1;
			}
			else if (X > mapDistance)
			{
				X -= mapDistance << 1;
				xLimit -= mapDistance << 1;
			}
			calc = ((mapDistance << 1) * (Z + mapDistance)) + (X + mapDistance);

			if (!heightMap)
			{ // Set the cubes biome skin
				switch (cube[calc].biome)
				{
				case 1:
					cubeModel[j]->SetSkin("darkGrass.png");
					break;
				case 2:
					cubeModel[j]->SetSkin("grass.png");
					break;
				case 3:
					cubeModel[j]->SetSkin("snow.png");
					break;
				}
			}
			else
			{ // Set the cubes height skin
				switch (cube[calc].yTier)
				{
				case 0:
					cubeModel[j]->SetSkin("HeightMap_0.png");
					break;
				case 1:
					cubeModel[j]->SetSkin("HeightMap_1.png");
					break;
				case 2:
					cubeModel[j]->SetSkin("HeightMap_2.png");
					break;
				case 3:
					cubeModel[j]->SetSkin("HeightMap_3.png");
					break;
				case 4:
					cubeModel[j]->SetSkin("HeightMap_4.png");
					break;
				}
			}
		}
		X -= (renderDistance << 1);
	}
}

void TreeSpawn(IMesh* &treeMesh, IMesh* &darkTreeMesh, IMesh* &snowTreeMesh, vector<IModel*> &treeModel, vector<IModel*> &darkTreeModel, vector<IModel*> &snowTreeModel, IModel* &treeModelNode,
					IMesh* &grassMesh, vector<IModel*> &grassModel,
					const int &mapDistance,    //Needed so convert the negative values to 0 so we know where the player is
					const int &renderDistance, //So the algorithm knows where to start looking
					const int &zModifier,      //How far along the Z-Axis is the player currently
					const int &xModifier,      //How far along the X-Axis is the player currently
					const int &density         //How many trees are spawning
			  )
{
    int Z = zModifier - renderDistance; //10 blocks behind your current spot on the Z-axis
	int X = xModifier - renderDistance; //10 blocks behind your current spot on the X-axis
	int zLimit = Z + (renderDistance << 1); //10 blocks ahead of your current spot on the Z-axis
	int xLimit = X + (renderDistance << 1); //10 blocks ahead of your current spot on the X-axis

	int calc;
	int i = 0;
	int j = 0;
	int l = 0;
	int g = 0;
	for (; Z < zLimit; Z++)
	{ //Spawning along the Z axis
		if (Z < -mapDistance)
		{
			Z += mapDistance << 1;
			zLimit += mapDistance << 1;
		}
		else if (Z > mapDistance)
		{
			Z -= mapDistance << 1;
			zLimit -= mapDistance << 1;
		}
		for (; X < xLimit; X++)
		{ //Spawning along the X axis
			if (X < -mapDistance)
			{
				X += mapDistance << 1;
				xLimit += mapDistance << 1;
			}
			else if (X > mapDistance)
			{
				X -= mapDistance << 1;
				xLimit -= mapDistance << 1;
			}
			calc = ((mapDistance << 1) * (Z + mapDistance)) + (X + mapDistance);
			if (cube[calc].tree == 1 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{ // Forest tree
				if (i == darkTreeModel.size())
				{ // If there aren't enough trees on the map, it adds to them
					treeModelNode = darkTreeMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
					darkTreeModel.push_back(treeModelNode);
				}
				else
					darkTreeModel[i]->SetPosition((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				i++;
			}
			else if (cube[calc].tree == 2 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{ // Normal tree
				if (j == treeModel.size())
				{ // If there aren't enough trees on the map, it adds to them
					treeModelNode = treeMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
					treeModel.push_back(treeModelNode);
				}
				else
					treeModel[j]->SetPosition((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				j++;
			}
			else if (cube[calc].tree == 3 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{ // Snowy tree
				if (l == snowTreeModel.size())
				{ // If there aren't enough trees on the map, it adds to them
					treeModelNode = snowTreeMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
					snowTreeModel.push_back(treeModelNode);
				}
				else
					snowTreeModel[l]->SetPosition((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				l++;
			}
			else if (cube[calc].grass == 1 && (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4) > -100)
			{ // Tall grass
				if (g == grassModel.size())
				{ // If there isn't enough tall grass on the map, it adds to it
					treeModelNode = grassMesh->CreateModel((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
					grassModel.push_back(treeModelNode);
				}
				else
					grassModel[g]->SetPosition((float)cube[calc].xBlock * 10, (cube[calc].yPos - (powf(CPerlin.GetOctaves(), 2) / 2)) - (CPerlin.GetBias() * 4), (float)cube[calc].zBlock * 10);
				g++;
			}
		}
		X -= (renderDistance << 1);
	}
	while (i < darkTreeModel.size())
	{ // Shortening the tree array to compensate
		darkTreeMesh->RemoveModel(darkTreeModel.back());
		darkTreeModel.pop_back();
	}
	while (j < treeModel.size())
	{
		treeMesh->RemoveModel(treeModel.back());
		treeModel.pop_back();
	}
	while (l < snowTreeModel.size())
	{
		snowTreeMesh->RemoveModel(snowTreeModel.back());
		snowTreeModel.pop_back();
	}
	while (g < grassModel.size())
	{
		grassMesh->RemoveModel(grassModel.back());
		grassModel.pop_back();
	}
}

void Decelerate(const float &acceleration, float &cubeSpeed, const float &fTime)
{ // Slows the cube down
	if (cubeSpeed > acceleration * fTime) //Anti-forward
		cubeSpeed -= acceleration * fTime;
	else if (cubeSpeed < -acceleration * fTime) //Anti-backward
		cubeSpeed += acceleration * fTime;
	else
		cubeSpeed = 0.0f;
}

void FnButtonSwap(I3DEngine* &myEngine, ClButton* &button, ISprite* &sprite, bool &sprActive, const string &fileName, const float &xPos, const float &yPos)
{ // Changing the button from normal to down and back
	if (button->FnButtonState())
		if (sprActive)
		{
			myEngine->RemoveSprite(sprite);
			sprite = myEngine->CreateSprite(fileName + " down.png", xPos, yPos);
			sprActive = false;
		}
	if (!button->FnButtonState())
		if (!sprActive)
		{
			myEngine->RemoveSprite(sprite);
			sprite = myEngine->CreateSprite(fileName + ".png", xPos, yPos);
			sprActive = true;
		}
}
