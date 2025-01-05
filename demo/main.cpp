#define SDL_MAIN_USE_CALLBACKS 1 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>


namespace {
	struct AppState {
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		TTF_Font* font = nullptr;
		SDL_Texture* textTexture = nullptr;
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

	SDL_Color textColor = {255, 255, 255};
	SDL_Surface* const textSurface = TTF_RenderText_Solid(state->font, "Hello, world!", 0, textColor);
	if(textSurface == nullptr) {
		SDL_Log("Couldn't render text surface! %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	state->textTexture = SDL_CreateTextureFromSurface(state->renderer, textSurface);
	SDL_DestroySurface(textSurface);
	if(state->textTexture == nullptr) {
		SDL_Log("Failed creating texture from rendered text! %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if(event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void* appstate)
{
	AppState* const state = static_cast<::AppState*>(appstate);

	const double now = ((double)SDL_GetTicks()) / 1000.0;

	const float red = (float)(0.5 + 0.5 * SDL_sin(now));
	const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
	const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
	SDL_SetRenderDrawColorFloat(state->renderer, red, green, blue, 1.0f);

	SDL_RenderClear(state->renderer);

	SDL_FRect dest = {100.0f, 100.0f, 0, 0};
	SDL_GetTextureSize(state->textTexture, &dest.w, &dest.h);
	SDL_RenderTexture(state->renderer, state->textTexture, nullptr, &dest);

	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	AppState* const state = static_cast<::AppState*>(appstate);
	
	SDL_DestroyTexture(state->textTexture);
	TTF_CloseFont(state->font);
	TTF_Quit();
	SDL_DestroyRenderer(state->renderer);
	SDL_DestroyWindow(state->window);
	SDL_Quit();
	delete state;
}
