// Includes
# include "NPCs.hpp"
# include <iostream>
# include <fstream>
# include <memory>
# include <unordered_map>

// General Functions for shit to work
static inline std::string trim(const std::string& s) {
	size_t start = s.find_first_not_of(" \t\r\n");
	if (start == std::string::npos) return "";
	size_t end = s.find_last_not_of(" \t\r\n");
	return s.substr(start, end - start + 1);
}

// Friendly Functions
void FriendlyNPC::setID(int id) { NPCID = id; }
void FriendlyNPC::setName(const std::string& n) { name = n; }
void FriendlyNPC::setJob(FriendlyJob j) { job = j; }
void FriendlyNPC::setHealth(int h) { health = h; }
int FriendlyNPC::getID() const { return NPCID; }
const std::string& FriendlyNPC::getName() const { return name; }
FriendlyJob FriendlyNPC::getJob() const { return job; }
int FriendlyNPC::getHealth() const { return health; }
void FriendlyNPC::interact() {
    //Placeholder for interaction code
}

// Enemy Functions
void EnemyNPC::setID(int id) { NPCID = id; }
void EnemyNPC::setName(const std::string& n) { name = n; }
void EnemyNPC::setHealth(int h) { health = h; }
void EnemyNPC::setAttack(int a) { attack = a; }
void EnemyNPC::setDefense(int d) { defense = d; }
void EnemyNPC::setXP(int x) { xp = x; }
void EnemyNPC::setGold(int g) { gold = g; }
int EnemyNPC::getID() const { return NPCID; }
const std::string& EnemyNPC::getName() const { return name; }
int EnemyNPC::getHealth() const { return health; }
int EnemyNPC::getAttack() const { return attack; }
int EnemyNPC::getDefense() const { return defense; }
int EnemyNPC::getXP() const { return xp; }
int EnemyNPC::getGold() const { return gold; }
void EnemyNPC::interact() {
    //Placeholder
}

// NPCFactory Functions
std::unordered_map<int, std::unique_ptr<NPC>> NPCFactory::loadNPCs(const std::string& filename) {
    std::unordered_map<int, std::unique_ptr<NPC>> npcs;
    std::ifstream file(filename);
    
    if (!file) {
        std::cerr << "Failed to open NPC file: " << filename << "\n";
        return npcs;
    }
    
    std::vector<std::string> block;
    std::string line;
    
    while (std::getline(file, line)) {
        if(line.find("----------") != std::string::npos) {
            if (!block.empty()) {
                auto npc = parseNPCBlock(block);
                if (npc) {
                    int id = npc->getID();
					npcs[id] = std::move(npc);
                }
                block.clear();
            }
        } else if (!line.empty()) {
            block.push_back(line);
        }
    }
    if (!block.empty()) {
        auto npc = parseNPCBlock(block);
        if (npc) {
            int id = npc->getID();
			npcs[id] = std::move(npc);
        }
    }
    
    return npcs;
}
std::unique_ptr<NPC> NPCFactory::parseNPCBlock(const std::vector<std::string>& lines) {
    int id = 0;
    std::string name = "Unknown";
    int health = 0;
    int xp = 0;
    
    // Enemy Specific
    int damage = 0;
    int defense = 0;
	int gold = 0;
    
    // Friendly Specific
    FriendlyJob job = FriendlyJob::Shop;
    
    NPCType type = NPCType::Friendly; // Defaults to friendly
    
    { // ID - Name
        std::string header = trim(lines[0]);
        auto dash = header.find('-');
        if (dash != std::string::npos) {
            id = std::stoi(header.substr(0, dash));
            name = trim(header.substr(dash + 1));
            name.erase(0, name.find_first_not_of(" \t"));
        }
    }
    
    // Remaining Information
    for (size_t i = 1; i < lines.size(); ++i) {
        std::string line = trim(lines[i]);
        if (line.empty()) continue;
        
        auto colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        
        std::string key = trim(line.substr(0, colon));
		std::string value = trim(line.substr(colon + 1));
        
        if (key == "Type") {
            if (value == "Enemy") type = NPCType::Enemy;
            else if (value == "Friendly") type = NPCType::Friendly;
        }
        else if (key == "Health") health = std::stoi(value);
        else if (key == "XP") xp = std::stoi(value);
        else if (key == "Damage") damage = std::stoi(value);
        else if (key == "Defense") defense = std::stoi(value);
		else if (key == "Gold") gold = std::stoi(value);
        else if (key == "Job") {
            if (value == "Shop") job = FriendlyJob::Shop;
            else if (value == "Quest") job = FriendlyJob::Quest;
        }
    }
    
    // Constructing the Correct NPC type
    if (type == NPCType::Enemy) {
        auto npc = std::make_unique<EnemyNPC>();
        npc->setID(id);
        npc->setName(name);
        npc->setHealth(health);
        npc->setAttack(damage);
        npc->setDefense(defense);
        npc->setXP(xp);
        npc->setType(NPCType::Enemy);
        return npc;
    }
    
    auto npc = std::make_unique<FriendlyNPC>();
    npc->setID(id);
    npc->setName(name);
    npc->setHealth(health);
    npc->setJob(job);
    npc->setType(NPCType::Friendly);
    return npc;
}


