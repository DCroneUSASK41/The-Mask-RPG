#include <iostream>
#include <string>
# include <vector>
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
	
	Backdrop = loadTexture("Assets/Background.jpg");
	
	font = TTF_OpenFont("Assets/font.ttf", 20);
	fontUI = TTF_OpenFont("Assets/font.ttf", 10);
	if (!font || !fontUI) {
		std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
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

    SDL_Rect dst{ x, y, 100, 100 };
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
void Renderer::drawText(std::string text, int x, int y) {
	if (!font) return;
	
	SDL_Color white = {255, 255, 255, 255};
	
	SDL_Surface* surf = TTF_RenderText_Blended(fontUI, text.c_str(), white);
	if (!surf) return;
	
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	
	if (!tex) return;
	
	int w, h;
	SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
	
	SDL_Rect dst{ x, y, w, h };
	SDL_RenderCopy(renderer, tex, nullptr, &dst);
	
	SDL_DestroyTexture(tex);
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
void Renderer::drawTooltip(const std::string& name, const std::string& desc, int x, int y) {
	if (!font) return;
	
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color black = {0, 0, 0, 200};
	
	SDL_Surface* nameSurf = TTF_RenderText_Blended(font, name.c_str(), white);
	SDL_Surface* descSurf = TTF_RenderText_Blended_Wrapped(font, desc.c_str(), white, 150);
	
	SDL_Texture* nameTex = SDL_CreateTextureFromSurface(renderer, nameSurf);
	SDL_Texture* descTex = SDL_CreateTextureFromSurface(renderer, descSurf);
	
	int w1, h1, w2, h2;
	SDL_QueryTexture(nameTex, nullptr, nullptr, &w1, &h1);
	SDL_QueryTexture(descTex, nullptr, nullptr, &w2, &h2);
	
	int padding = 8;
	int boxW = std::max(w1, w2) + padding * 2;
	int boxH = h1 + h2 + padding * 3;
	
	SDL_Rect bg{ x, y, boxW, boxH };
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 80);
	SDL_RenderFillRect(renderer, &bg);
	
	SDL_Rect nameDst{ x + padding, y + padding, w1, h1 };
	SDL_Rect descDst{ x + padding, y + padding + h1 + 4, w2, h2 };
	
	SDL_RenderCopy(renderer, nameTex, nullptr, &nameDst);
	SDL_RenderCopy(renderer, descTex, nullptr, &descDst);
	
	SDL_FreeSurface(nameSurf);
	SDL_FreeSurface(descSurf);
	SDL_DestroyTexture(nameTex);
	SDL_DestroyTexture(descTex);
}
void Renderer::drawPlayerUI(int level, int xp, int maxHealth, int health, int gold, std::vector<int>& xpThresholds) {
	float scale = windowHeight / 1080.0f;
	
	int panelW = int(300 * scale);
	int panelH = int(140 * scale);
	int margin = int(20 * scale);
	
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 80); // UI Background
	SDL_Rect panel{ margin, margin, panelW, panelH };
	SDL_RenderFillRect(renderer, &panel);
	
	int barW = int(220 * scale); // Bar Dimensions
	int barH = int(20 * scale);
	
	int hpW = (barW * health) / maxHealth; // Health Bar
	SDL_Rect hpBack{
		int(margin + 20 * scale),
		int(margin + 20 * scale), 
		barW, 
		barH 
	};
	SDL_Rect hpFront{ 
		int(margin + 20 * scale), 
		int(margin + 20 * scale), 
		hpW, 
		barH 
	};
	
	SDL_SetRenderDrawColor(renderer, 80, 0, 0, 80);
	SDL_RenderFillRect(renderer, &hpBack);
	SDL_SetRenderDrawColor(renderer, 200, 0, 0, 160);
	SDL_RenderFillRect(renderer, &hpFront);
	
	int nextXP = xpThresholds[level + 1]; // XP bar
	int xpW = (barW * xp) / nextXP;
	SDL_Rect xpBack{ 
		int(margin + 20 * scale), 
		int(margin + 60 * scale), 
		barW, 
		barH 
	};
	SDL_Rect xpFront{ 
		int(margin + 20 * scale), 
		int(margin + 60 * scale), 
		xpW, 
		barH 
	};

	SDL_SetRenderDrawColor(renderer, 0, 80, 0, 80);
	SDL_RenderFillRect(renderer, &xpBack);
	SDL_SetRenderDrawColor(renderer, 0, 200, 0, 160);
	SDL_RenderFillRect(renderer, &xpFront);
	
	drawText("Health: " + std::to_string(health) + "/" + std::to_string(maxHealth),
			 int(margin + 20 * scale), int(margin + 45 * scale));
	drawText("XP: " + std::to_string(xp) + "/" + std::to_string(nextXP),
			 int(margin + 20 * scale), int(margin + 85 * scale));
	drawText("Level: " + std::to_string(level),
			 int(margin + 20 * scale + 100 * scale), int(margin + 85 * scale));
	drawText("Gold: " + std::to_string(gold), int(margin + 20 * scale), int(margin + 110 * scale));
}
void Renderer::drawBackdrop() {
	SDL_Rect background { 0, 0, windowWidth, windowHeight };
	SDL_RenderCopy(renderer, Backdrop, nullptr, &background);
}

// When updating, use the command line below:
// "C:\Users\dyo596\C++\mingw64\bin\g++.exe" RPG_Main_Body.cpp render2d.cpp RPG_Inventory_System.cpp NPCs.cpp -DSDL_MAIN_HANDLED -IC:/Users/dyo596/C++/SDL2/include -LC:/Users/dyo596/C++/SDL2/lib -lSDL2 -lSDL2_image -lSDL2_ttf -mconsole -Wl,-subsystem,console -o game.exe
// At least until back on your laptop!