#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine2.h"
#include "FileReading.h"

#include <time.h>
#include <array>
#include <iostream>

class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		sAppName = "Let's Platform Outdoors";
	}

	~Game()
	{
		delete sprPlayer;
		delete decPlayer;
		delete sprGrass;
		delete decGrass;
		delete sprTree;
		delete decTree;
		delete sprBackground;
		delete decBackground;
		delete sprBackground2;
		delete decBackground2;
		delete sprMountains;
		delete decMountains;
		delete sprCloud;
		delete decCloud;
	}

	bool playerOnGround = false;
	bool fellInPit = false;

	std::string level;
	int levelWidth;
	int levelHeight;

	int tileSize = 32;
	olc::vi2d visibleTiles;

	olc::vf2d playerPos = {1.0f, 12.0f};
	olc::vf2d playerVel;

	olc::vf2d camera;
	olc::vf2d offset;

	olc::Sprite* sprPlayer;
	olc::Decal*	 decPlayer;
	olc::Sprite* sprGrass;
	olc::Decal*  decGrass;
	olc::Sprite* sprTree;
	olc::Decal*  decTree;
	olc::Sprite* sprBackground;
	olc::Decal*  decBackground;
	olc::Sprite* sprBackground2;
	olc::Decal*  decBackground2;
	olc::Sprite* sprMountains;
	olc::Decal*  decMountains;
	olc::Sprite* sprCloud;
	olc::Decal*  decCloud;

	int sprPlayerOffsetX = 0;
	int sprPlayerOffsetY = 0;

	std::array<olc::vf2d, 6> cloudPos;
	
	float timeCounter = 0.0f;
	float timeBetweenFrames = 0.05f;

