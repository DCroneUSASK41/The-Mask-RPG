# ifndef render2d_HPP
# define render2d_HPP
# include <string>
# include <vector>
# include <unordered_map>
# include <SDL2/SDL.h>
# include <SDL2/SDL_image.h>
# include <SDL2/SDL_ttf.h>

// Classes and Structures
class Renderer {
    public:
        bool init(const char* title, int width, int height);
        void clear();
        void drawPlayer(int x, int y);
        void drawNPC(int x, int y, int id);
        void drawItem(int x, int y, int id, int slotSize);
		void drawText(std::string text, int x, int y);
        void present();
        void shutdown();
		void drawInventory();
		void drawBackdrop();
		void drawSlot(int x, int y, int slotSize);
		void drawTooltip(const std::string& name, const std::string& desc, int x, int y);
		void drawPlayerUI(int level, int xp, int maxHealth, int health, int gold, std::vector<int>& xpThresholds);
        
		int windowWidth;
		int windowHeight;
		TTF_Font* font = nullptr;
		TTF_Font* fontUI = nullptr;
		
    private:
        SDL_Texture* loadTexture(const std::string& path);
        SDL_Texture* getNPCTexture(int id);
        SDL_Texture* getItemTexture(int id);
        
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        
        SDL_Texture* playerTexture = nullptr;
		SDL_Texture* slotTexture = nullptr;
		SDL_Texture* Backdrop = nullptr;
        
        std::unordered_map<int, SDL_Texture*> npcTextures;
        std::unordered_map<int, SDL_Texture*> itemTextures;
};

# endif