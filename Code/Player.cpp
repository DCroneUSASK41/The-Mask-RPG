// Includes
# include <iostream>
# include <vector>
# include <algorithm>
# include <cmath>
# include "RPG_Inventory_System.hpp"
# include "Player.hpp"

void Player::generateXPtable() {
	xpThresholds.clear();
	xpThresholds.resize(MAX_LEVEL + 1);
	
	xpThresholds[0] = 0;
	xpThresholds[1] = 10;
	
	for (int i = 1; i < MAX_LEVEL; ++i) {
		int prev = xpThresholds[i];
		int next = std::ceil(prev + std::log2(prev));
		xpThresholds[i + 1] = next;
	}
}
void Player::addXP(int amount) {
	xp += amount;
	
	if (level == MAX_LEVEL) {
		xp = std::min(xp, xpThresholds[MAX_LEVEL]);
	}
	
	while (level < MAX_LEVEL && xp >= xpThresholds[level + 1]) {
		xp -= xpThresholds[level + 1];
		level++;
		onLevelUp();
	}
}
void Player::onLevelUp() {
	maxHealth += 10;
	health = maxHealth;
	if (level >= MAX_LEVEL) return;
}
Player::Player() {
	generateXPtable();
}
void Player::applyDeathPenalty() {
	// Reset and charge the player for death
	xp = 0;
	gold = std::max(0, gold - 100);
	
	// Spawn at the start
	health = maxHealth;
	x = spawnX;
	y = spawnY;
}
void Player::move(int dx, int dy) {
	x += dx;
	y += dy;
}
int Player::findFirstPotionSlot() const {
	for (int i = 0; i < inventory.getGeneralSlotCount(); ++i) {
		const Item* it = inventory.getItem(i);
		if (it && dynamic_cast<const Potion*>(it)) {
			return i;
		}
	}
	return - 1;
}
bool Player::consumePotion(int slot) {
	Item* it = inventory.getItem(slot);
	Potion* potion = dynamic_cast<Potion*>(it);
	if (!potion) return false;
	
	ItemActionResult r = potion->use();
	
	if (r.success) {
		health = std::min(maxHealth, health + r.healAmount);
		potion->removeFromStack(1);
		if (potion->getStackCount() <= 0) {
			inventory.removeItem(slot);
		}
		
		return true;
	}
	
	return false;
}
int Player::getAttackDamage() const {
	const Weapon* w = dynamic_cast<const Weapon*>(inventory.getEquippedWeapon());
	if (w) return w->getDamage();
	return 1; // Unarmed Damage
}
void Player::recalculateStats() {
	int baseHealth = 100 + level * 10;
	int baseDefense = 0;
	
	auto boosts = inventory.getStatBoosts();
	
	maxHealth = baseHealth + boosts.health;
	Defense = baseDefense + boosts.defense;
	
	if (health > maxHealth)
		health = maxHealth;
}