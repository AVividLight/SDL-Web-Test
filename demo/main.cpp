#define SDL_MAIN_USE_CALLBACKS 1 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>


namespace {
	struct AppState {
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		TTF_Font* font = nullptr;
		SDL_Texture* textTextureNormal = nullptr;
		SDL_Texture* textTextureHovered = nullptr;
		SDL_Texture** activeTextTexture = nullptr;


		/*struct Buttons {

		} Buttons;*/
	};
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	::AppState* state = new ::AppState;
	*appstate = state;

	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	if(!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if(!SDL_CreateWindowAndRenderer("examples/renderer/clear", 640, 480, 0, &state->window, &state->renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if(!TTF_Init()) {
		SDL_Log("Couldn't initialize SDL_TTF: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	state->font = TTF_OpenFont("./IBMPlexMono-Regular.ttf", 24);
	if(state->font == nullptr) {
		SDL_Log("Couldn't open font: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	{
		SDL_Color textColor = {255, 255, 255};
		SDL_Surface* const textSurface = TTF_RenderText_Solid(state->font, "Hello, world!", 0, textColor);
		if(textSurface == nullptr) {
			SDL_Log("Couldn't render text surface! %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}

		state->textTextureNormal = SDL_CreateTextureFromSurface(state->renderer, textSurface);
		SDL_DestroySurface(textSurface);
		if(state->textTextureNormal == nullptr) {
			SDL_Log("Failed creating texture from rendered text! %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}

		state->activeTextTexture = &state->textTextureNormal;
	}

	{
		SDL_Color textColor = {255, 0, 0};
		SDL_Surface* const textSurface = TTF_RenderText_Solid(state->font, "Hey there!", 0, textColor);
		if(textSurface == nullptr) {
			SDL_Log("Couldn't render text surface! %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}

		state->textTextureHovered = SDL_CreateTextureFromSurface(state->renderer, textSurface);
		SDL_DestroySurface(textSurface);
		if(state->textTextureHovered == nullptr) {
			SDL_Log("Failed creating texture from rendered text! %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}
	}

	return SDL_APP_CONTINUE;
}


void HandleMouseMoved(::AppState* const appstate, const SDL_MouseMotionEvent event) {
	//if mouse is over text region, change activeTextTexture to textTextureHovered otherwise otherwise set to textTextureNormal
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	switch(event->type) {
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	case SDL_EVENT_MOUSE_MOTION:
		HandleMouseMoved(static_cast<::AppState*>(appstate), event->motion);

	default:
		return SDL_APP_CONTINUE;
	}
}


SDL_AppResult SDL_AppIterate(void* appstate)
{
	::AppState* const state = static_cast<::AppState*>(appstate);

	SDL_SetRenderDrawColorFloat(state->renderer, 0.33f, 0.33f, 0.33f, 1.0f);

	SDL_RenderClear(state->renderer);

	SDL_FRect dest = {100.0f, 100.0f, 0, 0};
	SDL_GetTextureSize(*state->activeTextTexture, &dest.w, &dest.h);
	SDL_RenderTexture(state->renderer, *state->activeTextTexture, nullptr, &dest);

	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	AppState* const state = static_cast<::AppState*>(appstate);
	
	SDL_DestroyTexture(state->textTextureNormal);
	TTF_CloseFont(state->font);
	TTF_Quit();
	SDL_DestroyRenderer(state->renderer);
	SDL_DestroyWindow(state->window);
	SDL_Quit();
	delete state;
}
