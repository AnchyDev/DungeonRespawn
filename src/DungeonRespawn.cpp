#include "DungeonRespawn.h"

bool DSPlayerScript::IsInsideDungeonRaid(Player* player)
{
    if (!player)
    {
        return false;
    }

    Map* map = player->GetMap();
    if (!map)
    {
        return false;
    }

    if (!map->IsDungeon() && !map->IsRaid())
    {
        return false;
    }


    return true;
}
void DSPlayerScript::OnPlayerReleasedGhost(Player* player)
{
    if (!drEnabled)
    {
        return;
    }

    if (!IsInsideDungeonRaid(player))
    {
        return;
    }

    LOG_INFO("module", "Step 1");

    playersToTeleport.push_back(player->GetGUID());
}

void DSPlayerScript::ResurrectPlayer(Player* player)
{
    player->ResurrectPlayer(respawnHpPct / 100.0f, false);
    player->SpawnCorpseBones();
}

bool DSPlayerScript::OnBeforeTeleport(Player* player, uint32 /*mapid*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (!drEnabled)
    {
        return true;
    }

    if (!IsInsideDungeonRaid(player))
    {
        return true;
    }

    if (!player->isDead())
    {
        return true;
    }

    LOG_INFO("module", "Step 2");

    for (auto it = playersToTeleport.begin(); it != playersToTeleport.end(); ++it)
    {
        if (it == playersToTeleport.end())
        {
            break;
        }

        if (player->GetGUID() != *it)
        {
            continue;
        }

        LOG_INFO("module", "Step 3");

        playersToTeleport.erase(it);
        LOG_INFO("module", "Step 4");
        auto prData = GetRespawnData(player);
        if (prData)
        {
            player->TeleportTo(prData->dungeon.map, prData->dungeon.x, prData->dungeon.y, prData->dungeon.z, prData->dungeon.o);
            LOG_INFO("module", "Step 5");
        }
        else
        {
            //Some maps have different entrance locations, so we will fetch the LFG entrance.
            auto lfgDungeonEntry = GetLFGDungeon(player->GetMapId(), player->GetDifficulty(player->GetMap()->IsRaid()));
            if (lfgDungeonEntry)
            {
                LOG_INFO("module", "Step 6");
                LOG_INFO("module", "Found dungeon {}, {}", lfgDungeonEntry->ID, lfgDungeonEntry->name[0]);

                for (auto dIt = begin(dungeons); dIt != end(dungeons); ++dIt)
                {
                    if (dIt->map != lfgDungeonEntry->ID)
                    {
                        continue;
                    }

                    player->TeleportTo(lfgDungeonEntry->map, dIt->x, dIt->y, dIt->z, dIt->o);
                    ResurrectPlayer(player);

                    return false;
                }
            }

            

            AreaTriggerTeleport const* at = sObjectMgr->GetMapEntranceTrigger(player->GetMapId());
            if (at)
            {
                player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);
                ResurrectPlayer(player);

                return false;
            }
        }
    }

    return true;
}

void DSWorldScript::OnAfterConfigLoad(bool reload)
{
    if (reload)
    {
        dungeons.clear();
    }

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

    drEnabled = sConfigMgr->GetOption<bool>("DungeonRespawn.Enable", false);
    respawnHpPct = sConfigMgr->GetOption<float>("DungeonRespawn.RespawnHealthPct", 50.0f);
}

PlayerRespawnData* DSPlayerScript::GetRespawnData(Player* player)
{
    for (auto it = respawnData.begin(); it != respawnData.end(); ++it)
    {
        if (it != respawnData.end())
        {
            if (player->GetGUID() == it->guid)
            {
                return &(*it);
            }
        }
    }

    return 0;
}

void DSPlayerScript::OnMapChanged(Player* player)
{
    if (!player)
    {
        return;
    }

    if (!IsInsideDungeonRaid(player))
    {
        return;
    }

    auto prData = GetRespawnData(player);
    if (!prData)
    {
        PlayerRespawnData newPrData;
        DungeonData newDData;
        newPrData.guid = player->GetGUID();
        newPrData.dungeon = newDData;
        newPrData.dungeon.map = player->GetMapId();
        newPrData.dungeon.x = player->GetPositionX();
        newPrData.dungeon.y = player->GetPositionY();
        newPrData.dungeon.z = player->GetPositionZ();
        respawnData.push_back(newPrData);
    }
    else
    {
        prData->guid = player->GetGUID();
        prData->dungeon.map = player->GetMapId();
        prData->dungeon.x = player->GetPositionX();
        prData->dungeon.y = player->GetPositionY();
        prData->dungeon.z = player->GetPositionZ();
    }
}

void SC_AddDungeonRespawnScripts()
{
    new DSWorldScript();
    new DSPlayerScript();
}
