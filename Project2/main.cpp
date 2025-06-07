#include <SDL.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>

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
#define PADDLE2_STARTPOS_X 760
#define PADDLE2_STARTPOS_Y 400
#define PADDLE_MOVE_SPEED 400
#define BALL_MOVE_SPEED 300

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
	float velocity_x;
	float velocity_y;
	//float speed;
	//float started;
};
struct ball ball1;

struct velocity_components {
	float velocity_x;
	float velocity_y;
};
int ball_move_init = 0;
const Uint8* keystate = SDL_GetKeyboardState(NULL);
// to make the AI react after a delay instead of instantly to the balls position
float reaction_timer = 0;


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
int check_wall_collision(paddle* paddle) {
	//assuming that the y position is the center, we just need to add half the size to get top/bottom
	if (paddle->y <= 0) {
		return -1;
	}
	else if ((paddle->y + paddle->height) >= WINDOW_HEIGHT) {
		return 1;
	}
	else {
		return 0;
	}
}
void initialise_ball_direction(ball* ball) {
	// random angle
	srand(time(NULL));
	float random_angle = (rand() % 360) * (M_PI / 180.0f);
	//find x and y component
	ball->velocity_x = BALL_MOVE_SPEED * cos(random_angle);
	ball->velocity_y = BALL_MOVE_SPEED * sin(random_angle);
}
int check_paddle_ball_collision(ball* ball) {
	//check against left paddle first
	if (ball->x <= paddle1.x + PADDLE_WIDTH && ball->x + ball->width >= paddle1.x && ball->y <= paddle1.y + PADDLE_HEIGHT && ball->y + ball->height >= paddle1.y) {
		return 1;
	}
	else if (paddle2.x <= ball->x + ball->width && paddle2.x + PADDLE_WIDTH >= ball->x && paddle2.y <= ball->y + ball->height && paddle2.y + PADDLE_HEIGHT >= ball->y) {
		return 1;
	}
	else return 0;
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
	else if (keystate[SDL_SCANCODE_S] == 1) {
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

	ball1.x = WINDOW_WIDTH / 2;
	ball1.y = WINDOW_HEIGHT / 2;
	ball1.width = 20;
	ball1.height = 20;
	ball1.velocity_x = 0;
	ball1.velocity_y = 0;

	//TO DO: setup middle-divider for pong game
}
void update(ball* ball) {
	// get delta time factor converted to seconds to be used to update objects
	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

	last_frame_time = SDL_GetTicks();

	// move paddle in direction of move_direction depending on user input we checked in process_input
	if ((check_wall_collision(&paddle1) * -1 == (int)paddle1.move_direction) || ((int)check_wall_collision(&paddle1) == 0)) {
		paddle1.y += PADDLE_MOVE_SPEED * delta_time * paddle1.move_direction;
	}
	// move ball depending on its x/y component of velocity
	// if ball_position above/below screen, change direction before moving
	if (ball->y <= 0) {
		// offset added for cases when ball moves too fast and ends up being out of the screen
		// for multiple frames, which leads to issues so have added offset to avoid that
		ball->y = 1;
		ball->velocity_y = -ball->velocity_y;
	}
	else if (ball->y + ball->height >= WINDOW_HEIGHT){
		ball->y = WINDOW_HEIGHT - ball->height - 1;
		ball->velocity_y = -ball->velocity_y;
	}
	if (check_paddle_ball_collision(&ball1)) {
		ball->velocity_x = -ball->velocity_x;
	}
	// if ball exits screen reset it's position at centre and randomise direction
	if (ball->x + ball->width <= 0 || ball->x >= WINDOW_WIDTH) {
		ball->x = WINDOW_WIDTH / 2;
		ball->y = WINDOW_HEIGHT / 2;
		ball->velocity_x = 0;
		ball->velocity_y = 0;
		initialise_ball_direction(&ball1);
	}
	ball->x += ball->velocity_x * delta_time;
	ball->y += ball->velocity_y * delta_time;
	// have paddle 2 move towards the ball after a slight delay
	reaction_timer += delta_time;
	if (reaction_timer >= 0.1f) {
		reaction_timer = 0;
		if (ball->y >= paddle2.y + PADDLE_HEIGHT / 2 && paddle2.y + PADDLE_HEIGHT <= WINDOW_HEIGHT) {
			paddle2.y += PADDLE_MOVE_SPEED * delta_time;
		}
		else if (ball->y <= paddle2.y + PADDLE_HEIGHT / 2 && paddle2.y >= 0) {
			paddle2.y -= PADDLE_MOVE_SPEED * delta_time;
		}
	}
	

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
	SDL_Rect paddle2_rect = {
		(int)paddle2.x ,
		(int)paddle2.y,
		(int)paddle2.width,
		(int)paddle2.height
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
	SDL_RenderFillRect(renderer, &paddle2_rect);

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
	initialise_ball_direction(&ball1);

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
			update(&ball1);
			render();
			break;
		}
		
	}

	destroy_window();

	return 0;

}

