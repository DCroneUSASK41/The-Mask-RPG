// Includes
# include "RPG_Inventory_System.hpp"
# include <iostream>
# include <fstream>
# include <vector>
# include <array>

// Function Declarations


// Item Functions
const std::string& Item::getName() const {
    return Name;
}
const std::string& Item::getDescription() const {
    return Description;
}
const std::vector < Trait>& Item::getTraits() const {
    return Traits;
}
int Item::getStackCount() const {
    return StackCount;
}
int Item::getMaxStack() const {
    return MaxStack;
}
int Item::getItemID() const { return ItemID; }
void Item::setName(const std::string& n) { Name = n; } 
void Item::setDescription(const std::string& d) { Description = d; } 
void Item::setTraits(const std::vector<Trait>& t) { Traits = t; } 
void Item::addToStack(int amount) { StackCount += amount; } 
void Item::removeFromStack(int amount) { StackCount -= amount; } 
void Item::setItemID(int id) { ItemID = id; }

// Inventory Functions
Inventory::Inventory() {}
bool Inventory::addItem(std::unique_ptr < Item > item) {
    if (!item)
    return false;

    // Try Stacking first
    int stackIndex = findStackableSlot(*item);
    if (stackIndex != -1) {
        mergeStacks(*GeneralSlots[stackIndex], *item);
        if (item->getStackCount() == 0)
        return true;
    }

    // Try Empty slots
    int emptyIndex = findFirstEmptyGeneralSlot();
    if (emptyIndex != -1) {
        GeneralSlots[emptyIndex] = std::move(item);
        return true;
    }

    // Since Everything has been tested, inventory must be full
    return false;
}
std::unique_ptr < Item > Inventory::removeItem(int SlotIndex) {
    if (!isValidGeneralSlot(SlotIndex))
    return nullptr;

    return std::move(GeneralSlots[SlotIndex]);
}
bool Inventory::moveItem(int from, int to) {
    if (!isValidGeneralSlot(from) || !isValidGeneralSlot(to))
    return false;

    if (!GeneralSlots[from])
    return false;

    if (!GeneralSlots[to]) {
        GeneralSlots[to] = std::move(GeneralSlots[from]);
        return true;
    }

    if (canStack(*GeneralSlots[to], *GeneralSlots[from])) {
        mergeStacks(*GeneralSlots[to], *GeneralSlots[from]);

        if (GeneralSlots[from]->getStackCount() == 0)
        GeneralSlots[from].reset();

        return true;
    }

    std::swap(GeneralSlots[from], GeneralSlots[to]);
    return true;
}
bool Inventory::equipItem(int SlotIndex) {
    if (!isValidGeneralSlot(SlotIndex))
    return false;

    if (!GeneralSlots[SlotIndex])
    return false;

    Item& item = *GeneralSlots[SlotIndex];

    if (isWeapon(item)) {
        auto old = std::move(WeaponSlot);
        WeaponSlot = std::move(GeneralSlots[SlotIndex]);
        GeneralSlots[SlotIndex] = std::move(old);
        return true;
    }

    if (isArmor(item)) {
        Armor& armor = static_cast<Armor&>(item);
        ArmorSlotType type = armor.getSlot();

        int index = static_cast<int > (type);
        auto old = std::move(ArmorSlots[index]);
        ArmorSlots[index] = std::move(GeneralSlots[SlotIndex]);
        GeneralSlots[SlotIndex] = std::move(old);
        return true;
    }

    return false; // All else failed
}
std::unique_ptr < Item > Inventory::unequipWeapon() {
    return std::move(WeaponSlot);
}
std::unique_ptr < Item > Inventory::unequipArmor(ArmorSlotType type) {
    int index = static_cast<int > (type);
    if (!isValidArmorSlot(type))
    return nullptr;

    return std::move(ArmorSlots[index]);
}
Item* Inventory::getItem(int SlotIndex) {
    if (!isValidGeneralSlot(SlotIndex))
    return nullptr;

    return GeneralSlots[SlotIndex].get();
}
bool Inventory::isSlotEmpty(int SlotIndex) const {
    if (!isValidGeneralSlot(SlotIndex))
    return true;

    return !GeneralSlots[SlotIndex];
}
const Item* Inventory::getEquippedWeapon() const {
    return WeaponSlot.get();
}
const Item* Inventory::getEquippedArmor(ArmorSlotType type) const {
    int index = static_cast<int > (type);
    if (!isValidArmorSlot(type))
    return nullptr;

    return ArmorSlots[index].get();
}
int Inventory::getGeneralSlotCount() const {
    return static_cast<int > (GeneralSlots.size());
}
int Inventory::getArmorSlotCount() const {
    return static_cast<int > (ArmorSlots.size());
}
bool Inventory::isValidGeneralSlot(int Index) const {
    return Index >= 0 && Index < static_cast<int > (GeneralSlots.size());
}
bool Inventory::isValidArmorSlot(ArmorSlotType type) const {
    int index = static_cast<int > (type);
    return index >= 0 && index < static_cast<int > (ArmorSlots.size());
}
bool Inventory::isWeapon(const Item& item) const {
    return dynamic_cast<const Weapon*>(&item) != nullptr;
}
bool Inventory::isArmor(const Item& item) const {
    return dynamic_cast<const Armor*>(&item) != nullptr;
}
bool Inventory::isPotion(const Item& item) const {
    return dynamic_cast<const Potion*>(&item) != nullptr;
}
bool Inventory::isStackable(const Item& item) const {
    return item.getMaxStack() > 1;
}
bool Inventory::canStack(const Item& a, const Item& b) const {
    if (!isStackable(a) || !isStackable(b))
    return false;

    if (a.getName() != b.getName())
    return false;

    return typeid(a) == typeid(b);
}
void Inventory::mergeStacks(Item& target, Item& source) {
    int space = target.getMaxStack() - target.getStackCount();
    int amount = std::min(space, source.getStackCount());

    target.addToStack(amount);
    source.removeFromStack(amount);
}
int Inventory::findFirstEmptyGeneralSlot() const {
    for (int i = 0; i < GeneralSlots.size(); ++i) {
        if (!GeneralSlots[i])
        return i;
    }

    return - 1;
}
int Inventory::findStackableSlot(const Item& item) const {
    for (int i = 0; i < GeneralSlots.size(); ++i) {
        if (GeneralSlots[i] && canStack(*GeneralSlots[i], item))
        return i;
    }

    return -1;
}

