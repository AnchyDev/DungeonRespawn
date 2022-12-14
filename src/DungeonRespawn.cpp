#include "DungeonRespawn.h"

void DSUnitScript::OnUnitDeath(Unit* unit, Unit* /*killer*/)
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
    /*LFGDungeonEntry const* dungeon = GetLFGDungeon(map->GetId(), map->GetDifficulty());

    if (!dungeon)
    {
        return;
    }*/

    //player->TeleportToEntryPoint();
}

void DSPlayerScript::OnPlayerReleasedGhost(Player* player)
{
    player->ResurrectPlayer(1.0, false);
}

bool DSPlayerScript::OnBeforeTeleport(Player* /*player*/, uint32 /*mapid*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    return true;
}

void SC_AddDungeonRespawnScripts()
{
    new DSUnitScript();
    new DSPlayerScript();
}
