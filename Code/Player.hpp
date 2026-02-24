# ifndef PLAYER_HPP
# define PLAYER_HPP
# pragma once

# include <vector>
# include <algorithm>
# include <cmath>
# include "RPG_Inventory_System.hpp"

class Player {
	public:
		static const int MAX_LEVEL = 25;
		
		// Position
		int x = 0;
		int y = 0;
		int spawnX = 0;
		int spawnY = 0;
		
		// Stats
		int health = 100;
		int maxHealth = 100;
		int Defense = 0;
		int gold = 0;
		int xp = 0;
		int level = 0;
		
		std::vector<int> xpThresholds; // Levelling System
		void generateXPtable(); // XP Table Generation
		void addXP(int amount); // XP Gain
		void onLevelUp(); // Level Up Boosts
		
		Inventory inventory; // Inventory Construction
		Player(); // Constructor
		
		void applyDeathPenalty(); // Removes extra XP and a set amount of Gold on death
		
		void move(int dx, int dy); // Movement based on x and y change
		
		int findFirstPotionSlot() const; // Finding potion for in combat consumption
		bool consumePotion(int slot); // Consuming Potion in combat
		
		int getAttackDamage() const; // Calculating the damage the player deals
		void recalculateStats(); // Adjusts player stats every game tick for damage/health
};

# endif