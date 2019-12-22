// Final version.cpp: A program using the TL-Engine

#include "Functions.h"
#include "Buttons.h"
#include <math.h>
#include <string>

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	myEngine->StartFullscreen(1920, 1080);

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("Media");
	myEngine->AddMediaFolder("Buttons/World");
	myEngine->AddMediaFolder("Buttons/Save");
	myEngine->AddMediaFolder("Buttons/Load");

	/**** Set up your scene here ****/
	// - Meshes -
	IMesh* cubeMesh;
	IMesh* playerMesh;
	IMesh* stageMesh;
	IMesh* treeMesh;
	IMesh* darkTreeMesh;
	IMesh* snowTreeMesh;
	IMesh* grassMesh;

	// - Models -
	IModel* playerModel;
	vector<IModel*> cubeModel;
	vector<IModel*> treeModel;
	vector<IModel*> darkTreeModel;
	vector<IModel*> snowTreeModel;
	vector<IModel*> grassModel;

	IModel* cubeModelN;
	IModel* treeModelN;

	IModel* floor[2];
	IModel* skyFloor;
	IModel* skybox;
	IFont* Text = myEngine->LoadFont("Font.bmp", 30u);

	// - Creating models -
	cubeMesh = myEngine->LoadMesh("Terrain.x");
	playerMesh = myEngine->LoadMesh("camera.x");
	playerModel = playerMesh->CreateModel(0.0f, 10.0f, 0.0f);

	treeMesh = myEngine->LoadMesh("tree.x");
	darkTreeMesh = myEngine->LoadMesh("dark spruce.x");
	snowTreeMesh = myEngine->LoadMesh("snow tree.x");

	grassMesh = myEngine->LoadMesh("grass.x");

	stageMesh = myEngine->LoadMesh("backwater.x");
	skyFloor = stageMesh->CreateModel(0, -120, 0);
	skyFloor->Scale(9);
	stageMesh = myEngine->LoadMesh("nearwater2.x");
	floor[1] = stageMesh->CreateModel(0.0f, -115.0f, 0.0f);
	floor[1]->Scale(9);
	stageMesh = myEngine->LoadMesh("nearwater3.x");
	floor[0] = stageMesh->CreateModel(0.0f, -100.0f, 0.0f);

	stageMesh = myEngine->LoadMesh("skybox.x");
	skybox = stageMesh->CreateModel(0, -110, 0);
	skybox->Scale(75);

	bool cameraRotate = false;                // false           | Used to allow or disallow camera rotation with the keys
	int fps;                                  // Stores the frames per second

	// - Controls -
	const EKeyCode Up = Key_W;                // Key W           | Movement key
	const EKeyCode Down = Key_S;              // Key S           | Movement key
	const EKeyCode Left = Key_A;              // Key A           | Movement key
	const EKeyCode Right = Key_D;             // Key D           | Movement key

	const EKeyCode BiasUp = Key_R;            // Key R           | Terrain set key
	const EKeyCode BiasDown = Key_F;          // Key F           | Terrain set key
	const EKeyCode UI = Key_Return;           // Key Return      | Show the UI
	const EKeyCode SaveMode = Key_Shift;    // Key Shift       | Decide if you want to save the world, or the world and biome layout

	const EKeyCode WarpUp = Key_1;            // Key 1           | Terrain warp key
	const EKeyCode WarpDown = Key_2;          // Key 2           | Terrain warp key

	const EKeyCode SetCamera = Key_0;         // Key 0           | Camera Change

	// - Scrolling -
	float beforeRollPos = 0;                  // 0               | Records where the mouse wheel was before you move it
	float afterRollPos = 0;                   // 0               | Records where the mouse wheel is after you move it
	int   scrollPos = 0;                      // 0               | Current scroll position of the buttons

	// - Movement -
	float acceleration = 200.0f;              // 200.0f          | Adds and takes away from a given cubes current speed
	float speedLimit = 100.0f;                // 100.0f          | Value where the cube stops accelerating
	float swaySpeed = 0;                      // 0.0f            | This value changes to make the lower water sway side to side
	float swayTimer = 0;

	// - Map -
	int   mapLimit = 1280;                    // 1280            | Max size of the map
	int   sightLimit = 32;                    // 32              | Max size of the spawned map (32 playable, 256 for visuals)
	int   playerLimit = mapLimit - sightLimit;// Max size for player movement

	// - Extras -
	float halfCubeSize = 10.0f;               // 10.0f           | Cube size, used when when I need to multiply a co-ordinate by the cubes size
	float chunkSize = halfCubeSize * 10.0f;   // x10             | How often the map updates, used to make it look less stuttery
	float fTime = 0.0f;                       // 0.0f            | Frame time, measuring time between each frame produced
	int   octaveCount = 9;
	bool  textureMode = false;                // false           | Height map is off, remembers what mode the textures are on
	int   density = 1;                        // X/1             | Decides on the amount of trees and grass spawned
	int   saveMode = 0;                       // 0/1/2           | 0 loads the world, 1 loads the biomes, 2 for both, used for performance reasons

	float subPosition[2] = { 0.0f, 0.0f };    // [Z][X] = [0][1] | Checks if player has moved a blocks distance, used in level spawning

	bool  firstPerson = false;                // false           | Records which camera is in use
	bool  activeUI = true;                    // true            | Decides if the stats appear

	// - Cameras -
	ICamera* myCamera;
	myCamera = myEngine->CreateCamera(kManual);
	myCamera->SetPosition(0.0f, 350.0f, -750.0f);
	myCamera->RotateX(25.0f);

	ICamera* playerCam;
	playerCam = myEngine->CreateCamera(kManual);
	playerCam->SetPosition(playerCube.pos[x], playerCube.pos[y], playerCube.pos[z] + halfCubeSize / 2);
	playerCam->AttachToParent(playerModel);

	// Set the slider
	float SliderHeight = 64.0f;
	float SliderPos[2] = { 1400.0f, 10.0f };
	float PointerPosY = 40.0f;
	float PointerPosX[5] = { 1409.0f, 1477.0f, 1545.0f, 1612.0f, 1680.0f };
	int   activePos = 0;
	string SliderNames[5] = { "Slider_0", "Slider_1", "Slider_2", "Slider_3", "Slider_4" };
	ISprite* Pointer = myEngine->CreateSprite("Pointer.png", PointerPosX[0], PointerPosY);
	ISprite* Slider = myEngine->CreateSprite(SliderNames[activePos] + ".png", SliderPos[0], SliderPos[1]);

	// Set world buttons
	bool spriteActiveA[worldButtons];
	ISprite* button[worldButtons];
	string fileNames[worldButtons] = { "Add octave", "Remove octave", "New seed", "Height map", "Tree density", "Add bias", "Remove bias" }; // Button file names
	float buttonXPos = 1735.0f;
	float buttonYPos = 85.0f;
	for (int i = 0; i < worldButtons; i++)
	{
		button[i] = myEngine->CreateSprite(fileNames[i] + ".png", buttonXPos, ((i + 1) * 110.0f) - buttonYPos);
		idxWorldButtons[i]->FnSetButtonPos(button[i]->GetX(), button[i]->GetY());
		idxWorldButtons[i]->FnSetButtonSize();
		spriteActiveA[i] = true;
	}

	// Set save buttons
	bool spriteActiveS[saveButtons];
	ISprite* sButton[saveButtons];
	string saveFileName[saveButtons] = { "Save 0", "Save 1", "Save 2", "Save 3", "Save 4", "Save 5" }; // Save file names
	float sButtonXPos = 100.0f;
	float sButtonYPos = 956.0f;
	for (int i = 0; i < saveButtons; i++)
	{
		sButton[i] = myEngine->CreateSprite(saveFileName[i] + ".png", ((i + 1) * 120.0f) - sButtonXPos, sButtonYPos);
		idxSaveButtons[i]->FnSetButtonPos(((i + 1) * 120.0f) - sButtonXPos, sButtonYPos);
		idxSaveButtons[i]->FnSetButtonSize();
		spriteActiveS[i] = true;
	}

	// Set load buttons
	bool spriteActiveL[saveButtons];
	ISprite* lButton[saveButtons];
	string loadFileName[saveButtons] = { "Load 0", "Load 1", "Load 2", "Load 3", "Load 4", "Load 5" }; // Load file names
	float lButtonXPos = 100.0f;
	float lButtonYPos = 1006.0f;
	for (int i = 0; i < saveButtons; i++)
	{
		lButton[i] = myEngine->CreateSprite(loadFileName[i] + ".png", ((i + 1) * 120.0f) - lButtonXPos, lButtonYPos);
		idxLoadButtons[i]->FnSetButtonPos(((i + 1) * 120.0f) - lButtonXPos, lButtonYPos);
		idxLoadButtons[i]->FnSetButtonSize();
		spriteActiveL[i] = true;
	}

	for (int i = -mapLimit; i < mapLimit; i++)
	{ // Load up all the blocks
		for (int j = -mapLimit; j < mapLimit; j++)
		{
			cubeN.xBlock = j;
			cubeN.zBlock = i;
			cube.push_back(cubeN);
		}
	}
	CPerlin.setAllValues(mapLimit << 1, mapLimit << 1, octaveCount, 300); // Set the height map
	CPerlin.Update(3, octaveCount, density); // Set the blocks to the height map

	SpawnBlocksOptimised(cubeMesh, cubeModel, cubeModelN, treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10);
	OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, (int)subPosition[0] / 10, (int)subPosition[1] / 10, chunkSize, textureMode);
	TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		if (firstPerson)
			myEngine->DrawScene(playerCam);
		else
			myEngine->DrawScene(myCamera);

		/**** Update your scene each frame here ****/
		fTime = myEngine->Timer();

		// Movement -------
		playerModel->MoveLocalX(playerCube.xSpeed * fTime); //Left / Right
		playerCube.pos[x] = playerModel->GetX();

		playerModel->MoveLocalZ(playerCube.zSpeed * fTime); //Foreward / Backward
		playerCube.pos[z] = playerModel->GetZ();

		if (myEngine->KeyHit(SetCamera))
		{
			firstPerson = !firstPerson;
			myCamera->ResetOrientation();
		}

		if (myEngine->KeyHit(UI))
			activeUI = !activeUI;

		if (myEngine->KeyHit(SaveMode))
		{
			saveMode++;
			if (saveMode > 2)
				saveMode = 0;
		}

		// - Accelerating -
		if (myEngine->KeyHeld(Up) && playerCube.zSpeed <= speedLimit) //Forward
			playerCube.zSpeed += acceleration * fTime;
		if (myEngine->KeyHeld(Down) && playerCube.zSpeed >= -speedLimit) //Backward
			playerCube.zSpeed -= acceleration * fTime;
		if (myEngine->KeyHeld(Right) && playerCube.xSpeed <= speedLimit) //Right
			playerCube.xSpeed += acceleration * fTime;
		if (myEngine->KeyHeld(Left) && playerCube.xSpeed >= -speedLimit) //Left
			playerCube.xSpeed -= acceleration * fTime;

		if (!myEngine->KeyHeld(Up) && !myEngine->KeyHeld(Down))
			Decelerate(acceleration, playerCube.zSpeed, fTime); //As long as you aren't holding 'W' or 'S', it will decelerate you in the correct direction
		if (!myEngine->KeyHeld(Left) && !myEngine->KeyHeld(Right))
			Decelerate(acceleration, playerCube.xSpeed, fTime); //As long as you aren't holding 'A' or 'D', it will delerate you in the right direction

		//---------------------------------------------------------
		// Terrain
		// Level Spawning -------
		// - Up Z-Axis -
		if (playerCube.pos[z] > subPosition[0] + chunkSize)
		{
			subPosition[0] += chunkSize;
			OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, ((int)subPosition[0] - chunkSize) / 10, (int)subPosition[1] / 10, chunkSize, textureMode, 3);
			floor[0]->MoveZ(chunkSize);
			TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
		}
		// - Down Z-Axis -
		else if (playerCube.pos[z] < subPosition[0] - chunkSize)
		{
			subPosition[0] -= chunkSize;
			OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, ((int)subPosition[0] + chunkSize) / 10, (int)subPosition[1] / 10, chunkSize, textureMode, 1);
			floor[0]->MoveZ(-chunkSize);
			TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
		}
		// - Up X-Axis -
		if (playerCube.pos[x] > subPosition[1] + chunkSize)
		{
			subPosition[1] += chunkSize;
			OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, (int)subPosition[0] / 10, ((int)subPosition[1] - chunkSize) / 10, chunkSize, textureMode, 2);
			floor[0]->MoveX(chunkSize);
			TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
		}
		// - Down X-Axis -
		else if (playerCube.pos[x] < subPosition[1] - chunkSize)
		{
			subPosition[1] -= chunkSize;
			OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, (int)subPosition[0] / 10, ((int)subPosition[1] + chunkSize) / 10, chunkSize, textureMode, 0);
			floor[0]->MoveX(-chunkSize);
			TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
		}

		skybox->RotateY(0.5 * fTime);
		float sm[16];
		skybox->GetMatrix(sm);
		sm[15] = 0;
		skybox->SetMatrix(sm);

		swaySpeed = 5.0f * sin(swayTimer);
		floor[0]->MoveX(swaySpeed * fTime);
		swayTimer += fTime;

		//---------------------------------------------------------
		// Testing Tools
		// Buttons -------
		// - Setting values -
		mousePos[x] = myEngine->GetMouseX();
		mousePos[y] = myEngine->GetMouseY();
		for (int i = 0; i < worldButtons; i++) //Swaps the icons for each button depending on if you hover over or not
			FnButtonSwap(myEngine, idxWorldButtons[i], button[i], spriteActiveA[i], fileNames[i], buttonXPos, ((i + 1) * 110.0f) - buttonYPos);
		for (int i = 0; i < saveButtons; i++) //Swaps the icons for each button depending on if you hover over or not
			FnButtonSwap(myEngine, idxSaveButtons[i], sButton[i], spriteActiveS[i], saveFileName[i], ((i + 1) * 120.0f) - sButtonXPos, sButtonYPos);
		for (int i = 0; i < saveButtons; i++) //Swaps the icons for each button depending on if you hover over or not
			FnButtonSwap(myEngine, idxLoadButtons[i], lButton[i], spriteActiveL[i], loadFileName[i], ((i + 1) * 120.0f) - lButtonXPos, lButtonYPos);
        
        if (mousePos[x] >= 1070.0f)
		{ //If I roll it forwards
			// - Setting Scrolling -
			beforeRollPos = myEngine->GetMouseWheel(); //Stores position before rolling mouse wheel, used so I can check which way I rolled it
			if (myEngine->GetMouseWheelMovement()) //When I move the mouse wheel, store the new position to see where it moved
				afterRollPos = myEngine->GetMouseWheel();
			if (afterRollPos < beforeRollPos)
			{
				if (scrollPos > 0)
				{
					buttonYPos -= 10.0f;
					scrollPos--;
					for (int i = 0; i < worldButtons; i++)
					{
						button[i]->MoveY(-buttonYPos);
						myEngine->RemoveSprite(button[i]);
						button[i] = myEngine->CreateSprite(fileNames[i] + ".png", buttonXPos, ((i + 1) * 110.0f) - buttonYPos);
						idxWorldButtons[i]->FnSetButtonPos((int)button[i]->GetX(), (int)button[i]->GetY());
					}
				}
			}
            else if (afterRollPos > beforeRollPos)
            { //Rolling it back
                if (scrollPos < 8)
                {
                    buttonYPos += 10.0f;
                    scrollPos++;
                    for (int i = 0; i < worldButtons; i++)
                    {
                        button[i]->MoveY(-buttonYPos);
                        myEngine->RemoveSprite(button[i]);
                        button[i] = myEngine->CreateSprite(fileNames[i] + ".png", buttonXPos, ((i + 1) * 110.0f) - buttonYPos);
                        idxWorldButtons[i]->FnSetButtonPos((int)button[i]->GetX(), (int)button[i]->GetY());
                    }
                }
            }
		}
		// - Executing buttons -
		if (myEngine->KeyHit(Mouse_LButton))
		{ //If you left click, execute one of these depending on the position of the mouse
			for (int i = 0; i < worldButtons; i++)
			{
				if (idxWorldButtons[i]->FnButtonState())
				{ //Button functions | Add octave, Remove octave, Change seed
					idxWorldButtons[i]->Execute(mapLimit, octaveCount, textureMode, density, i);
					OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, (int)subPosition[0] / 10, (int)subPosition[1] / 10, chunkSize, textureMode);
					TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
				}
			}
			for (int i = 0; i < saveButtons; i++)
			{
				if (idxSaveButtons[i]->FnButtonState())
				{ //Button functions | Add octave, Remove octave, Change seed
					idxSaveButtons[i]->SaveWorld(i, saveMode);
					idxSaveButtons[i]->Execute(mapLimit, octaveCount, textureMode, density, i);
					OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, (int)subPosition[0] / 10, (int)subPosition[1] / 10, chunkSize, textureMode);
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
				}
			}
			for (int i = 0; i < saveButtons; i++)
			{
				if (idxLoadButtons[i]->FnButtonState())
				{ //Button functions | Add octave, Remove octave, Change seed
					idxLoadButtons[i]->SaveWorld(i, saveMode);
					idxLoadButtons[i]->Execute(mapLimit, octaveCount, textureMode, density, i);
					CPerlin.Update(3, octaveCount, density);
					OptimisedTerrain(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, (int)subPosition[0] / 10, (int)subPosition[1] / 10, chunkSize, textureMode);
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
					TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
				}
			}
		}
		else if (myEngine->KeyHeld(Mouse_LButton))
		{
			if (mousePos[y] <= SliderPos[y] + SliderHeight && mousePos[y] >= SliderPos[y])
			{
				if (mousePos[x] > PointerPosX[0] - 15 && mousePos[x] < PointerPosX[4] + 15) // Set the edges of the slider
					Pointer->SetX((float)mousePos[x]);

				if (mousePos[x] <= PointerPosX[0] + 15 && mousePos[x] >= PointerPosX[0] - 15 && activePos != 0)
				{ // Each if statement is an area of slider, this is 0
					sightLimit = 32; // Setting the draw distance value
					PrepareResize(cubeMesh, cubeModel, cubeModelN, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10); // Getting all the arrays and positions set up
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
					TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
					myEngine->RemoveSprite(Slider); // Change the sprite to highlight a different number
					activePos = 0;
					Slider = myEngine->CreateSprite(SliderNames[activePos] + ".png", SliderPos[0], SliderPos[1]);
				}

				else if (mousePos[x] <= PointerPosX[1] + 15 && mousePos[x] >= PointerPosX[1] - 15 && activePos != 1)
				{ // This is 1
					sightLimit = 64;
					PrepareResize(cubeMesh, cubeModel, cubeModelN, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10);
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
					TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
					myEngine->RemoveSprite(Slider);
					activePos = 1;
					Slider = myEngine->CreateSprite(SliderNames[activePos] + ".png", SliderPos[0], SliderPos[1]);
				}

				else if (mousePos[x] <= PointerPosX[2] + 15 && mousePos[x] >= PointerPosX[2] - 15 && activePos != 2)
				{ // This is 2, etc...
					sightLimit = 128;
					PrepareResize(cubeMesh, cubeModel, cubeModelN, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10);
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
					TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
					myEngine->RemoveSprite(Slider);
					activePos = 2;
					Slider = myEngine->CreateSprite(SliderNames[activePos] + ".png", SliderPos[0], SliderPos[1]);
				}

				else if (mousePos[x] <= PointerPosX[3] + 15 && mousePos[x] >= PointerPosX[3] - 15 && activePos != 3)
				{
					sightLimit = 192;
					PrepareResize(cubeMesh, cubeModel, cubeModelN, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10);
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
					TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
					myEngine->RemoveSprite(Slider);
					activePos = 3;
					Slider = myEngine->CreateSprite(SliderNames[activePos] + ".png", SliderPos[0], SliderPos[1]);
				}

				else if (mousePos[x] <= PointerPosX[4] + 15 && mousePos[x] >= PointerPosX[4] - 15 && activePos != 4)
				{
					sightLimit = 256;
					PrepareResize(cubeMesh, cubeModel, cubeModelN, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10);
					TextureSwap(cubeModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, textureMode);
					TreeSpawn(treeMesh, darkTreeMesh, snowTreeMesh, treeModel, darkTreeModel, snowTreeModel, treeModelN, grassMesh, grassModel, mapLimit, sightLimit, (int)subPosition[0] / 10, (int)subPosition[1] / 10, density);
					myEngine->RemoveSprite(Slider);
					activePos = 4;
					Slider = myEngine->CreateSprite(SliderNames[activePos] + ".png", SliderPos[0], SliderPos[1]);
				}
			}
		}

		// Camera settings -------
		// - Swapping state -
		if (myEngine->KeyHeld(Key_Control))
		{ //So you can rotate the camera to get different angles
			if (cameraRotate == false)
				cameraRotate = true;
		}
		else
			cameraRotate = false;
		// - Applying states -
		if (!cameraRotate && !firstPerson)
		{ //If you aren't wanting to rotate and you just want to move, letting go of control will let you move around the space
			if (myEngine->KeyHeld(Key_Up))
				myCamera->MoveLocalZ(150.0f * fTime);
			if (myEngine->KeyHeld(Key_Down))
				myCamera->MoveLocalZ(-150.0f * fTime);
			if (myEngine->KeyHeld(Key_Left))
				myCamera->MoveLocalX(-150.0f * fTime);
			if (myEngine->KeyHeld(Key_Right))
				myCamera->MoveLocalX(150.0f * fTime);
		}
		else
		{ //Hold control again and it goes back to rotate mode
			if (myEngine->KeyHeld(Key_Up))
				myCamera->RotateLocalX(-100.0f * fTime);
			if (myEngine->KeyHeld(Key_Down))
				myCamera->RotateLocalX(100.0f * fTime);
			if (myEngine->KeyHeld(Key_Left))
				myCamera->RotateY(-100.0f * fTime);
			if (myEngine->KeyHeld(Key_Right))
				myCamera->RotateY(100.0f * fTime);
		}

		if (firstPerson)
		{
			if (myEngine->KeyHeld(Key_Up))
				playerModel->RotateLocalX(-100.0f * fTime);
			if (myEngine->KeyHeld(Key_Down))
				playerModel->RotateLocalX(100.0f * fTime);
			if (myEngine->KeyHeld(Key_Left))
				playerModel->RotateY(-100.0f * fTime);
			if (myEngine->KeyHeld(Key_Right))
				playerModel->RotateY(100.0f * fTime);

			skybox->SetZ(playerCam->GetZ());
			skyFloor->SetZ(playerCam->GetZ());
			skybox->SetX(playerCam->GetX());
			skyFloor->SetX(playerCam->GetX());
			skybox->SetZ(playerCam->GetZ());
			floor[1]->SetZ(playerCam->GetZ());
			skybox->SetX(playerCam->GetX());
			floor[1]->SetX(playerCam->GetX());
		}
		else
		{
			skybox->SetZ(myCamera->GetZ());
			skyFloor->SetZ(myCamera->GetZ());
			skybox->SetX(myCamera->GetX());
			skyFloor->SetX(myCamera->GetX());
			skybox->SetZ(myCamera->GetZ());
			floor[1]->SetZ(myCamera->GetZ());
			skybox->SetX(myCamera->GetX());
			floor[1]->SetX(myCamera->GetX());
		}
		// - Resetting camera -
		if (myEngine->KeyHit(Key_Back)) //In case you want your original angle again without resetting the program
			myCamera->ResetOrientation();

		if (myEngine->KeyHit(Key_Escape))
			myEngine->Stop();

		if (activeUI)
		{
			fps = (int)(1.0f / fTime);
			Text->Draw("FPS: ", 10, 10, kBlack, kLeft, kTop);
			Text->Draw(to_string(fps), 80, 10, kBlack, kLeft, kTop);
			Text->Draw("Cubes loaded: ", 10, 40, kBlack, kLeft, kTop);
			Text->Draw(to_string(cubeModel.size()), 10, 70, kBlack, kLeft, kTop);
			Text->Draw("Map size: ", 10, 100, kBlack, kLeft, kTop);
			Text->Draw(to_string(cube.size()), 10, 130, kBlack, kLeft, kTop);
			Text->Draw("Octaves: ", 10, 160, kBlack, kLeft, kTop);
			Text->Draw(to_string(CPerlin.GetOctaves()), 125, 160, kBlack, kLeft, kTop);
			if (saveMode == 0)
				Text->Draw("Loading mode: world", 10, 911, kBlack, kLeft, kTop);
			else if (saveMode == 1)
				Text->Draw("Loading mode: biome", 10, 911, kBlack, kLeft, kTop);
			else if (saveMode == 2)
				Text->Draw("Loading mode: world & biome", 10, 911, kBlack, kLeft, kTop);
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
