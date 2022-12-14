#include "DungeonRespawn.h"

bool DSPlayerScript::IsInsideDungeonRaid(Player* player)
{
    if (!player)
    {
        LOG_INFO("module", "Failed to find player when releasing..");
        return false;
    }

    Map* map = player->GetMap();
    if (!map)
    {
        LOG_INFO("module", "Failed to find map for player {}", player->GetName());
        return false;
    }

    LOG_INFO("module", "Found map {} for player {}", map->GetId(), player->GetName());

    if (!map->IsDungeon() && !map->IsRaid())
    {
        LOG_INFO("module", "Not a dungeon/raid.");
        return false;
    }

    LOG_INFO("module", "Player is inside dungeon/raid.");

    return true;
}
void DSPlayerScript::OnPlayerReleasedGhost(Player* player)
{
    if (!IsInsideDungeonRaid(player))
    {
        return;
    }

    playersToTeleport.push_back(player->GetGUID());
}

void DSPlayerScript::ResurrectPlayer(Player* player)
{
    LOG_INFO("module", "Resurrecting player..");
    player->ResurrectPlayer(1.0, false);
    player->SpawnCorpseBones();
    LOG_INFO("module", "Resurrected player");
}

bool DSPlayerScript::OnBeforeTeleport(Player* player, uint32 /*mapid*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (!IsInsideDungeonRaid(player))
    {
        return true;
    }

    for (auto it = begin(playersToTeleport); it != end(playersToTeleport); ++it)
    {
        if (player->GetGUID() != *it)
        {
            continue;
        }

        //Some maps have differen.t entrance locations, so we will fetch the LFG entrance.
        auto lfgDungeonEntry = GetLFGDungeon(player->GetMapId(), player->GetDifficulty(player->GetMap()->IsRaid()));
        if (lfgDungeonEntry)
        {
            ResurrectPlayer(player);

            LOG_INFO("module", "VECOUNT: {}", playersToTeleport.size());
            playersToTeleport.erase(it);
            LOG_INFO("module", "VECOUNT: {}", playersToTeleport.size());

            for (auto dIt = begin(dungeons); dIt != end(dungeons); ++dIt)
            {
                if (dIt->map != lfgDungeonEntry->map)
                {
                    continue;
                }

                LOG_INFO("module", "Overriding teleport..");
                player->TeleportTo(dIt->map, dIt->x, dIt->y, dIt->z, dIt->o);
                LOG_INFO("module", "Overrided teleport");
            }
            return false;
        }

        AreaTriggerTeleport const* at = sObjectMgr->GetMapEntranceTrigger(player->GetMapId());
        if (at)
        {
            LOG_INFO("module", "Found area trigger mapid {}", at->target_mapId);

            ResurrectPlayer(player);

            LOG_INFO("module", "VECOUNT: {}", playersToTeleport.size());
            playersToTeleport.erase(it);
            LOG_INFO("module", "VECOUNT: {}", playersToTeleport.size());

            LOG_INFO("module", "Overriding teleport..");
            player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);
            LOG_INFO("module", "Overrided teleport");

            return false;
        }
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
