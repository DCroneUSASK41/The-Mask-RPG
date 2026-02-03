# ifndef NPCs_HPP
# define NPCs_HPP

# include <string>
# include <vector>
# include <memory>
# include <unordered_map>

// Class Declarations
enum class FriendlyJob {
	Shop,
	Quest
};
enum class NPCType {
    Friendly,
    Enemy
};
class NPC {
    public:
        virtual ~NPC() = default;
        
        virtual void setID(int id) = 0;
        virtual void setName(const std::string& n) = 0;
        virtual void setHealth(int h) = 0;
        
        virtual int getID() const = 0;
        virtual const std::string& getName() const = 0;
        virtual int getHealth() const = 0;
        
        virtual NPCType getType() const { return type; }
        virtual void setType(NPCType t) { type = t; }
        
        virtual void interact() = 0;
		
		virtual int getAttack() const { return 0; };
		virtual int getDefense() const { return 0; };
		virtual getXP() const { return 0; };
		virtual getGold() const { return 0; };
    
    protected:
        NPCType type;
};
class FriendlyNPC : public NPC {
	public:
		void setID(int id) override;
		void setName(const std::string& n) override;
		void setJob(FriendlyJob j);
		void setHealth(int h) override;
		
		int getID() const override;
		const std::string& getName() const override;
		FriendlyJob getJob() const;
		int getHealth() const override;
		
		void setType(NPCType t) override { type = t; }
		
		void interact() override;
	
	private:
		int NPCID;
		std::string name;
		int health;
		FriendlyJob job;
};
class EnemyNPC : public NPC {
	public:
		void setID(int id) override;
		void setName(const std::string& n) override;
		void setHealth(int h) override;
		void setAttack(int a);
		void setDefense(int d);
		void setXP(int x);
		void setGold(int g);
		
		int getID() const override;
		const std::string& getName() const override;
		int getHealth() const override;
		int getAttack() const;
		int getDefense() const;
		int getXP() const;
		int getGold() const;
		
		void setType(NPCType t) override { type = t; }
		
		void interact() override;
	
	private:
		int NPCID;
		std::string name;
		int health;
		int attack;
		int defense;
		int xp;
		int gold;
};
class NPCFactory {
	public:
		std::unordered_map<int, std::unique_ptr<NPC>> loadNPCs(const std::string& filename);
	
	private:
	    std::unique_ptr<NPC> parseNPCBlock(const std::vector<std::string>& lines);
};

# endif