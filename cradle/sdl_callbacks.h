// The cradle library - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

// The core callbacks required by SDL - SDL_AppInit, SDL_AppIterate,
// SDL_AppEvent, and SDL_AppQuit. These callbacks are implemented as passthroughs into
// the corresponding init, idle, process_event, and quit static methods
// declared in main.h
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    size_t i;

    if (!SDL_SetAppMetadata(app_metadata[0], app_metadata[1], app_metadata[2])) {
        return SDL_APP_FAILURE;
    }

    for (i = 0; i < SDL_arraysize(extended_metadata); i++) {
        if (!SDL_SetAppMetadataProperty(extended_metadata[i].key, extended_metadata[i].value)) {
            return SDL_APP_FAILURE;
        }
    }
    
    if(!CRADLE_GAME::init(argc, argv))
        return SDL_APP_FAILURE;
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    CRADLE_GAME::idle();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return CRADLE_GAME::process_event(event);
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    CRADLE_GAME::quit();
}

