#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "render2d.hpp"

bool Renderer::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << "\n";
        return false;
    }
	
	windowWidth = width;
	windowHeight = height;
	
    window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN
	);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << "\n";
        return false;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        return false;
    }

    playerTexture = loadTexture("Assets/Player.png");
    if (!playerTexture) {
        std::cerr << "Failed to load Player.png\n";
        return false;
    }
	
	slotTexture = loadTexture("Assets/Inventory_Slot.png");
	if (!slotTexture) {
		std::cerr << "Failed to load Inventory_Slot.png\n";
		return false;
	}
	
    return true;
}
SDL_Texture* Renderer::loadTexture(const std::string& path) {
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex) {
        std::cerr << "Failed to load texture: " << path
                  << " | Error: " << IMG_GetError() << "\n";
    }
    return tex;
}
SDL_Texture* Renderer::getNPCTexture(int id) {
    if (npcTextures.count(id)) { return npcTextures[id]; }

    std::string path = "Assets/NPCs/NPC-" + std::to_string(id) + ".png";
    SDL_Texture* tex = loadTexture(path);
    npcTextures[id] = tex;
    return tex;
}
SDL_Texture* Renderer::getItemTexture(int id) {
    if (itemTextures.count(id)) { return itemTextures[id]; }

    std::string path = "Assets/Items/I-" + std::to_string(id) + ".png";
    SDL_Texture* tex = loadTexture(path);
    itemTextures[id] = tex;
    return tex;
}
void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}
void Renderer::drawPlayer(int x, int y) {
    SDL_Rect dst{ x, y, 100, 100 };
    SDL_RenderCopy(renderer, playerTexture, nullptr, &dst);
}
void Renderer::drawNPC(int x, int y, int id) {
    SDL_Texture* tex = getNPCTexture(id);
    if (!tex) return;

    SDL_Rect dst{ x, y, 32, 32 };
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
}
void Renderer::drawItem(int x, int y, int id, int slotSize) {
    SDL_Texture* tex = getItemTexture(id);
    if (!tex) return;
	
	int iconSize = slotSize * 0.7;
	
	x += (slotSize - iconSize) / 2;
	y += (slotSize - iconSize) / 2;
	
    SDL_Rect dst{ x, y, iconSize, iconSize };
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
}
void Renderer::present() {
    SDL_RenderPresent(renderer);
}
void Renderer::shutdown() {
    SDL_DestroyTexture(playerTexture);

    for (auto& pair : npcTextures) { SDL_DestroyTexture(pair.second); }
	for (auto& pair : itemTextures) { SDL_DestroyTexture(pair.second); }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
void Renderer::drawInventory() {
	if (!slotTexture) return;
	
	float uiScale = 0.5f;
	
	int base_slotSize = 92;
	int slotSize = static_cast<int>(base_slotSize * uiScale);
	
	int cols = 10;
	int rows = 3;
	
	int totalWidth = (cols + 1) * slotSize;
	int totalHeight = (rows + 1) * slotSize;
	
	int startX = (windowWidth - totalWidth) / 2;
	int startY = (windowHeight - totalHeight) / 2;
	
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			int x = startX + c * slotSize;
			int y = startY + r * slotSize;
			drawSlot(x, y, slotSize);
		}
	}
	
	int weaponX = startX;
	int weaponY = startY + rows * slotSize;
	drawSlot(weaponX, weaponY, slotSize);
	
	int armorSlots = 4;
	int armorStartX = startX - slotSize;
	int armorStartY = startY;
	
	for (int i = 0; i < armorSlots; i++) {
		int x = armorStartX;
		int y = armorStartY + i * slotSize;
		drawSlot(x, y, slotSize);
	}
}
void Renderer::drawSlot(int x, int y, int slotSize) {
	SDL_Rect dst{ x, y, slotSize, slotSize };
	SDL_RenderCopy(renderer, slotTexture, nullptr, &dst);
}

// When updating, use the command line below:
// "C:\Users\dyo596\C++\mingw64\bin\g++.exe" RPG_Main_Body.cpp render2d.cpp RPG_Inventory_System.cpp NPCs.cpp -DSDL_MAIN_HANDLED -IC:/Users/dyo596/C++/SDL2/include -LC:/Users/dyo596/C++/SDL2/lib -lSDL2 -lSDL2_image -lSDL2_ttf -mconsole -Wl,-subsystem,console -o game.exe
// At least until back on your laptop!