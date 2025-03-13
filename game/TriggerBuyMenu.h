#ifndef TRIGGER_BUY_MENU_H
#define TRIGGER_BUY_MENU_H

#include "Trigger.h"
#include "Player.h"

class TriggerBuyMenu : public idTrigger {
public:
    CLASS_PROTOTYPE(TriggerBuyMenu);

    void                Spawn(void);
    void                Think(void);
    void                Event_Trigger(idEntity* activator);
    void                Event_Touch(idEntity* other, trace_t* trace);
    void                Event_ShowBuymenu(idEntity* player);
    void                Event_HideBuymenu(idEntity* player);
    void                HandleSinglePlayerBuyMenu(idPlayer* player);

private:
    void                TriggerAction(idEntity* activator);
};

#endif // TRIGGER_BUY_MENU_H
