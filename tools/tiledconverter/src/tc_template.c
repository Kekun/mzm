#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tc_template.h"

static const char* tmjTemplate1 =
    "{ \"compressionlevel\":-1,\n"
    " \"height\":%u,\n"
    " \"infinite\":false,\n"
    " \"layers\":[";

static const char* tileLayerTemplate1 =
    "\n"
    "        {\n"
    "         \"data\":[";

static const char* tileLayerTemplate2 =
    "],\n"
    "         \"height\":%u,\n"
    "         \"id\":%d,\n"
    "         \"name\":\"%s\",\n"
    "         \"opacity\":1,\n"
    "         \"type\":\"tilelayer\",\n"
    "         \"visible\":true,\n"
    "         \"width\":%u,\n"
    "         \"x\":0,\n"
    "         \"y\":0\n"
    "        }";

static const char* tmjTemplate2 =
    "],\n"
    " \"nextlayerid\":%d,\n"
    " \"nextobjectid\":1,\n"
    " \"orientation\":\"orthogonal\",\n"
    " \"properties\":[";

static const char* tmjPropertyTemplate =
    "\n"
    "        {\n"
    "         \"name\":\"%s\",\n"
    "         \"type\":\"%s\",\n"
    "         \"value\":%s\n"
    "        }";

static const char* tmjTemplate3 =
    "],\n"
    " \"renderorder\":\"right-down\",\n"
    " \"tiledversion\":\"1.12.2\",\n"
    " \"tileheight\":16,\n"
    " \"tilesets\":[\n"
    "        {\n"
    "         \"firstgid\":1,\n"
    "         \"source\":\"..\\/clipdata.tsj\"\n"
    "        }],\n"
    " \"tilewidth\":16,\n"
    " \"type\":\"map\",\n"
    " \"version\":\"1.10\",\n"
    " \"width\":%u\n"
    "}";

static void printComma(FILE* file, bool* needsComma)
{
    if (*needsComma)
        fprintf(file, ", ");
    else
        *needsComma = true;
}

static void fprintTileLayerData(FILE* file, unsigned int width, unsigned int height, const uint16_t* data)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (x == width - 1 && y == height - 1)
            {
                fprintf(file, "%u", data[x + y * width]);
            }
            else if (x == width - 1)
            {
                fprintf(file, "%u,\n            ", data[x + y * width]);
            }
            else
            {
                fprintf(file, "%u, ", data[x + y * width]);
            }
        }
    }
}

static void fprintTileLayer(FILE* file, const struct TcTileLayer* tileLayer, int id, const char* name, bool* prependComma)
{
    printComma(file, prependComma);
    fprintf(file, tileLayerTemplate1);
    fprintTileLayerData(file, tileLayer->width, tileLayer->height, tileLayer->data);
    fprintf(file, tileLayerTemplate2, tileLayer->height, id, name, tileLayer->width);
}

static void fprintIntProperty(FILE* file, const char* name, int value, bool* prependComma)
{
    char* valueStr = NULL;

    printComma(file, prependComma);
    asprintf(&valueStr, "%d", value);
    fprintf(file, tmjPropertyTemplate, name, "int", valueStr);

    free(valueStr);
}

static void fprintStringProperty(FILE* file, const char* name, const char* value, bool* prependComma)
{
    char* valueStr = NULL;

    assert(value != NULL);
    printComma(file, prependComma);
    asprintf(&valueStr, "\"%s\"", value);
    fprintf(file, tmjPropertyTemplate, name, "string", valueStr);

    free(valueStr);
}

static void fprintTmj(FILE* file,
                      const struct TcTileLayer* clipDataLayer,
                      const struct TcTileLayer* bg0Layer,
                      const struct TcTileLayer* bg1Layer,
                      const struct TcTileLayer* bg2Layer,
                      const struct TcTileLayer* bg3Layer,
                      const struct TcRoomProps* roomProps)
{
    int layerId = 1;

    fprintf(file, tmjTemplate1, clipDataLayer->height);

    {
        bool prependComma = false;

        /* FIXME bg3 isn't optional but it's not handled yet as it's LZ77. */
        if (bg3Layer) fprintTileLayer(file, bg3Layer, layerId++, "bg3", &prependComma);

        if (bg0Layer && roomProps->bg0Priority == 3) fprintTileLayer(file, bg0Layer, layerId++, "bg0", &prependComma);

        if (bg2Layer) fprintTileLayer(file, bg2Layer, layerId++, "bg2", &prependComma);

        if (bg0Layer && roomProps->bg0Priority == 2) fprintTileLayer(file, bg0Layer, layerId++, "bg0", &prependComma);

        /* Samus is drawn here. */

        if (bg0Layer && roomProps->bg0Priority == 1) fprintTileLayer(file, bg0Layer, layerId++, "bg0", &prependComma);

        fprintTileLayer(file, bg1Layer, layerId++, "bg1", &prependComma);

        if (bg0Layer && roomProps->bg0Priority == 0) fprintTileLayer(file, bg0Layer, layerId++, "bg0", &prependComma);

        fprintTileLayer(file, clipDataLayer, layerId++, "clipdata", &prependComma);
    }

    fprintf(file, tmjTemplate2, layerId);

    {
        bool prependComma = false;

        /* u8 tileset; */
        /* BgProperty bg0Prop; */
        /* BgProperty bg1Prop; */
        /* BgProperty bg2Prop; */
        /* BgProperty bg3Prop; */
        /* const u8* pBg0Data; */
        /* const u8* pBg1Data; */
        /* const u8* pBg2Data; */
        /* const u8* pClipData; */
        /* const u8* pBg3Data; */

        /* u8 bg3Scrolling; */
        /* u8 transparency; */

        /* const struct SpriteSpawn* pDefaultSpriteLayer; */
        /* u8 defaultSpriteset; */

        /* u8 firstSpriteLayerEvent; */
        /* const struct SpriteSpawn* pFirstSpriteLayer; */
        /* u8 firstSpriteset; */

        /* u8 secondSpriteLayerEvent; */
        /* const struct SpriteSpawn* pSecondSpriteLayer; */
        /* u8 secondSpriteset; */

        fprintIntProperty (file, "bg3Scrolling", roomProps->bg3Scrolling, &prependComma);
        if (roomProps->transparency) fprintStringProperty (file, "transparency", roomProps->transparency, &prependComma);
        fprintIntProperty (file, "mapX", roomProps->mapX, &prependComma);
        fprintIntProperty (file, "mapY", roomProps->mapY, &prependComma);
        if (roomProps->effect) fprintStringProperty (file, "effect", roomProps->effect, &prependComma);
        if (roomProps->effect) fprintIntProperty (file, "effectY", roomProps->effectY, &prependComma);
        fprintStringProperty (file, "music", roomProps->music, &prependComma);
    }

    fprintf(file, tmjTemplate3, clipDataLayer->width);
}

void tcWriteTmj(const char* filename,
                const struct TcTileLayer* clipDataLayer,
                const struct TcTileLayer* bg0Layer,
                const struct TcTileLayer* bg1Layer,
                const struct TcTileLayer* bg2Layer,
                const struct TcTileLayer* bg3Layer,
                const struct TcRoomProps* roomProps)
{
    FILE* file;

    file = fopen(filename, "w");
    if (file == NULL) {
        perror("open failed");
        return;
    }

    fprintTmj(file, clipDataLayer, bg0Layer, bg1Layer, bg2Layer, bg3Layer, roomProps);

    fclose(file);
}
