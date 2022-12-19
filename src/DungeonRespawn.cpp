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

bool DSPlayerScript::OnBeforeTeleport(Player* player, uint32 mapid, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/)
{
    if (!drEnabled)
    {
        return true;
    }

    if (!player)
    {
        return true;
    }

    if (!IsInsideDungeonRaid(player))
    {
        if (player->GetMapId() != mapid)
        {
            auto prData = GetRespawnData(player);
            prData->isTeleportingNewMap = true;
        }
        return true;
    }

    if (!player->isDead())
    {
        return true;
    }

    GuidVector::iterator itToRemove;
    bool canRestore = false;

    for (auto it = playersToTeleport.begin(); it != playersToTeleport.end(); ++it)
    {
        if (*it == player->GetGUID())
        {
            itToRemove = it;
            canRestore = true;
            break;
        }
    }

    if (!canRestore)
    {
        return true;
    }

    playersToTeleport.erase(itToRemove);

    auto prData = GetRespawnData(player);
    if (prData)
    {
        //Invalid Player Restore data, use default behaviour.
        if (prData->dungeon.map == -1)
        {
            return true;
        }

        if (prData->dungeon.map != player->GetMapId())
        {
            return true;
        }

        player->TeleportTo(prData->dungeon.map, prData->dungeon.x, prData->dungeon.y, prData->dungeon.z, prData->dungeon.o);
        ResurrectPlayer(player);
        return false;
    }

    return true;
}

void DSWorldScript::OnAfterConfigLoad(bool reload)
{
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

void DSPlayerScript::OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
{
    if (!player)
    {
        return;
    }

    LOG_INFO("module", "new {} old {}", newZone, newArea);
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
        return;
    }

    if (!prData->isTeleportingNewMap)
    {
        return;
    }

    prData->dungeon.map = player->GetMapId();
    prData->dungeon.x = player->GetPositionX();
    prData->dungeon.y = player->GetPositionY();
    prData->dungeon.z = player->GetPositionZ();
    prData->dungeon.o = player->GetOrientation();

    prData->isTeleportingNewMap = false;
}

void DSPlayerScript::CreateRespawnData(Player* player)
{
    auto prData = GetRespawnData(player);
    if (!prData)
    {
        DungeonData newDData =
        {
            -1, //Map
            0, 0, 0, 0 //X,Y,Z,O
        };
        PlayerRespawnData newPrData =
        {
            player->GetGUID(), //PlayerGuid
            newDData, //DungeonData
            false //IsTeleportingNewMap
        };
        respawnData.push_back(newPrData);
    }
}

void DSPlayerScript::OnLogin(Player* player)
{
    if (!player)
    {
        return;
    }

    CreateRespawnData(player);
}

void SC_AddDungeonRespawnScripts()
{
    new DSWorldScript();
    new DSPlayerScript();
}
