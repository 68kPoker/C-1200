
#define ID_NONE (0)

enum
{
    FLOOR_TYPE_FLOOR,
    FLOOR_TYPE_FLAGSTONE
};

enum
{
    OBJECT_TYPE_NONE,
    OBJECT_TYPE_WALL,
    OBJECT_TYPE_BOX
};

struct floorData
{
    int type, ID;
};

struct objectData
{
    int type, ID;
};

struct tileData
{
    struct floorData floor;
    struct objectData object;
};
