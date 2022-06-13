
#include "GameData.h"
#include "SystemData.h"

/* Root program structure */

struct programData
{
    struct gameData gameData;
    struct systemData sysData; /* OS data */
};
