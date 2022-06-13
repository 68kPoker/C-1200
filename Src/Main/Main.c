
#include "ProgramData.h"
#include "Program.h"
#include "Board.h"
#include "Tile.h"

int main(void)
{
    struct programData pd;
    FILE *f;

    struct tileData *td = &pd.gameData.boardData.tiles[3][3];

    resetProgram(&pd);
    
    replaceObject(OBJECT_TYPE_BOX, ID_NONE, &td->object, td, &pd.gameData.boardData, &pd.gameData, &pd);

    scanBoard(&pd.gameData.boardData, &pd.gameData, &pd);

    if (!fopen_s(&f, "Board.txt", "w"))
    {
        fprintf(f, "Boxes: %d\n", pd.gameData.boardData.allBoxes);

        drawBoard(f, &pd.gameData.boardData, &pd.gameData, &pd);
        fclose(f);
    }

    return(0);

}
