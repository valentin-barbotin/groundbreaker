#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "discord.h"


t_discord_app           *g_app_discord = NULL;
struct DiscordActivity  *g_activity_discord = NULL;


void    setupDiscord() {
    t_discord_app   app;
    g_app_discord = &app;

    memset(&app, 0, sizeof(app));


    struct IDiscordActivityEvents activities_events;
    memset(&activities_events, 0, sizeof(activities_events));

    // struct IDiscordRelationshipEvents relationships_events;
    // memset(&relationships_events, 0, sizeof(relationships_events));
    // relationships_events.on_refresh = OnRelationshipsRefresh;

    struct DiscordCreateParams params;
    DiscordCreateParamsSetDefault(&params);
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = &app;
    params.activity_events = &activities_events;
    // params.relationship_events = &relationships_events;
    // params.user_events = &users_events;
    DISCORD_REQUIRE(DiscordCreate(DISCORD_VERSION, &params, &app.core));

    app.users = app.core->get_user_manager(app.core);
    app.achievements = app.core->get_achievement_manager(app.core);
    app.activities = app.core->get_activity_manager(app.core);
    app.application = app.core->get_application_manager(app.core);
    app.lobbies = app.core->get_lobby_manager(app.core);
    app.relationships = app.core->get_relationship_manager(app.core);

    app.activities->clear_activity(app.activities, NULL, NULL);

    puts("Discord initialized");

    struct DiscordActivity activity;
    g_activity_discord = &activity;
    memset(&activity, 0, sizeof(activity));

    strcpy(activity.state, "(Idle)");
    strcpy(activity.details, "Description");
    strcpy(activity.assets.large_image, "fvtud6yx0aa7zha");
    strcpy(activity.assets.large_text, "ratio");
    // strcpy(activity.party.id, "party_id");
    // strcpy(activity.secrets.join, "join_secret");
    // strcpy(activity.secrets.spectate, "spectate_secret");
    // strcpy(activity.secrets.match, "match_secret");
    activity.timestamps.start = 0;
    // activity.party.size.current_size = 1;
    // activity.party.size.max_size = 2;
    // activity.instance = 1;

    app.activities->update_activity(app.activities, &activity, &app, NULL);

    for (;;) {
        DISCORD_REQUIRE(g_app_discord->core->run_callbacks(g_app_discord->core));
        sleep(4);
    }
}

void    updateDiscordActivity() {
    g_app_discord->activities->update_activity(g_app_discord->activities, &g_activity_discord, g_app_discord, NULL);
}
