#ifndef MODULE_DUNGEONRESPAWN_H
#define MODULE_DUNGEONRESPAWN_H

#include "ScriptMgr.h"
#include "LFGMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include <vector>
#include "TaskScheduler.h"

struct DungeonData
{
    uint32 map;
    float x;
    float y;
    float z;
    float o;
};

std::vector<DungeonData> dungeons;

bool drEnabled;
float respawnHpPct;

class DSPlayerScript : public PlayerScript
{
public:
    DSPlayerScript() : PlayerScript("DSPlayerScript") { }

private:
    std::vector<ObjectGuid> playersToTeleport;
    TaskScheduler taskScheduler;
    bool IsInsideDungeonRaid(Player* /*player*/);
    void ResurrectPlayer(Player* /*player*/);
    void OnPlayerReleasedGhost(Player* /*player*/) override;
    bool OnBeforeTeleport(Player* /*player*/, uint32 /*mapid*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/) override;
};

class DSWorldScript : public WorldScript
{
public:
    DSWorldScript() : WorldScript("DSWorldScript") { }

private:
    void OnAfterConfigLoad(bool /*reload*/) override;
};

#endif //MODULE_DUNGEONRESPAWN_H
