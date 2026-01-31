# include <SDL2/SDL.h>
# include <SDL2/SDL_image.h>
# include <SDL2/SDL_ttf.h>
# include <iostream>
# include <fstream>
# include <chrono>
# include <thread>
# include <cmath>
# include <memory>
# include <vector>
# include <array>
# include <unordered_map>
# include "RPG_Inventory_System.hpp"
# include "NPCs.hpp"
# include "render2d.hpp"

// Initial Class and Structure declarations
enum class GameState;
enum class CombatState;
struct InventorySlotInfo;
struct CombatContext;
class Player;

// Structures and Classes
enum class GameState {
    Camp,
    Explore,
    Combat,
    Inventory
};
GameState state = GameState::Explore;
enum class CombatState {
    PlayerTurn,
    EnemyTurn,
    Victory,
    Defeat,
    WaitingForInput
};
struct InventorySlotInfo {
	int slotIndex;
	bool isEmpty;
	std::string name;
	int stackCount;
	int itemID;
};
struct CombatContext {
    Player* player;
    EnemyNPC* enemy;
    
    int enemyHealth;
    int playerHealth;
    
    CombatState state = CombatState::PlayerTurn;
    
    int pendingDamage = 0; // for UI feedback
    int lastDamage = 0; // for UI feedback
    
    bool playerActed = false;
};
class Player {
    public:
        // Position
        int x = 0;
        int y = 0;
        int spawnX = 0;
        int spawnY = 0;
        
        // Stats
        int health = 100;
        int maxHealth = 100;
        int gold = 0;
        int xp = 0;
        int level = 0;
        
        // Leveling System
        std::vector<int> xpThresholds;
        
        // Inventory
        Inventory inventory;
        
        // Constructor
        Player() {
            generateXPtable();
        }
        
        // XP table generation
        void generateXPtable() {
            xpThresholds.clear();
            xpThresholds.push_back(0);
            xpThresholds.push_back(10);
            
            for (int i = 1; i < 25; ++i) {
                int prev = xpThresholds[i];
                int next = std::ceil(prev + std::log2(prev));
                xpThresholds.push_back(next);
            }
        }
        
        // XP Gain
        void addXP(int amount) {
            xp += amount;
            
            while (level < 25 && xp >= xpThresholds[level + 1]) {
                xp -= xpThresholds[level + 1];
                level++;
                onLevelUp();
            }
        }
        
        // Level Up Rewards
        void onLevelUp() {
            maxHealth += 10;
            health = maxHealth;
        }
        
        // Death Penalty
        void applyDeathPenalty() {
            // Reset and charge the player for death
            xp = 0;
            gold = std::max(0, gold - 100);
            
            // Spawn at the start
            x = spawnX;
            y = spawnY;
        }
        
        // Movement
        void move(int dx, int dy) {
            x += dx;
            y += dy;
        }
};

// Initial function declarations
void runGame();
void test_inventory();
std::vector<InventorySlotInfo> showInventory(Inventory& inv);
void explore(Player& player);
void fight(CombatContext* ctx, int playerChoice);
void handleDeath(Player& player);
std::vector<InventorySlotInfo> getInventoryInfo(Inventory& inv);

// Test Function
void test_inventory() {
	std::cout << "=== Inventory System Test ===\n\n";

    ItemFactory factory;
    Inventory inv;

    // Load items from file
    std::vector<std::unique_ptr<Item>> items = factory.loadItems("ItemList.txt");
    std::cout << "Loaded " << items.size() << " items from file.\n\n";

    // Print loaded items
    std::cout << "=== Loaded Items ===\n";
    for (size_t i = 0; i < items.size(); ++i) {
        Item* it = items[i].get();
        std::cout << i << ": " << it->getName();

        std::cout << " (Stack " << it->getStackCount()
                  << "/" << it->getMaxStack() << ")";

        std::cout << " [Traits:";
        for (auto t : it->getTraits())
            std::cout << " " << static_cast<int>(t);
        std::cout << "]\n";
    }

    std::cout << "\n=== Adding Items to Inventory ===\n";
    for (auto& it : items) {
        std::cout << "Adding: " << it->getName() << "\n";
        inv.addItem(std::move(it));
    }

    std::cout << "\n=== Inventory Slots After Adding ===\n";
    for (int i = 0; i < inv.getGeneralSlotCount(); ++i) {
        Item* it = inv.getItem(i);
        if (it) {
            std::cout << "Slot " << i << ": " << it->getName()
                      << " (x" << it->getStackCount() << ")\n";
        }
    }

    // Equip first weapon
    std::cout << "\n=== Equipping First Weapon ===\n";
    for (int i = 0; i < inv.getGeneralSlotCount(); ++i) {
        Item* it = inv.getItem(i);
        if (it && dynamic_cast<const Weapon*>(it)) {
            std::cout << "Equipping weapon from slot " << i << "\n";
            inv.equipItem(i);
            break;
        }
    }

    // Equip all armor
    std::cout << "\n=== Equipping Armor ===\n";
    for (int i = 0; i < inv.getGeneralSlotCount(); ++i) {
        Item* it = inv.getItem(i);
        if (it && dynamic_cast<const Armor*>(it)) {
            std::cout << "Equipping armor from slot " << i << "\n";
            inv.equipItem(i);
        }
    }

    // Show equipped items
    std::cout << "\n=== Equipped Items ===\n";

    if (auto w = inv.getEquippedWeapon())
        std::cout << "Weapon: " << w->getName() << "\n";
    else
        std::cout << "Weapon: None\n";

    std::cout << "Armor:\n";
    for (int i = 0; i < inv.getArmorSlotCount(); ++i) {
        auto a = inv.getEquippedArmor(static_cast<ArmorSlotType>(i));
        if (a)
            std::cout << "  Slot " << i << ": " << a->getName() << "\n";
        else
            std::cout << "  Slot " << i << ": Empty\n";
    }

    // Use first potion
    std::cout << "\n=== Using First Potion ===\n";
    for (int i = 0; i < inv.getGeneralSlotCount(); ++i) {
        Item* it = inv.getItem(i);
        if (it && dynamic_cast<const Potion*>(it)) {
            std::cout << "Using potion in slot " << i << "\n";
            it->use();
            break;
        }
    }

    std::cout << "\n=== Test Complete ===\n";
}

