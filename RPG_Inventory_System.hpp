# ifndef RPG_INVENTORY_SYSTEM_HPP
# define RPG_INVENTORY_SYSTEM_HPP

# include <string>
# include <vector>
# include <memory>
# include <array>

// Class and Structure Declarations
enum class ArmorSlotType {
    Helmet,
    Coat,
    Pants,
    Boots
};
enum class Trait {
    Fire,
    Poison,
    Consumable,
    Equipable
};
struct ItemActionResult {
    bool success = false;

    // Potions
    int healAmount = 0;
    std::vector < Trait > removeDebuffs;

    // Equipment
    bool equipped = false;
    ArmorSlotType armorSlot = ArmorSlotType::Coat;
    bool isWeapon = false;

    // Generics
    bool nothingHappened = false;
};
class Item {
    public:
    void setName(const std::string& n);
    void setDescription(const std::string& d);
    void setTraits(const std::vector < Trait>& t);

    void addToStack(int amount);
    void removeFromStack(int amount);

    virtual ItemActionResult use() = 0;
    virtual ~Item() = default;

    const std::string& getName() const;
    const std::string& getDescription() const;
    const std::vector < Trait>& getTraits() const;
    int getStackCount() const;
    int getMaxStack() const;

    void setItemID(int id);
    int getItemID() const;

    protected:
    std::string Name;
    std::string Description;
    std::vector < Trait > Traits;
    int StackCount = 1;
    int MaxStack = 1;

    private:
    int ItemID = -1;
};
class Inventory {
    public:
    Inventory();

    bool addItem(std::unique_ptr < Item > item);
    std::unique_ptr < Item > removeItem(int SlotIndex);
    bool moveItem(int from, int to);
    bool equipItem(int SlotIndex);
    std::unique_ptr < Item > unequipWeapon();
    std::unique_ptr < Item > unequipArmor(ArmorSlotType type);
    Item* getItem(int SlotIndex);
    bool isSlotEmpty(int SlotIndex) const;
    const Item* getEquippedWeapon() const;
    const Item* getEquippedArmor(ArmorSlotType type) const;
    int getGeneralSlotCount() const;
    int getArmorSlotCount() const;

    private:
    std::array < std::unique_ptr < Item >,
    30 > GeneralSlots;
    std::unique_ptr < Item > WeaponSlot;
    std::array < std::unique_ptr < Item >,
    4 > ArmorSlots;

    bool isValidGeneralSlot(int Index) const;
    bool isValidArmorSlot(ArmorSlotType type) const;
    bool isWeapon(const Item& item) const;
    bool isArmor(const Item& item) const;
    bool isPotion(const Item& item) const;
    bool isStackable(const Item& item) const;
    bool canStack(const Item& a, const Item& b) const;
    void mergeStacks(Item& target, Item& source);
    int findFirstEmptyGeneralSlot() const;
    int findStackableSlot(const Item& item) const;
};
class Weapon: public Item {
    public:
    Weapon();

    ItemActionResult use() override;
    ItemActionResult equip();
    void setDamage(int d);

    private:
    int Damage;
};
class Potion: public Item {
    public:
    Potion();

    ItemActionResult use() override;
    ItemActionResult consume();
    void setHealAmount(int h);

    private:
    int HealAmount = 0;
    std::vector < Trait > Removes;
};
class Armor: public Item {
    public:
    Armor();

    ItemActionResult use() override;
    ItemActionResult equip();
    void setHealthBonus(int h);
    void setDefense(int d);
    void setSlot(ArmorSlotType s);
    ArmorSlotType getSlot() const;

    private:
    int Defense;
    int HealthBonus;
    ArmorSlotType Slot;
};
class ItemFactory {
    public:
    std::vector < std::unique_ptr < Item>> loadItems(const std::string& filename);

    private:
    std::unique_ptr < Item > parseItemBlock(const std::vector < std::string>& lines);
};
class GenericItem: public Item {
    public:
    GenericItem();
    ItemActionResult use() override;
};

#endif