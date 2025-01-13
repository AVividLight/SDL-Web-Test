#define SDL_MAIN_USE_CALLBACKS 1 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>


namespace {
	constexpr unsigned char QUANTITY = 8;

	constexpr unsigned int FLAG_NONE = 0;
	constexpr unsigned int FLAG_VISIBLE =	0b00000000'00000000'00000000'00000001;
	constexpr unsigned int FLAG_HOVERED =	0b00000000'00000000'00000000'00000010;
	constexpr unsigned int FLAG_PRESSED =	0b00000000'00000000'00000000'00000100;
	constexpr unsigned int FLAG_DISABLED =	0b00000000'00000000'00000000'00001000;

	struct AppState {
		struct UIElements {
			UIElements()
				: X{0}
				, Y{0}
				, W{0}
				, H{0}
				, Texture{nullptr}
				, Flags{FLAG_NONE}
				, Index{0}
			{

			}


			[[nodiscard]] int AddText(const float posX, const float posY, SDL_Texture* const texture) {
				X[Index] = posX;
				Y[Index] = posY;
				W[Index] = texture->w;
				H[Index] = texture->h;
				Texture[Index] = texture;
				Flags[Index] |= FLAG_VISIBLE;
				return Index++;
			}


			bool PointInElement(const float x, const float y, const int index) const  {
				return x >= X[index] && y >= Y[index] && x < X[index] + W[index] && y < Y[index] + H[index];
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
		} UIElements;

		SDL_Color BackgroundColor = {33, 33, 33, 255};

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		TTF_Font* font = nullptr;

		int Text1ID = -1;
		int Text2ID = -1;
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

	state->font = TTF_OpenFont("./IBMPlexMono-Regular.ttf", 36);
	if(state->font == nullptr) {
		SDL_Log("Couldn't open font: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	{
		SDL_Color textColor = {255, 255, 255};
		SDL_Surface* const textSurface = TTF_RenderText_LCD(state->font, "Hello, world!", 0, textColor, state->BackgroundColor);
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

		state->Text1ID = state->UIElements.AddText(123.0f, 321.0f, texture);
	}

	{
		SDL_Color textColor = {255, 0, 0};
		SDL_Surface* const textSurface = TTF_RenderText_LCD(state->font, "Hey, woah!", 0, textColor, state->BackgroundColor);
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

		state->Text2ID = state->UIElements.AddText(200.0f, 200.0f, texture);
		state->UIElements.Flags[state->Text2ID] &= ~FLAG_VISIBLE;
	}

	SDL_SetRenderDrawColorFloat(state->renderer, state->BackgroundColor.r / 255.0f, state->BackgroundColor.g / 255.0f, state->BackgroundColor.b / 255.0f, 1.0f);

	return SDL_APP_CONTINUE;
}


void HandleMouseMoved(::AppState* const state, const SDL_MouseMotionEvent event) {
	if(state->UIElements.PointInElement(event.x, event.y, state->Text1ID)) {
		if(!(state->UIElements.Flags[state->Text1ID] & FLAG_HOVERED)) {
			state->UIElements.Flags[state->Text1ID] |= FLAG_HOVERED;

			state->UIElements.Flags[state->Text1ID] &= ~FLAG_VISIBLE;
			state->UIElements.Flags[state->Text2ID] |= FLAG_VISIBLE;
		}
	} else {
		if(state->UIElements.Flags[state->Text1ID] & FLAG_HOVERED) {
			state->UIElements.Flags[state->Text1ID] &= ~FLAG_HOVERED;

			state->UIElements.Flags[state->Text1ID] |= FLAG_VISIBLE;
			state->UIElements.Flags[state->Text2ID] &= ~FLAG_VISIBLE;
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

	for(unsigned int index = 0; index < state->UIElements.Index; index += 1) {
		if(state->UIElements.Flags[index] & FLAG_VISIBLE) {
			SDL_FRect textRenderPosition = state->UIElements.RectAtIndex(index);
			SDL_RenderTexture(state->renderer, state->UIElements.Texture[index], NULL, &textRenderPosition);
		}
	}
	SDL_RenderPresent(state->renderer);

	return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	AppState* const state = static_cast<::AppState*>(appstate);
	
	while(state->UIElements.Index > 0) {
		SDL_DestroyTexture(state->UIElements.Texture[state->UIElements.Index--]);
	}

	TTF_CloseFont(state->font);
	TTF_Quit();
	SDL_DestroyRenderer(state->renderer);
	SDL_DestroyWindow(state->window);
	SDL_Quit();
	delete state;
}
