#include <SDL.h>
#include <stdio.h>
#include <cstdlib>

#define FALSE 0
#define TRUE 1
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS)
#define PADDLE_HEIGHT 100
#define PADDLE_WIDTH 20
#define PADDLE1_STARTPOS_X 20
#define PADDLE1_STARTPOS_Y 100
#define PADDLE2_STARTPOS_X (WINDOW_WIDTH - PADDLE1_STARTPOS_X)
#define PADDLE2_STARTPOS_Y (WINDOW_HEIGHT - PADDLE1_STARTPOS_Y)
#define PADDLE_MOVE_SPEED 300
#define BALL_MOVE_SPEED 400

int game_is_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int last_frame_time = 0;

struct paddle {
	float x;
	float y;
	float width;
	float height;
	float move_direction;
};
struct paddle paddle1;
struct paddle paddle2;

struct ball {
	float x;
	float y;
	float width;
	float height;
	float angle;
	float speed;
	float started;
};
struct ball ball1;

const Uint8* keystate = SDL_GetKeyboardState(NULL);


int initialise_window(void){
	//returns 0 if works, so if not there was a error
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initialising SDL. \n");
		return FALSE;
	}
	window = SDL_CreateWindow(
		"silly billy", /*title of the window*/
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL
		 /*can add multiple SDL flags here to do more fancy stuff, like full screen for example (flags)*/
		);
	if (!window) {
		fprintf(stderr, "Error creating SDL window. \n");
		return FALSE;
	}
	renderer = SDL_CreateRenderer(
			window, /*what is the window we are attaching to? the one we just made*/
			-1,		/*driver, pass -1 to SDL to use default driver*/
			0 /*specify flags here*/
		);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL Renderer. \n");
		return FALSE;
	}

	return TRUE;
}

//check for collision with ceiling or floor (edge of windows)
int check_wall_collision(void) {
	//assuming that the y position is the center, we just need to add half the size to get top/bottom
	if (paddle1.y <= 0) {
		return -1;
	}
	else if ((paddle1.y + paddle1.height) >= WINDOW_HEIGHT) {
		return 1;
	}
	else {
		return 0;
	}
}
float set_ball_randommove(void) {
	// random angle
	float random_angle = (rand() % 360) * (M_PI / 180.0f); 
	//find x and y component
	float v_x = BALL_MOVE_SPEED * cos(random_angle);
	float v_y = BALL_MOVE_SPEED * sin(random_angle);
	return 0;
}

// in charge of changing game_is_running to false if needed
void process_input() {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		// whenever you click the "x" button on window
	case SDL_QUIT:
		game_is_running = FALSE;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
			game_is_running = FALSE;
		break;
	}

	// also want to check the state of the keyboard to see if movement keys pressed 
	if (keystate[SDL_SCANCODE_W] == 1) {
		paddle1.move_direction = -1;
	}
	else if(keystate[SDL_SCANCODE_S] == 1){
		paddle1.move_direction = 1;
	}
	else {
		paddle1.move_direction = 0;
	}
}
void setup() {
	// setup two paddles on eitherside of screen
	paddle1.x = PADDLE1_STARTPOS_X;
	paddle1.y = PADDLE1_STARTPOS_Y;
	paddle1.width = PADDLE_WIDTH;
	paddle1.height = PADDLE_HEIGHT;
	paddle1.move_direction = 0;

	paddle2.x = PADDLE2_STARTPOS_X;
	paddle2.y = PADDLE2_STARTPOS_Y;
	paddle2.width = PADDLE_WIDTH;
	paddle2.height = PADDLE_HEIGHT;
	paddle2.move_direction = 0;

	ball1.x = WINDOW_WIDTH/2;
	ball1.y = WINDOW_HEIGHT/2;
	ball1.width = 20;
	ball1.height = 20;
	ball1.angle = 0;
	ball1.speed = BALL_MOVE_SPEED;
	ball1.started = 0;
	//TO DO: setup middle-divider for pong game
}

void update() {
	// logic to keep fixed time step
	// sleep until we reach frame target time
	/*int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}*/
	
// get delta time factor converted to seconds to be used to update objects
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

	last_frame_time = SDL_GetTicks();

	// move paddle in direction of move_direction depending on user input we checked in process_input
	if ((check_wall_collision() * -1 == (int)paddle1.move_direction) || ((int)check_wall_collision() == 0)) {
		paddle1.y += PADDLE_MOVE_SPEED * delta_time * paddle1.move_direction;
	}
	// if ball has yet to move, start moving him in random direction
	//if ()
	
}
void render() {
	// set color you want (activate it)
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);
	// draw stuff
	SDL_Rect paddle1_rect = {
		(int)paddle1.x ,
		(int)paddle1.y,
		(int)paddle1.width,
		(int)paddle1.height
	};
	SDL_Rect ball1_rect = {
		(int)ball1.x ,
		(int)ball1.y,
		(int)ball1.width,
		(int)ball1.height
	};
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &paddle1_rect);
	SDL_RenderFillRect(renderer, &ball1_rect);

	SDL_RenderPresent(renderer);
}
void destroy_window() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char*argv[]) {
	game_is_running = initialise_window();
	setup();

	enum GameState {
		main_menu = 1,
		game = 2
	};
	GameState current_state = game;


	while (game_is_running) {
		switch (current_state) {
		case main_menu:
			break;
		case game:
			process_input();
			update();
			render();
			break;
		}
		
	}

	destroy_window();

	return 0;

}



//SDL_Window* window = nullptr;
//SDL_Renderer* renderer = nullptr;
//
//SDL_Init(SDL_INIT_VIDEO);
//SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
//
///*set render color, sets the bg color, then clear since we don't want to draw with it but just use it for bg*/
//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//SDL_RenderClear(renderer);
//
///*set to white*/
//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
///*draw point to center*/
//SDL_RenderDrawPoint(renderer, 640 / 2, 480 / 2);
//
///*PRESENT the renderer*/
//SDL_RenderPresent(renderer);
//SDL_Delay(5000);
//SDL_DestroyWindow(window);

