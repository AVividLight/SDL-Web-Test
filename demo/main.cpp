#define SDL_MAIN_USE_CALLBACKS 1 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>


namespace {
	constexpr unsigned char QUANTITY = 8;
	constexpr unsigned int FLAG_HOVERED =	0b00000000'00000000'00000000'00000001;
	constexpr unsigned int FLAG_PRESSED =	0b00000000'00000000'00000000'00000010;
	constexpr unsigned int FLAG_DISABLED =	0b00000000'00000000'00000000'00000100;

	struct AppState {
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		TTF_Font* font = nullptr;

		struct Text {
			Text() : Index{0} {}


			void AddText(const float posX, const float posY, const float sizeW, const float sizeH, SDL_Texture* const texture) {
				X[Index] = posX;
				Y[Index] = posY;
				W[Index] = sizeW;
				H[Index] = sizeH;
				Texture[Index] = texture;
				Index += 1;
			}


			SDL_FRect RectAtIndex(const unsigned char index) const {
				return {X[index], Y[index], W[index], H[index]};
			}


			float X[QUANTITY];
			float Y[QUANTITY];
			float W[QUANTITY];
			float H[QUANTITY];
			SDL_Texture* Texture[QUANTITY];
			unsigned int Flags[QUANTITY];
			unsigned char Index;
		} Text;

		unsigned char ActiveTextIndex = 0;
	};
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
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

		SDL_Texture* const texture = SDL_CreateTextureFromSurface(state->renderer, textSurface);
		SDL_DestroySurface(textSurface);
		if(texture == nullptr) {
			SDL_Log("Failed creating texture from rendered text! %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}

		SDL_FRect dest = {100.0f, 100.0f, 0, 0};
		SDL_GetTextureSize(texture, &dest.w, &dest.h);
		state->Text.AddText(dest.x, dest.y, dest.w, dest.h, texture);
	}

	{
		SDL_Color textColor = {255, 0, 0};
		SDL_Surface* const textSurface = TTF_RenderText_Solid(state->font, "Hey, woah!", 0, textColor);
		if(textSurface == nullptr) {
			SDL_Log("Couldn't render text surface! %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}

		SDL_Texture* const texture = SDL_CreateTextureFromSurface(state->renderer, textSurface);		
		SDL_DestroySurface(textSurface);
		if(texture == nullptr) {
			SDL_Log("Failed creating texture from rendered text! %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}

		SDL_FRect dest = {100.0f, 100.0f, 0, 0};
		SDL_GetTextureSize(texture, &dest.w, &dest.h);
		state->Text.AddText(dest.x, dest.y, dest.w, dest.h, texture);
	}

	SDL_SetRenderDrawColorFloat(state->renderer, 0.20f, 0.20f, 0.20f, 1.0f);

	return SDL_APP_CONTINUE;
}


void HandleMouseMoved(::AppState* const state, const SDL_MouseMotionEvent event) {
	if(event.x >= state->Text.X[0] && event.y >= state->Text.Y[0] && event.x < state->Text.X[0] + state->Text.W[0] && event.y < state->Text.Y[0] + state->Text.H[0]) {
		if(!(state->Text.Flags[0] & FLAG_HOVERED)) {
			state->Text.Flags[0] |= FLAG_HOVERED;
			state->ActiveTextIndex = 1;
		}
	} else {
		if(state->Text.Flags[0] & FLAG_HOVERED) {
			state->Text.Flags[0] &= ~FLAG_HOVERED;
			state->ActiveTextIndex = 0;
		}
	}
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	switch(event->type) {
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	case SDL_EVENT_MOUSE_MOTION:
		HandleMouseMoved(static_cast<::AppState*>(appstate), event->motion);

	default:
		return SDL_APP_CONTINUE;
	}
}


SDL_AppResult SDL_AppIterate(void* appstate) {
	::AppState* const state = static_cast<::AppState*>(appstate);

	SDL_RenderClear(state->renderer);

	SDL_FRect textRenderPosition = state->Text.RectAtIndex(state->ActiveTextIndex);
	SDL_RenderTexture(state->renderer, state->Text.Texture[state->ActiveTextIndex], NULL, &textRenderPosition);

	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	AppState* const state = static_cast<::AppState*>(appstate);
	
	while(state->Text.Index > 0) {
		SDL_DestroyTexture(state->Text.Texture[state->Text.Index--]);
	}

	TTF_CloseFont(state->font);
	TTF_Quit();
	SDL_DestroyRenderer(state->renderer);
	SDL_DestroyWindow(state->window);
	SDL_Quit();
	delete state;
}