public:
	bool OnUserCreate() override
	{
		srand(time(NULL));
		level = ReadFromFiles("res/Outdoors.lvl", levelWidth, levelHeight);

		sprPlayer = new olc::Sprite("res/PlayerSpriteSheet.png");
		decPlayer = new olc::Decal(sprPlayer);

		sprGrass = new olc::Sprite("res/Grass.png");
		decGrass = new olc::Decal(sprGrass);

		sprTree = new olc::Sprite("res/Tree.png");
		decTree = new olc::Decal(sprTree);

		sprBackground = new olc::Sprite("res/Background.png");
		decBackground = new olc::Decal(sprBackground);

		sprBackground2 = new olc::Sprite("res/Background2.png");
		decBackground2 = new olc::Decal(sprBackground2);

		sprMountains = new olc::Sprite("res/Mountains.png");
		decMountains = new olc::Decal(sprMountains);

		sprCloud = new olc::Sprite("res/Cloud.png");
		decCloud = new olc::Decal(sprCloud);

		for (int i = 0; i < cloudPos.size(); i++)
			cloudPos[i] = { (float)(i * 100), (float)(rand() % 60) };

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		//
		auto GetTile = [&](int x, int y)
		{
			if (x >= 0 && x < levelWidth && y >= 0 && y < levelHeight)
				return level[y * levelWidth + x];
			else
				return ' ';
		};

		auto SetTile = [&](int x, int y, wchar_t c)
		{
			if (x >= 0 && x < levelWidth && y >= 0 && y < levelHeight)
				level[y * levelWidth + x] = c;
		};


		if (GetKey(olc::A).bHeld && !fellInPit) 
		{
			playerVel.x = (playerOnGround ? -12.0f : -9.0f);

			timeCounter += fElapsedTime;

			if (timeCounter >= timeBetweenFrames)
			{
				timeCounter -= timeBetweenFrames;
				if (sprPlayerOffsetX == 12)
					sprPlayerOffsetX = 1;
				else
					sprPlayerOffsetX++;
			}			

			sprPlayerOffsetY = 1;
		}
		if (GetKey(olc::D).bHeld && !fellInPit)
		{
			playerVel.x = (playerOnGround ? 12.0f : 9.0f);

			timeCounter += fElapsedTime;

			if (timeCounter >= timeBetweenFrames)
			{
				timeCounter -= timeBetweenFrames;
				if (sprPlayerOffsetX == 12)
					sprPlayerOffsetX = 1;
				else
					sprPlayerOffsetX++;
			}

			sprPlayerOffsetY = 0;
		}
		if (GetKey(olc::SPACE).bPressed) 
			if (playerVel.y == 0.0f) 
				playerVel.y = -10.0f;

		if ((GetKey(olc::A).bReleased) || (GetKey(olc::D).bReleased))
			sprPlayerOffsetX = 0;				

		// Drag
		if (playerOnGround)
		{
			playerVel.x += -20.0f * playerVel.x * fElapsedTime;
			if (fabs(playerVel.x) < 0.01f)
				playerVel.x = 0.0f;
		}

		playerVel.y += 20.0f * fElapsedTime;

		if (playerVel.x > 10.0f)
			playerVel.x = 10.0f;
		if (playerVel.x < -10.0f)
			playerVel.x = -10.0f;

		if (playerVel.y > 100.0f)
			playerVel.y = 100.0f;
		if (playerVel.y < -100.0f)
			playerVel.y = -100.0f;

		olc::vf2d newPlayerPos = playerPos + playerVel * fElapsedTime;

		playerOnGround = false;
		if (playerVel.x <= 0) // Moving Left
		{ 
			if (GetTile(newPlayerPos.x + 0.0f, playerPos.y + 0.0f) == '_' || GetTile(newPlayerPos.x + 0.0f, playerPos.y + 0.9f) == '_' ||
				GetTile(newPlayerPos.x + 0.0f, playerPos.y + 0.0f) == '#' || GetTile(newPlayerPos.x + 0.0f, playerPos.y + 0.9f) == '#')
			{
				newPlayerPos.x = (int)newPlayerPos.x + 1;
				playerVel.x = 0;
			}
		}
		else // Moving Right
		{
			if (GetTile(newPlayerPos.x + 1.0f, playerPos.y + 0.0f) == '_' || GetTile(newPlayerPos.x + 1.0f, playerPos.y + 0.9f) == '_' ||
				GetTile(newPlayerPos.x + 1.0f, playerPos.y + 0.0f) == '#' || GetTile(newPlayerPos.x + 1.0f, playerPos.y + 0.9f) == '#' ||
				GetTile(playerPos.x + 0.0f, newPlayerPos.y + 1.0f) == '-' || GetTile(playerPos.x + 0.9f, newPlayerPos.y + 1.0f) == '-')

			{
				newPlayerPos.x = (int)newPlayerPos.x;
				playerVel.x = 0;
			}
		}
		
		if (playerVel.y <= 0) // Moving Up
		{
			if (GetTile(playerPos.x + 0.0f, newPlayerPos.y + 0.0f) == '_' || GetTile(playerPos.x + 0.9f, newPlayerPos.y + 0.0f) == '_' ||
				GetTile(playerPos.x + 0.0f, newPlayerPos.y + 0.0f) == '#' || GetTile(playerPos.x + 0.9f, newPlayerPos.y + 0.0f) == '#' ||
				GetTile(playerPos.x + 0.0f, newPlayerPos.y + 1.0f) == '-' || GetTile(playerPos.x + 0.9f, newPlayerPos.y + 1.0f) == '-')
			{
				newPlayerPos.y = (int)newPlayerPos.y + 1;
				playerVel.y = 0;
			}
		}
		else // Moving Down
		{
			if (GetTile(playerPos.x + 0.0f, newPlayerPos.y + 1.0f) == '_' || GetTile(playerPos.x + 0.9f, newPlayerPos.y + 1.0f) == '_' ||
				GetTile(playerPos.x + 0.0f, newPlayerPos.y + 1.0f) == '#' || GetTile(playerPos.x + 0.9f, newPlayerPos.y + 1.0f) == '#')
			{
				newPlayerPos.y = (int)newPlayerPos.y;
				playerVel.y = 0;
				playerOnGround = true;
			}
		}

		playerPos = newPlayerPos;

		camera = playerPos;
		
		visibleTiles =
		{
			ScreenWidth() / tileSize,
			ScreenHeight() / tileSize
		};

		offset = camera - (olc::vi2d)visibleTiles / 2.0f;

		if (playerPos.x < 0) playerPos.x = 0;
		if (playerPos.x > levelWidth - 1) playerPos.x = levelWidth - 1;

		if (offset.x < 0) offset.x = 0;
		if (offset.x > levelWidth - visibleTiles.x) offset.x = levelWidth - visibleTiles.x;
		if (offset.y < 0) offset.y = 0;
		if (offset.y > levelHeight - visibleTiles.y) offset.y = levelHeight - visibleTiles.y;

		olc::vf2d tileOffset =
		{
			(offset.x - (int)offset.x) * tileSize,
			(offset.y - (int)offset.y) * tileSize
		};

		if (playerPos.y > levelHeight)
		{
			fellInPit = true;
			playerVel.x = 0.0f;
			if (playerPos.y >= levelHeight + 50)
			{
				fellInPit = false;
				playerPos = { 1.0f, 12.0f };
				playerVel = { 0.0f, 0.0f };
			}
		}

		for (int i = 0; i < cloudPos.size(); i++)
		{
			cloudPos[i].x += -5.0f * fElapsedTime;

			if (cloudPos[i].x < -2 * tileSize)
				cloudPos[i] = { (float)ScreenWidth(), (float)(rand() % 60) };
		}

		Clear(olc::Pixel(0.0f, 200.0f, 255.0f));

		SetPixelMode(olc::Pixel::MASK);

		DrawDecal(olc::vi2d(0, -40) - offset, decMountains);
		DrawDecal(olc::vi2d(512, -40) - offset, decMountains);

		FillRect(olc::vi2d(0, 216 - offset.y), olc::vi2d(512, 110), olc::BLUE);

		for (int i = 0; i < 6; i++)
		{
			DrawPartialDecal(cloudPos[i], decCloud, olc::vi2d(0, 0), olc::vi2d(2 * tileSize, tileSize));				
		}
		
		DrawPartialDecal(olc::vi2d(0, -70) - 2 * offset, decBackground, olc::vi2d(0, 0), olc::vi2d(512, 400));
		DrawPartialDecal(olc::vi2d(510, -70) - 2 * offset, decBackground, olc::vi2d(0, 0), olc::vi2d(512, 400));
		
		for (int x = 0; x < visibleTiles.x + 1; x++)
			for (int y = 0; y < visibleTiles.y + 1; y++)
			{
				char tileSizeID = GetTile(x + offset.x, y + offset.y);
				switch (tileSizeID)
				{
				case '_': DrawPartialDecal(olc::vf2d(x, y) * tileSize - tileOffset, decGrass, olc::vi2d(0, 0), olc::vf2d(tileSize, tileSize));
					break;
				case '#': DrawPartialDecal(olc::vf2d(x, y) * tileSize - tileOffset, decGrass, olc::vi2d(0, tileSize), olc::vf2d(tileSize, tileSize));
					break;
				case 'O': DrawPartialDecal(olc::vf2d(x, y + 0.1f) * tileSize - tileOffset, decTree, olc::vi2d(0, 0), olc::vf2d(tileSize, tileSize));
					break;
				case 'L': DrawPartialDecal(olc::vf2d(x, y + 0.1f) * tileSize - tileOffset, decTree, olc::vi2d(0, tileSize), olc::vf2d(tileSize, tileSize));
					break;
				}
			}

		DrawPartialDecal((playerPos - offset) * tileSize, decPlayer, olc::vi2d(sprPlayerOffsetX * tileSize, sprPlayerOffsetY * tileSize), olc::vi2d(tileSize, tileSize));
				
		SetPixelMode(olc::Pixel::NORMAL);

		return true;
	}
};


int main()
{

#ifdef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	Game game;
	if (game.Construct(512, 300, 2, 2))
		game.Start();
	return 0;
}