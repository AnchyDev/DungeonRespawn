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

    for (auto it = begin(playersToTeleport); it != end(playersToTeleport); ++it)
    {
        if (player->GetGUID() != *it)
        {
            continue;
        }

        playersToTeleport.erase(it);

        //Some maps have different entrance locations, so we will fetch the LFG entrance.
        auto lfgDungeonEntry = GetLFGDungeon(player->GetMapId(), player->GetDifficulty(player->GetMap()->IsRaid()));
        if (lfgDungeonEntry)
        {
            ChatHandler(player->GetSession()).SendSysMessage(Acore::StringFormatFmt("Found dungeon {}, {}", lfgDungeonEntry->ID, lfgDungeonEntry->name));

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

void SC_AddDungeonRespawnScripts()
{
    new DSWorldScript();
    new DSPlayerScript();
}
