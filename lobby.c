

#include "lobby.h"

t_lobby *g_lobby;

short   g_currentMap = 0;

/**
 * @brief Update the current map index
 * 
 * @param short index 
 */
void       selectMap(short index) {
    if (!g_nbMap) return;

    if (g_currentMap + index == g_nbMap) {
        g_currentMap = 0;
        return;
    }

    printf("g_currentMap: %hd, index: %hd\n", g_currentMap, index);
    g_currentMap += index;
    if (g_currentMap < 0) {
        g_currentMap = g_nbMap - 1;
    } else if (g_currentMap > g_nbMap) {
        g_currentMap = 0;
    }
    printf("g_currentMap: %hd\n", g_currentMap);
}