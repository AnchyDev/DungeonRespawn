#include "DungeonRespawn.h"

void DSUnitScript::OnUnitDeath(Unit* unit, Unit* /*killer*/)
{
    if (!unit)
    {
        return;
    }

    Player* player = unit->ToPlayer();
    if (!player)
    {
        return;
    }


    Map* map = player->GetMap();
    if (!map)
    {
        return;
    }

    if (!map->IsDungeon() && !map->IsRaid())
    {
        return;
    }

    /*LFGDungeonEntry const* dungeon = GetLFGDungeon(map->GetId(), map->GetDifficulty());

    if (!dungeon)
    {
        return;
    }*/

    player->TeleportToEntryPoint();
}

void SC_AddDungeonRespawnScripts()
{
    new DSUnitScript();
}
