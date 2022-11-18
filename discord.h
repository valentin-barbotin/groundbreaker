#ifndef DISCORD_H
 #define DISCORD_H

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <assert.h>

 #include "discord/discord_game_sdk.h"

 #define DISCORD_REQUIRE(x) assert(x == DiscordResult_Ok)

 #define CLIENT_ID  1041430157715587132

 typedef struct s_discord_app
 {
    struct IDiscordCore*                core;
    struct IDiscordUserManager*         users;
    struct IDiscordAchievementManager*  achievements;
    struct IDiscordActivityManager*     activities;
    struct IDiscordRelationshipManager* relationships;
    struct IDiscordApplicationManager*  application;
    struct IDiscordLobbyManager*        lobbies;
    DiscordUserId user_id;
 }              t_discord_app;

 void   setupDiscord();
 void   updateDiscordActivity();
#endif