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
    auto elementIndex = std::find(playersToTeleport.begin(), playersToTeleport.end(), player->GetGUID());
    if (elementIndex != playersToTeleport.end())
    {
        LOG_INFO("module", "Test2");
        player->ResurrectPlayer(1.0, false);
        playersToTeleport.erase(elementIndex);

        auto dungeonIndex = std::find(dungeons.begin(), dungeons.end(), player->GetMapId());
        if (dungeonIndex != dungeons.end())
        {
            auto dungeonData = dungeons.at(std::distance(dungeons.begin(), dungeonIndex));
            player->TeleportTo(dungeonData.map, dungeonData.x, dungeonData.y, dungeonData.z, dungeonData.o);
        }
        return false;
    }
    return true;
}

void DSWorldScript::OnAfterConfigLoad(bool /*reload*/)
{
    QueryResult result = WorldDatabase.Query("SELECT dungeonId, position_x, position_y, position_z, orientation FROM lfg_dungeon_template");

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        DungeonData dungeon;

        dungeon.map = fields[0].Get<uint32>();
        dungeon.x = fields[1].Get<float>();
        dungeon.y = fields[2].Get<float>();
        dungeon.z = fields[3].Get<float>();
        dungeon.o = fields[4].Get<float>();

        dungeons.push_back(dungeon);
    } while (result->NextRow());
}

void SC_AddDungeonRespawnScripts()
{
    new DSWorldScript();
    new DSPlayerScript();
}
