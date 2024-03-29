#include "Game.hpp"
#include "RenderWindow.hpp"

#include <iostream>
#include <fstream>

TitleScreen* titlescreen = NULL;
Gameplay* gamescreen = NULL;
EndScreen* endscreen = NULL;
Manual* manualscreen = NULL;
Selection* selectionscreen = NULL;

RenderWindow window;

Game::Game()
{

}

Game::~Game()
{

}

enum State
{
	TitleState = 0,
	IngameState = 1,
	EndState = 2,
	ManualState = 3,
	SelectionState = 4
};

void Game::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0) 
		std::cout << "SDL_Init failed. Error: " << SDL_GetError() << std::endl;
	if (!(IMG_Init(IMG_INIT_PNG)))
		std::cout << "IMG_Init failed. Error: " << SDL_GetError() << std::endl;
	if (Mix_Init(MIX_INIT_MP3) < 0)
		std::cout << "Mix_Init failed. Error: " << SDL_GetError() << std::endl;
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
		std::cout << "Mix_OpenAudio failed. Error: " << SDL_GetError() << std::endl;
	if (TTF_Init())
		std::cout << "TTF_init has failed. Error: " << SDL_GetError() << std::endl;
	clickSfx = Mix_LoadWAV("res/sounds/button-click.wav");
	backSfx = Mix_LoadWAV("res/sounds/title-screen-back.wav");
	window.create("Project Taiko", 1280, 720);

	gameState = TitleState;
	titlescreen = new TitleScreen(window);
	titlescreen->init();
	gamescreen = new Gameplay(window);
	selectionscreen = new Selection(window);
	manualscreen = new Manual(window);

	isRunning = true;
}

void Game::update() {
	switch (gameState)
	{
	case TitleState:
		titlescreen->update();
		break;
	case IngameState:
		gamescreen->update();
		if (gamescreen->getOkay()) gamescreen->init(difficulty, selectionscreen->getMod());
		if (gamescreen->getMiss()) gamescreen->init(difficulty, selectionscreen->getMod());
		if (gamescreen->getEnd() == END_POINT)
		{
			Mix_HaltMusic();
			gameState = EndState;
			
			//getting score into a vector
			std::vector<int> tempScores;
			std::fstream scoreFile("data/score.txt");
			int tempScore;
			while (scoreFile >> tempScore) tempScores.push_back(tempScore);

			//replacing score
			if (gamescreen->getRanking() < tempScores[difficulty]) 
			tempScores[difficulty] = gamescreen->getRanking();

			//saving score
			scoreFile.close();
			std::ofstream outputScoreFile("data/score.txt");
			for (auto i: tempScores) outputScoreFile << i << std::endl;
			outputScoreFile.close();

			endscreen = new EndScreen(window, gamescreen->getTotalScore(), gamescreen->getMaxCombo(), gamescreen->getRanking(), gamescreen->getAcc(), gamescreen->getBindingCount());
		}
		break;
	case EndState:
		endscreen->update();
		break;
	case ManualState:
		manualscreen->update();
		break;
	case SelectionState:
		selectionscreen->update();
	default:
		break;
	}
}

void Game::handleEvents()
{
    SDL_Event e;
    Uint32 escPressedTime = 0;
    Uint32 rPressedTime = 0;

    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
            	switch (gameState)
            	{
            	case TitleState:
            		if (titlescreen->getButton(START)->getHovered())
                    {
                    	Mix_PlayChannel(-1, clickSfx, 0);
                        gameState = SelectionState;
 						selectionscreen->init();
 						
                        return;
                    }
                    if (titlescreen->getButton(MANUAL)->getHovered())
                    {
                    	Mix_PlayChannel(-1, clickSfx, 0);
                        gameState = ManualState;
                        
                        return;
                    }
                    if (titlescreen->getButton(QUIT)->getHovered())
                    {
                    	Mix_PlayChannel(-1, clickSfx, 0);
                        isRunning = false;
                        return;
                    }
                    break;

                case IngameState:
                	break;

                case EndState:
                	if (endscreen->getButton(RESTART)->getHovered())
                	{
                		Mix_PlayChannel(-1, clickSfx, 0);
                		Mix_HaltMusic();
                		gameState = IngameState;
                		gamescreen->init(difficulty, selectionscreen->getMod());
                		delete endscreen;
                		return;
                	}
                	if (endscreen->getButton(TITLE)->getHovered())
                	{
                		Mix_PlayChannel(-1, clickSfx, 0);
                		Mix_HaltMusic();
                		gameState = TitleState;
                		titlescreen->init();
                		delete endscreen;
                		return;
                	}
                	break;

                case ManualState:
                	if (manualscreen->getButton()->getHovered())
                	{
                		Mix_PlayChannel(-1, clickSfx, 0);
                		gameState = TitleState;
                		return;
                	}
                	break;

               	case SelectionState:
               		if (selectionscreen->getButton(EASY)->getHovered())
               		{
               			Mix_PlayChannel(-1, clickSfx, 0);
               			difficulty = EASY_DIF;
               			gameState = IngameState;
               			gamescreen->init(difficulty, selectionscreen->getMod());;
               			return;
               		}
               		if (selectionscreen->getButton(NORMAL)->getHovered())
               		{
               			Mix_PlayChannel(-1, clickSfx, 0);
               			difficulty = NORMAL_DIF;
               			gameState = IngameState;
               			gamescreen->init(difficulty, selectionscreen->getMod());;
               			return;
               		}
               		if (selectionscreen->getButton(HARD)->getHovered())
               		{
               			Mix_PlayChannel(-1, clickSfx, 0);
               			difficulty = HARD_DIF;
               			gameState = IngameState;
               			gamescreen->init(difficulty, selectionscreen->getMod());;
               			return;
               		}
               		if (selectionscreen->getButton(BACK)->getHovered())
               		{
               			Mix_PlayChannel(-1, clickSfx, 0);
               			gameState = TitleState;
                		return;
               		}
               		break;
            	}

            case SDL_KEYDOWN:
                if (gameState == IngameState)
                {
                	gamescreen->handleEvents(e);
                	switch (e.key.keysym.sym) {
                	case SDLK_ESCAPE:
                		if (e.key.repeat == 0)
                        {
                            escPressedTime = SDL_GetTicks();
                            Mix_PlayChannel(-1, backSfx, 0);
                        }
                        else if (SDL_GetTicks() - escPressedTime >= 500)
                        {
                        	Mix_HaltMusic();
                            gameState = TitleState;
   							titlescreen->init();                         
                        }
                        break;
                	case SDLK_r:
                		if (e.key.repeat == 0)
                        {
                            rPressedTime = SDL_GetTicks();
                            Mix_PlayChannel(-1, backSfx, 0);
                        }
                        else if (SDL_GetTicks() - rPressedTime >= 500)
                        {
                            gamescreen->init(difficulty, selectionscreen->getMod());;
                        }
                        break;
                	}
                }
                break;
        }
    }
}

void Game::clean()
{
	window.cleanUp();
	SDL_Quit();
}

void Game::render()
{
	switch(gameState)
	{
	case TitleState:
		titlescreen->render();
		break;
	case IngameState:
		gamescreen->render();
		break;
	case EndState:
		endscreen->render();
		break;
	case ManualState:
		manualscreen->render();
		break;
	case SelectionState:
		selectionscreen->render();
		break;
	default:
		break;
	}
}

bool Game::running() {
	return isRunning;
}