
#include "GameData.h"
#include "EditorData.h"
#include "SystemData.h"

/* Root program structure */

struct programData
{
    struct gameData gameData;
    struct editorData editData; /* Game-editor data */
    struct systemData sysData; /* OS data */
};
