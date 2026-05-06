// @brief  나전칠기 프로그래밍 기초 연습장
#include "MyFirstWndGame.h"
#include <iostream>


int main()
{
	MyFirstWndGame game;
	if (false == game.Initialize())
	{
		std::cout << "Failed to initialize the game. Error: " << GetLastError() << std::endl;
		return -1;
	}

	game.Run();

	game.Finalize();

	return 0;

}