// Weapon Functions
Weapon::Weapon() {
    MaxStack = 1;
    StackCount = 1;
}
ItemActionResult Weapon::use() { return equip(); }
ItemActionResult Weapon::equip() {
    ItemActionResult r;
    r.success = true;
    r.equipped = true;
    r.isWeapon = true;
    return r;
}
void Weapon::setDamage(int d) { Damage = d; }

// Armor Functions
Armor::Armor() {
    MaxStack = 1;
    StackCount = 1;
}
ItemActionResult Armor::use() { return equip(); }
ItemActionResult Armor::equip() {
    ItemActionResult r;
    r.success = true;
    r.equipped = true;
    r.armorSlot = Slot;
    return r;
}
void Armor::setHealthBonus(int h) { HealthBonus = h; }
void Armor::setDefense(int d) { Defense = d; }
void Armor::setSlot(ArmorSlotType s) { Slot = s; }
ArmorSlotType Armor::getSlot() const { return Slot; }

// Potion Functions
Potion::Potion() {
    MaxStack = 10;
    StackCount = 1;
}
ItemActionResult Potion::use() { return consume(); }
ItemActionResult Potion::consume() {
    ItemActionResult r;
    r.success = true;
    r.healAmount = HealAmount;
    r.removeDebuffs = Removes;
    return r;
}
void Potion::setHealAmount(int h) { HealAmount = h; }

