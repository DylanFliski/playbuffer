#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <iostream>

int DISPLAY_WIDTH = 330 * 2;
int DISPLAY_HEIGHT = 330 * 2;
int DISPLAY_SCALE = 2;
int score = 0;


Point2f GetPointOnScreen()
{
	Point2f i;

	i.x = (std::rand() % DISPLAY_WIDTH);
	i.y = (std::rand() % DISPLAY_HEIGHT);

	return i;
}

// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::CreateGameObject(1, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, 1, "Face");
	Play::CreateGameObject(2, GetPointOnScreen(), 1, "Apple");
	Play::CentreAllSpriteOrigins();
}


void UpdateObject()
{
	GameObject& myObject = Play::GetGameObjectByType(2);
	GameObject& player = Play::GetGameObjectByType(1);

	myObject.radius = 20;
	player.radius = 20;
	if (Play::IsColliding(myObject, player))
	{
		score++;
		myObject.pos = GetPointOnScreen();
	}

	Play::UpdateGameObject(myObject);
	Play::DrawObject(myObject);
}

bool Down = false;
bool Up = true;
bool Right = false;
bool Left = false;

float clip(float n, float lower, float higher) {
	return std::max(lower, std::min(n, higher));
}
void UpdatePlayer()
{

	GameObject& player = Play::GetGameObjectByType(1);
	Vector2D newVelocity = player.velocity;
	

	if (Play::KeyDown(VK_UP))
	{
		newVelocity.x = 0;
		newVelocity.y = 3;
		
	}

	else if (Play::KeyDown(VK_DOWN))
	{
		newVelocity.x = 0;
		newVelocity.y = 3;

	}

	if (Play::KeyDown(VK_RIGHT))
	{
		newVelocity.y = 0;
		newVelocity.x = 3;
	}

	else if (Play::KeyDown(VK_LEFT))
	{
		newVelocity.y = 0;
		newVelocity.x = -3;	
		
	}
	

	player.velocity = newVelocity;


	Play::UpdateGameObject(player);
	player.pos.x = clip(player.pos.x, 15, DISPLAY_WIDTH - 15);
	player.pos.y = clip(player.pos.y, 15, DISPLAY_HEIGHT - 15);
	Play::DrawObject(player);
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{

	Play::ClearDrawingBuffer(Play::cGrey);

	UpdatePlayer();
	UpdateObject();

	Play::DrawDebugText({ DISPLAY_WIDTH / 20, DISPLAY_HEIGHT / 20 }, std::to_string(score).c_str());
	Play::PresentDrawingBuffer();
	return Play::KeyDown(VK_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}


