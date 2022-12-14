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

#endif //MODULE_DUNGEONRESPAWN_H
