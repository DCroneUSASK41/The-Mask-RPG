# ifndef render2d_HPP
# define render2d_HPP
# include <string>
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
        void present();
        void shutdown();
		void drawInventory();
		void drawSlot(int x, int y, int slotSize);
		void drawTooltip(const std::string& name, const std::string& desc, int x, int y);
        
		int windowWidth;
		int windowHeight;
		TTF_Font* font = nullptr;
		
    private:
        SDL_Texture* loadTexture(const std::string& path);
        SDL_Texture* getNPCTexture(int id);
        SDL_Texture* getItemTexture(int id);
        
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        
        SDL_Texture* playerTexture = nullptr;
		SDL_Texture* slotTexture = nullptr;
        
        std::unordered_map<int, SDL_Texture*> npcTextures;
        std::unordered_map<int, SDL_Texture*> itemTextures;
};

# endif