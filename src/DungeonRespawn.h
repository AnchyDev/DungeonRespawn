#ifndef MODULE_DUNGEONRESPAWN_H
#define MODULE_DUNGEONRESPAWN_H

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include <vector>

class DSPlayerScript : public PlayerScript
{
public:
    DSPlayerScript() : PlayerScript("DSPlayerScript") { }

private:
    std::vector<ObjectGuid> playersToTeleport;
    void OnPlayerReleasedGhost(Player* /*player*/) override;
    bool OnBeforeTeleport(Player* /*player*/, uint32 /*mapid*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/) override;
};

#endif //MODULE_DUNGEONRESPAWN_H
