#include "../idlib/precompiled.h"
#pragma hdrstop

#include "TriggerBuyMenu.h"
#include "Player.h"

//const idEventDef EV_ShowBuymenu("showBuymenu", "e");
//const idEventDef EV_HideBuymenu("hideBuymenu", "e");

CLASS_DECLARATION(idTrigger, TriggerBuyMenu)
EVENT(EV_Activate, TriggerBuyMenu::Event_Trigger)
EVENT(EV_Touch, TriggerBuyMenu::Event_Touch)
EVENT(EV_ShowBuymenu, TriggerBuyMenu::Event_ShowBuymenu)
EVENT(EV_HideBuymenu, TriggerBuyMenu::Event_HideBuymenu)
END_CLASS

void TriggerBuyMenu::Spawn(void) {
    GetPhysics()->SetContents(CONTENTS_TRIGGER);
    GetPhysics()->SetClipMask(MASK_PLAYERSOLID);

    BecomeActive(TH_THINK);
}

void TriggerBuyMenu::Think(void) {
    idTrigger::Think();
}

void TriggerBuyMenu::Event_Trigger(idEntity* activator) {
    idPlayer* player = dynamic_cast<idPlayer*>(activator);
    if (player) {
        HandleSinglePlayerBuyMenu(player);
    }
}

void TriggerBuyMenu::Event_Touch(idEntity* other, trace_t* trace) {
    if (!other || !other->IsType(idPlayer::GetClassType())) {
        return;
    }

    idPlayer* player = static_cast<idPlayer*>(other);
    if (player->spectating) {
        return;
    }

    HandleSinglePlayerBuyMenu(player);
}

void TriggerBuyMenu::Event_ShowBuymenu(idEntity* player) {
    idPlayer* p = dynamic_cast<idPlayer*>(player);
    if (p) {
        p->ShowBuymenu();
    }
}

void TriggerBuyMenu::Event_HideBuymenu(idEntity* player) {
    idPlayer* p = dynamic_cast<idPlayer*>(player);
    if (p) {
        p->HideBuymenu();
    }
}

void TriggerBuyMenu::HandleSinglePlayerBuyMenu(idPlayer* player) {
    if (player) {
        player->ShowBuymenu();
    }
}


void TriggerBuyMenu::TriggerAction(idEntity* activator) {
    idPlayer* player = dynamic_cast<idPlayer*>(activator);
    if (player) {
        player->ShowBuymenu();
    }
}