// Functions
void runGame() {
    Player player;
	ItemFactory itemfactory;
	NPCFactory npcfactory;
	Renderer renderer;
	
	auto items = itemfactory.loadItems("ItemList.txt");
	auto npcs = npcfactory.loadNPCs("NPCs.txt");
	
	for (int i = 0; i < 5; ++i) {
		player.inventory.addItem(std::move(items[i]));
	}
	
	// Finish game loop here:
	int screenWidth = 800;
	int screenHeight = 600;
	
	SDL_DisplayMode dm;
	if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
		screenWidth = dm.w;
		screenHeight = dm.h;
	}
	
	SDL_Rect usable;
	if (SDL_GetDisplayUsableBounds(0, &usable) == 0) {
		screenWidth = usable.w;
		screenHeight = usable.h;
	}
	
	renderer.init("The Mask RPG", screenWidth, screenHeight);
	bool running = true;
	
	while (running) {
	    SDL_Event e;
	    while (SDL_PollEvent(&e)) {
	        if (e.type == SDL_QUIT) { running = false; }
	    }
		
	    const Uint8* keystate = SDL_GetKeyboardState(NULL);
		if (state == GameState::Explore) { // If Exploring "Not in inventory or fight"
			if (keystate[SDL_SCANCODE_W]) player.move(0, -4);
			if (keystate[SDL_SCANCODE_A]) player.move(-4, 0);
			if (keystate[SDL_SCANCODE_S]) player.move(0, 4);
			if (keystate[SDL_SCANCODE_D]) player.move(4, 0);
		}
		
		if (keystate[SDL_SCANCODE_I]) { state = GameState::Inventory; }
		else if (keystate[SDL_SCANCODE_ESCAPE]) { state = GameState::Explore; }
		
	    renderer.clear();
	    
		if (state == GameState::Inventory) { renderer.drawInventory(); }
		if (state == GameState::Explore) { renderer.drawPlayer(player.x, player.y); }
		
	    renderer.present();
		
		SDL_Delay(16); // ~~ 60 FPS
	}
	
	renderer.shutdown();
}
std::vector<InventorySlotInfo> showInventory(Inventory& inv) { return getInventoryInfo(inv); }
std::vector<InventorySlotInfo> getInventoryInfo(Inventory& inv) {
	std::vector<InventorySlotInfo> info;
	
	int count = inv.getGeneralSlotCount();
	info.reserve(count);
	
	for (int i = 0; i < count; ++i) {
		Item* item = inv.getItem(i);
		
		InventorySlotInfo slot;
		slot.slotIndex = i;
		slot.isEmpty = (item == nullptr);
		
		if (item) {
			slot.name = item->getName();
			slot.stackCount = item->getStackCount();
			slot.itemID = item->getItemID();
			
		} else {
			slot.name = "";
			slot.stackCount = 0;
			slot.itemID = -1;
		}
		
		info.push_back(slot);
	}
	
	return info;
}
void explore(Player& player) {
	bool exploring = true;
	
	while (exploring) {
		char input;
		std::cin >> input;
		
		switch (input) {
			case 'w': player.move(0, -1); break;
			case 'a': player.move(-1, 0); break;
			case 's': player.move(0, 1); break;
			case 'd': player.move(1, 0); break;
		}
		// Space for collision checks and encounters
	}
}
void fight(CombatContext* ctx, int playerChoice) {
    if (ctx->state == CombatState::PlayerTurn) {
        
        if (playerChoice == 1) { // Attack
            int damage = 10 - ctx->enemy->getDefense();
            if (damage < 1) damage = 1;
        
            ctx->enemyHealth -= damage;
            ctx->lastDamage = damage;
            ctx->playerActed = true;
            
            if (ctx->enemyHealth <= 0) {
                ctx->state = CombatState::Victory;
                return;
            }
            
            ctx->state = CombatState::EnemyTurn;
        }
        
        else if (playerChoice == 2) { // Use Potion
            // TODO Integrate Inventory for potion consumption
            ctx->playerActed = true;
            ctx->state = CombatState::EnemyTurn;
        }
    }
}

// Main Function for execution
int main() {
	runGame();
	return 0;
}