// ItemFactory Functions
std::vector < std::unique_ptr < Item>> ItemFactory::loadItems(const std::string& filename) {
    std::vector < std::unique_ptr < Item>> items;
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Failed to open item file: " << filename << "\n";
        return items;
    }

    std::vector < std::string > block;
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("-----------") != std::string::npos) {
            if (!block.empty()) {
                auto item = parseItemBlock(block);
                if (item) items.push_back(std::move(item));
                block.clear();
            }
        } else if (!line.empty()) {
            block.push_back(line);
        }
    }

    // Last block
    if (!block.empty()) {
        auto item = parseItemBlock(block);
        if (item) items.push_back(std::move(item));
    }

    return items;
}
std::unique_ptr < Item > ItemFactory::parseItemBlock(const std::vector < std::string>& lines) {
    if (lines.empty())
    return nullptr;

    // First line: "1 - Oak Sword"
    std::string header = lines[0];
    auto dashPos = header.find('-');
    if (dashPos == std::string::npos)
    return nullptr;

    int itemID = std::stoi(header.substr(0, dashPos));
    std::string name = header.substr(dashPos + 1);
    name.erase(0, name.find_first_not_of(" \t"));

    // Parsed attributes
    int damage = 0;
    int defense = 0;
    int health = 0;
    int healing = 0;
	std::string description;

    ArmorSlotType armorSlot = ArmorSlotType::Coat; // default fallback
    bool hasArmorSlot = false;

    std::vector < Trait > traits;

    // Parse remaining lines
    for (size_t i = 1; i < lines.size(); ++i) {
        std::string line = lines[i];
        if (line.empty())
        continue;

        // Trim leading whitespace
        line.erase(0, line.find_first_not_of(" \t"));

        // Stat lines contain a colon
        auto colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            value.erase(0, value.find_first_not_of(" \t"));

            if (key == "Damage") damage = std::stoi(value);
            else if (key == "Defense") defense = std::stoi(value);
            else if (key == "Health") health = std::stoi(value);
            else if (key == "Healing") healing = std::stoi(value);
			else if (key == "Description") description = value;

            continue;
        }

        // Non-colon lines: either armor slot or trait
        if (line == "Head") {
            armorSlot = ArmorSlotType::Helmet;
            hasArmorSlot = true;
        }
        else if (line == "Chest") {
            armorSlot = ArmorSlotType::Coat;
            hasArmorSlot = true;
        }
        else if (line == "Pants") {
            armorSlot = ArmorSlotType::Pants;
            hasArmorSlot = true;
        }
        else if (line == "Boots") {
            armorSlot = ArmorSlotType::Boots;
            hasArmorSlot = true;
        }
        else if (line == "Equippable") {
            traits.push_back(Trait::Equipable);
        }
        else if (line == "Consumable") {
            traits.push_back(Trait::Consumable);
        }
    }

    // Decide item type
    if (damage > 0) {
        auto w = std::make_unique < Weapon > ();
        w->setItemID(itemID);
        w->setName(name);
        w->setDescription(description);
        w->setDamage(damage);
        w->setTraits(traits);
        return w;
    }

    if (defense > 0 || health > 0) {
        auto a = std::make_unique < Armor > ();
        a->setItemID(itemID);
        a->setName(name);
        a->setDescription(description);
        a->setDefense(defense);
        a->setHealthBonus(health);
        a->setTraits(traits);
        a->setSlot(armorSlot);

        return a;
    }

    if (healing > 0) {
        auto p = std::make_unique < Potion > ();
        p->setItemID(itemID);
        p->setName(name);
        p->setDescription(description);
        p->setHealAmount(healing);
        p->setTraits(traits);
        return p;
    }

    auto g = std::make_unique < GenericItem > ();
    g->setItemID(itemID);
    g->setName(name);
    g->setDescription(description);
    g->setTraits(traits);
    return g;
}

// Generic Item Functions

GenericItem::GenericItem() {
    MaxStack = 100;
    StackCount = 1;
}
ItemActionResult GenericItem::use() {
    ItemActionResult r;
    r.success = true;
    r.nothingHappened = true;
    return r;
}
