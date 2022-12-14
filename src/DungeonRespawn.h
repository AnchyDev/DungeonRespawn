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
};

#endif //MODULE_DUNGEONRESPAWN_H
