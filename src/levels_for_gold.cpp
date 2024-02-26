/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
// #include "World.h"
// #include <cmath>

// #include "CreatureScript.h"
// #include "GameObjectScript.h"


#define NPC_TEXT_ID 4259
#define GOSSIP_SENDER_LFG_MAIN 1000
#define GOSSIP_ACTION_LFG_PLAYER 1001
#define GOSSIP_ACTION_LFG_SKILL 1002
#define PLAYER_MULTIPLIER 1.1
#define SKILL_MULTIPLIER 1.01


class npc_levels_for_gold : public CreatureScript
{
private:
    uint32 getPlayerLevelCost(uint8 currentLevel, uint8 desiredLevel)
    {
        uint32 cost = 0;
        for(uint8 i = currentLevel; i < desiredLevel; i++)
        {
            cost = cost + std::round(std::pow(PLAYER_MULTIPLIER,i + 1) * uint32(GOLD));
        }
        return cost;
        
    }
    uint32 getSkillLevelCost(uint16 currentLevel, uint16 desiredLevel)
    {
        uint32 cost = 0;
        for(uint16 i = currentLevel; i < desiredLevel; i++)
        {
            cost = cost + std::round(std::pow(SKILL_MULTIPLIER,i + 1) * uint32(GOLD));
        }
        return cost;
    }

public:
    npc_levels_for_gold() : CreatureScript("npc_levels_for_gold") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Buy character levels with gold", GOSSIP_SENDER_LFG_MAIN, GOSSIP_ACTION_LFG_PLAYER);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Buy profession levels with gold", GOSSIP_SENDER_LFG_MAIN, GOSSIP_ACTION_LFG_SKILL);
        SendGossipMenuFor(player, NPC_TEXT_ID, creature->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player *player, Creature *creature, uint32 sender, uint32 action) override
    {
        uint32 maxPlayerLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
        uint8 desiredLevel;
        const std::map<uint16, std::string> skillList = {
            {SKILL_ALCHEMY, "Alchemy"},
            {SKILL_BLACKSMITHING, "Blacksmithing"},
            {SKILL_COOKING, "Cooking"},
            {SKILL_ENCHANTING, "Enchanting"},
            {SKILL_ENGINEERING ,"Engineering"},
            {SKILL_FIRST_AID, "First Aid"},
            {SKILL_FISHING, "Fishing"},
            {SKILL_HERBALISM, "Herbalism"},
            {SKILL_INSCRIPTION, "Inscription"},
            {SKILL_JEWELCRAFTING, "Jewelcrafting"},
            {SKILL_LEATHERWORKING, "Leatherworking"},
            {SKILL_MINING, "Mining"},
            {SKILL_SKINNING, "Skinning"},
            {SKILL_TAILORING, "Tailoring"}
        };
        // typedef std::map<uint16, std::pair<>> SkillList;
        if(sender == GOSSIP_SENDER_LFG_MAIN)
        {
            if(action > 0 && action <= maxPlayerLevel)
            {
                // ClearGossipMenuFor(player);
                player->ModifyMoney(-getPlayerLevelCost(player->GetLevel(),action));
                player->GiveLevel(action);
                // AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "player->GiveLevel(" + std::to_string(player->GetLevel() + 1) + ")", GOSSIP_SENDER_LFG_MAIN, GOSSIP_ACTION_LFG_PLAYER);
                // SendGossipMenuFor(player, NPC_TEXT_ID, creature->GetGUID());
                // return true;
                action = GOSSIP_ACTION_LFG_PLAYER;
            }
            if(action == GOSSIP_ACTION_LFG_PLAYER)
            {
                ClearGossipMenuFor(player);
                // AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Increase Character 1 level " + std::to_string(maxPlayerLevel), GOSSIP_SENDER_LFG_MAIN, GOSSIP_ACTION_LFG_PLAYER);
                if(player->GetLevel() < maxPlayerLevel)
                {
                    desiredLevel = player->GetLevel() + 1;
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Increase character 1 level", GOSSIP_SENDER_LFG_MAIN, desiredLevel, "Buy your way to level " + std::to_string(desiredLevel) + "?", getPlayerLevelCost(player->GetLevel(),desiredLevel), false);
                }
                if(player->GetLevel() < maxPlayerLevel - 1)
                {
                    desiredLevel = player->GetLevel() + 2;
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Increase character 2 levels", GOSSIP_SENDER_LFG_MAIN, desiredLevel, "Buy your way to level " + std::to_string(desiredLevel) + "?", getPlayerLevelCost(player->GetLevel(),desiredLevel), false);
                }
                if(player->GetLevel() < maxPlayerLevel - 4)
                {
                    desiredLevel = player->GetLevel() + 5;
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Increase character 5 levels", GOSSIP_SENDER_LFG_MAIN, desiredLevel, "Buy your way to level " + std::to_string(desiredLevel) + "?", getPlayerLevelCost(player->GetLevel(),desiredLevel), false);
                }
                for(uint8 i = 10; i <= maxPlayerLevel; i+=10)
                {
                    if(player->getLevel() < i)
                    {
                        AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Increase character to level " + std::to_string(i), GOSSIP_SENDER_LFG_MAIN, i, "Buy your way to level " + std::to_string(i) + "?", getPlayerLevelCost(player->GetLevel(),i), false);
                    }
                }
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back...", GOSSIP_SENDER_LFG_MAIN, 0);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                return true;
            }
            else if(action == GOSSIP_ACTION_LFG_SKILL)
            {
                ClearGossipMenuFor(player);
                std::map<uint16, std::string>::iterator it;
                for(auto const& [skillID, label] : skillList)
                {
                    if(player->HasSkill(skillID)){
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Increase " + label + " skill", skillID, 0);
                    }
                }
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back...", GOSSIP_SENDER_LFG_MAIN, 0);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                return true;
            }
            else
            {
                // CloseGossipMenuFor(player);
                ClearGossipMenuFor(player);
                OnGossipHello(player, creature);
                return true;
            }
        }
        else
        {
            std::string label = skillList.at(sender);
            uint16 currentSkillLevel = player->GetPureSkillValue(sender);
            uint16 maxSkillLevel = player->GetPureMaxSkillValue(sender);
            if(action > 0)
            {
                player->ModifyMoney(-getSkillLevelCost(currentSkillLevel,currentSkillLevel + action));
                player->UpdateSkill(sender, action);
                action = 0;
            }
            ClearGossipMenuFor(player);
            currentSkillLevel = player->GetPureSkillValue(sender);
            maxSkillLevel = player->GetPureMaxSkillValue(sender);
            if(action == 0)
            {                
                if(currentSkillLevel < maxSkillLevel)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Buy 1 " + label + " skill point", sender, 1, "Buy 1 " + label + " skill point?", getSkillLevelCost(currentSkillLevel, currentSkillLevel + 1), false);
                }
                if(currentSkillLevel < maxSkillLevel - 4)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Buy 5 " + label + " skill points", sender, 5, "Buy 5 " + label + " skill points?", getSkillLevelCost(currentSkillLevel, currentSkillLevel + 5), false);
                }
                if(currentSkillLevel < maxSkillLevel - 9)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Buy 10 " + label + " skill points", sender, 10, "Buy 10 " + label + " skill points?", getSkillLevelCost(currentSkillLevel, currentSkillLevel + 10), false);
                }
                if(currentSkillLevel < maxSkillLevel - 24)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Buy 25 " + label + " skill points", sender, 25, "Buy 25 " + label + " skill points?", getSkillLevelCost(currentSkillLevel, currentSkillLevel + 25), false);
                }
                if(currentSkillLevel < maxSkillLevel)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Buy MAX " + label + " skill (" + std::to_string(maxSkillLevel) + ")", sender, (maxSkillLevel - currentSkillLevel), "Buy " + label + " skill point?", getSkillLevelCost(currentSkillLevel, maxSkillLevel), false);
                }
            }
            // else
            // {
            //     player->UpdateSkill(sender, action);
            // }
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back...", GOSSIP_SENDER_LFG_MAIN, GOSSIP_ACTION_LFG_SKILL);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }
    }
};

// class LevelsForGoldConf : public WorldScript
// {
// public:
//     LevelsForGoldConf() : WorldScript("LevelsForGoldConf") {}

//     void OnBeforeConfigLoad(bool /*reload*/) override
//     {
        
//     }
// };

void AddLevelsForGold()
{
    new npc_levels_for_gold();
}


// enum MyPlayerAcoreString
// {
//     HELLO_WORLD = 35410
// };

// // Add player scripts
// class MyPlayer : public PlayerScript
// {
// public:
//     MyPlayer() : PlayerScript("MyPlayer") { }

//     void OnLogin(Player* player) override
//     {
//         if (sConfigMgr->GetOption<bool>("MyModule.Enable", false))
//         {
//             ChatHandler(player->GetSession()).PSendSysMessage(HELLO_WORLD);
//         }
//     }
// };

// // Add all scripts in one
// void AddMyPlayerScripts()
// {
//     new MyPlayer();
// }
