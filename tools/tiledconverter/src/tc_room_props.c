#include <stdlib.h>
#include <string.h>
#include "tc_room_props.h"

void tcRoomPropsInit(struct TcRoomProps* roomProps)
{
    memset(roomProps, 0, sizeof(roomProps));
}

void tcRoomPropsClear(struct TcRoomProps* roomProps)
{
}

struct TcRoomProps* tcRoomPropsNew()
{
    struct TcRoomProps* roomProps = malloc(sizeof(struct TcRoomProps));
    tcRoomPropsInit(roomProps);
    return roomProps;
}

void tcRoomPropsFree(struct TcRoomProps* roomProps)
{
    tcRoomPropsClear(roomProps);
    free(roomProps);
}
