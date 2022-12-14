#ifndef MODULE_DUNGEONRESPAWN_H
#define MODULE_DUNGEONRESPAWN_H

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

class DSUnitScript : public UnitScript
{
public:
    DSUnitScript() : UnitScript("DSUnitScript") { }

private:
    void OnUnitDeath(Unit* /*unit*/, Unit* /*killer*/) override;
};

class DSPlayerScript : public PlayerScript
{
public:
    DSPlayerScript() : PlayerScript("DSPlayerScript") { }

private:
    void OnPlayerReleasedGhost(Player* /*player*/) override;
    bool OnBeforeTeleport(Player* /*player*/, uint32 /*mapid*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/, uint32 /*options*/, Unit* /*target*/) override;
};

#endif //MODULE_DUNGEONRESPAWN_H
