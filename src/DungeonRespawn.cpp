#include "DungeonRespawn.h"

/*void DSUnitScript::OnUnitDeath()
{
    LOG_INFO("module", "1");
    if (!unit)
    {
        return;
    }
    LOG_INFO("module", "2");
    Player* player = unit->ToPlayer();
    if (!player)
    {
        return;
    }

    LOG_INFO("module", "3");
    Map* map = player->GetMap();
    if (!map)
    {
        return;
    }
    LOG_INFO("module", "4");
    if (!map->IsDungeon() && !map->IsRaid())
    {
        return;
    }
    LOG_INFO("module", "5");

    //player->TeleportToEntryPoint();
}*/

void DSPlayerScript::OnPlayerReleasedGhost(Player* player)
{
    playersToTeleport.push_back(player->GetGUID());
    LOG_INFO("module", "Test1");
}

bool DSPlayerScript::OnBeforeTeleport(Player* player, uint32 /*mapid*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (std::count(playersToTeleport.begin(), playersToTeleport.end(), player->GetGUID()))
    {
        LOG_INFO("module", "Test2");
        player->ResurrectPlayer(1.0, false);
        return false;
    }
    return true;
}

void SC_AddDungeonRespawnScripts()
{
    new DSPlayerScript();
}
