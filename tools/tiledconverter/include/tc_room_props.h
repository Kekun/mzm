#ifndef TC_ROOM_PROPS_H
#define TC_ROOM_PROPS_H

#define TC_ROOM_PROPS_INIT { 0.0, 0.0, NULL, 0, 0, 0, NULL, 0, NULL }

struct TcRoomProps {
    double bg3ScrollingX;
    double bg3ScrollingY;
    const char* transparency;
    int bg0Priority;
    int mapX;
    int mapY;
    const char* effect;
    int effectY;
    const char* music;
};

void tcRoomPropsInit(struct TcRoomProps* roomProps);
void tcRoomPropsClear(struct TcRoomProps* roomProps);
struct TcRoomProps* tcRoomPropsNew();
void tcRoomPropsFree(struct TcRoomProps* roomProps);

#endif /* TC_ROOM_PROPS_H */
