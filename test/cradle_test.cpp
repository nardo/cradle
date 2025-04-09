// CradleTest - copyright KAGR LLC. The use of this source code is governed by the license agreement(s) described in the "license.txt" file in this directory.

// Cradle Engine test program root build file
// Most of the code for Cradle apps is compiled from a single .cpp file. This test program root file walks through the structure of a cradle app.  Enjoy!

// Cradle is built on SDL3 for its main hardware platform abstraction (ie video, input, etc.). SDL introduced "main() callbacks" in version 3 to account for various platforms' init/loop structure, which Cradle apps use:

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#define GL_SILENCE_DEPRECATION

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3_image/SDL_image.h"

#include "SDL3/SDL_opengl.h"
#include "SDL3/SDL_opengl_glext.h"

// Metadata strings for SDL:
static const char *app_metadata[3] = { "Cradle Test", "0.42a", "dev.athenacore.cradle" };
static const struct
{
    const char *key;
    const char *value;
} extended_metadata[] =
{
    { SDL_PROP_APP_METADATA_URL_STRING, "https://athenacore.dev/cradle/" },
    { SDL_PROP_APP_METADATA_CREATOR_STRING, "Cradle Dev Team" },
    { SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "(C) 2025 KAGR LLC" },
    { SDL_PROP_APP_METADATA_TYPE_STRING, "game" }
};
// Next we loop in some standard system headers...
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include <algorithm>

// Cradle includes support for networked voice conversation, so we include some GSM codec magic to crunch that audio into tiny packets...

#include "voice_compression/gsm.h"
#include "voice_compression/lpc10.h"
#include "voice_compression/ftol.h"

// loop in platform.h, which sets up all of the macros that let us tell which OS/CPU we're being bult for.
#include "core/platform.h"

// Cradle uses some GL extensions, so include those here
#include "platforms/gl_extensions.h"

// For audio playback, Cradle uses the OpenAL audio library in order to allow for 3D positional sound

#if defined(PLATFORM_WEB)
#include <AL/alc.h>
#include <AL/al.h>
#elif defined(PLATFORM_MAC_OSX)
#include <openal/alc.h>
#include <openal/al.h>
#else
#include "platforms/openal/alc.h"
#include "platforms/openal/al.h"
#endif

#include "decomposition-library/include/decomposition.h"
using namespace decomposition;

// Here's where things get fun...
// Large C++ projects have a tendency to be not very fun to work with in no small part due to excruciatingly long build times, as well as organizationally - classes are typically split into declaration and implementation files (.h/.cpp), and each .cpp has to include all the .h files for other classes referenced in an implementation. As the size of the project grows, each .cpp compile can include tens or hundreds of thousands of lines of header code, meaning that a change in a commonly used header can trigger a recompile of many files, each of which includes a ton of code.

// Further, splitting declaration and implementation means duplication of function signatures, and a lot of class_name:: garbage in every file.

// Cradle uses a novel approach to get around this problem. All of Cradle's classes are fully declared and implemented in individual header files, and although C++ doesn't allow forward references for undefined classes at global scope, it does allow forward referencing inside of classes and structures... so Cradle apps put the inclusion of (almost) all of their classes inside of a single struct declaration.

// This approach radically improves iteration time: every build (excluding changes to core libraries) involves only a single .cpp file compile, meaning that every change carries equal time cost. Further, refactors are simpler, declarations and implementations don't get out of sync, and overall project LOC is reduced - no duplication of method signatures, no massive include blocks in each file, etc. Overall, it's a massive win.

// That said, there are some ideosynchracies of this approach that take getting used to: First, wrapped files cannot declare global variables, since all of the included files are wrapped in a struct declared at global scope. For this reason, we also delcare a global single object of the wrapping struct with a short name (A). Any object instanced at file-level scope in an included file will show up as a member variable of the wrapping struct and can be referenced from the global struct object. For example, UserInterface.h declares a struct UIGlobals named "UI", and this "global" is referenced in the code as "A.UI". "Global" functions must be declared as "static" - becoming static class member functions of the wrapping struct.

// Second, any errors in definition scoping (ie missing a closing curly brace, etc.) will cause a cascade of compile errors, and some IDEs will rearrange compiler error output, which can make for real head-scratchers. For this reason, fixing compile errors is often easier managed working off the actual error output from the compiler/build rather than in the processed error window. Moving on...

// First, forward declarations for the global application struct and instance. We still need to forward-declare names, but not implementations.

struct cradle_test;
extern cradle_test A;

// Cradle's networking libraries (notify_sockets, replicator) use libtomcrypt for public key and symmetric encryption. As with system headers above, library headers need to be included outside the app struct.
#include "libtomcrypt/mycrypt.h"

// and a few of the engine headers need to be outside the struct as well - primarily the memory debugger, which redefines new and delete, as well as all the classes and types it relies on. Some compilers also don't like assigned static constants inside of structs unless integers, so we put the floating point math constants outside the struct as well. meh.
#include "core/mutex.h"
#include "core/assert.h"
#include "core/base_type_declarations.h"
#include "core/base_type_traits.h"
#include "core/construct.h"
#include "core/memory_functions.h"
#include "math/mathConstants.h"

// The main cradle program struct!
struct cradle_test
{
    // include the rest of the core library
    #include "core/core.h"
    
    // instantiate a logger to stdout
    stdout_log_consumer _stdout_log;

    // include the notify_sockets library. notify_sockets provides an intermediate network protocol that lands somewhere between UDP and TCP: the "notify protocol". The notify protocol is an unguaranteed connection protocol atop UDP. Instead of guaranteeing packet delivery, the notify protocol guarantees "notification" -- ie. at some future point, the sender will be notified whether a prior packet send was delivered or not. The notify protocol is encrypted, and notify_sockets provides for "introduced" connections mediated by a third party, as well as NAT "punch" traversal.
    #include "notify_sockets/notify_sockets.h"
    
    // include the replicator library. replicator provides a high-level network stack built on the notify protocol. replicator's core functionality includes both remote procedure call (RPC) as well as partial object "most recent state" prioritization and replication.
    #include "replicator/replicator.h"

    // Core math functions and classes:
    #include "math/math.h"
    
    // Next, the "Cradle"!
    // The cradle classes provide core rendering and input layering services on top of SDL. This layering is necessary for two key reasons:
    
    // First, while SDL does a fantastic job of abstracting core hardware platform differences in a single clean API, it still has issues with different rendering options - GL ES2 for the web and mobile, OpenGL for computers, etc., which have varying approaches to shaders and vertex buffering. The CorePlatform class and singleton instanced as "P" (see engine/platform.h) aims to abstract all the various rendering APIs into a single clean interface for cradle apps.
    #include "vectrex/vectrex.h"
    #include "cradle/cradle.h"

    // Second, cradle add journaling functionality to provide recording and deterministic playback of application runs, which makes tracking down certain types of bugs much easier. SDL events received in SDL_AppEvent (declared at the end of this file) are passed into the CorePlatform object (P) and from there into the global journal object (G) defined in test_journal.h.
    #include "test_journal.h"
    
    // When the game is recording a journal, calls into G are recored into a file and then processed -- so if the app crashes (or unrecoverablly hangs in some system call), the final journal event can be replayed from the root.

    // When the game is playing back a journal, calls into G are ignored; instead, journal entries are read from the journal file and processed from the global struct's static idle method (see below).

    // the position structure demonstrates how custom networked types are declared and registered in cradle:
    struct position {
        // the unit_float template describes a floating point 0..1 value that uses the specified precision bits for network transmission.
        unit_float<12> x, y;
        
        // position::register_class is called from test_game's constructor in order to register the position class with the test_game's type_database. replicator's replicant_connection class uses the type database to automatically serialize/de-serialize object state data on the wire.
        static void register_class(type_database &_type_database)
        {
            core_type_begin_class(_type_database, position, empty_type, false)
            core_type_slot(_type_database, position, x, 0)
            core_type_slot(_type_database, position, y, 0)
            core_type_end_class(_type_database)
        }
    };
    
    // now include the test game classes proper:
    
    // the "player" class, which represents a connected client or an "AI" player in the world. players are represented as small squares. Clicking in the world will set the client player's target position to the clicked location. The player object is also as a connection's "scope object" - the object that determines which objects on the host side of the connection are "in scope" for that connection. The gray circle around the client's player represents the scope of objects that will be replicated to that client.
    #include "test_player.h"
    
    // The "building" class is an example of a static object in the world. All of the buildings in the world are considered "in scope" for all clients.
    #include "test_building.h"
    
    // The test_connection class shows how to subclass and utilize the replicator library's key functions of remote procedure calls (via rpc_connection) and networked object and object state replication (via replicant_connection).
    #include "test_connection.h"
    
    // The test_game class manages an example "game". Each game has a net_interface instance to manage connections, a type database to register and describe all the networked classes, and lists of game objects.
    #include "test_game.h"
    
    // The test_net_interface shows how to subclass and utilize the replicator library's net_interface class.
    #include "test_net_interface.h"
    
    // The CradleTestUserInterface class is a simple "one screen" UI for CradleTest. CradleTestUserInterface manages two "game" instances - one rendered in the left "pane" and one in the right. The UI routes mouse events in either pane to the affected game instance, and uses keyboard inputs to select which type of game (client or server) is in each pane.
    #include "test_ui.h"
    #include "test_gfx.h"

    /// current_time stores the current system time in order to track elapsed time between calls to idle()
    time current_time;
    
    /// cradle_test's static idle() method is called from the SDL3 wrapper functions. idle either processes the next journal entry if the app is playing back a journal, or calls into the global journal's idle method. If the journal is in recording mode, call() will log the function call in the journal prior to calling the method on the journal object.
    static void idle()
    {
        if(A.G.get_current_mode() == journal::playback)
        {
            A.G.process_next_entry();
        }
        else
        {
            time new_current = time::get_current();
            time delta = new_current - A.current_time;
            if(delta > 10)
            {
                A.current_time = new_current;
                uint32 delta_ms = delta.get_milliseconds();
                
                A.G.call(&cradle_test_journal::idle, delta_ms);
            }
        }
        os_sleep(0);
    }
    
    /// cradle_test's static init() method is called from the SDL3 wrapper functions. init() processes the command line in order to set up the global journal properly, initializes the CorePlatform object (window, rendering, input), and then calls into the global cradle_test_journal's init method.
    static bool init(int argc, char **argv)
    {
        // A journal instance can only journal one thread, so we set cradle_test_journal's global instance for the main thread here:
        A.G.set_current_on_thread();

        //log_enable(notify_socket, true);
        //log_enable(net_connection, true);
        //log_enable(notify_connection, true);
        
        // the cradle engine's logging functionality allows log message sets to be individually enabled by token.
        log_enable(cradle_test, true);

        if(argc > 1)
        {
            if(!strcmp(argv[1], "-jsave"))
            {
                A.G.start_record("cradle.jrn");
            }
            else if(!strcmp(argv[1], "-jplay"))
            {
                A.G.start_playback("cradle.jrn");
            }
        }
        A.current_time = time::get_current();

        if(!A.P.init("Cradle Test", false, &A.G))
        {
            logf(cradle_test, ("Render Init Fail."));
            return false;
        }
        logf(cradle_test, ("Render Init."));
        
        A.G.call(&cradle_test_journal::init);

       return true;
    }
    static void quit()
    {
        A.G.quit = true;
    }

    /// cradle_test's static process_event() method is called from the SDL3 wrapper functions. process_event just passes the event into the global cradle CorePlatform object. The CorePlatform object P translates SDL events into calls into its registered cradle_journal instance.
    static SDL_AppResult process_event(SDL_Event *event)
    {
        return A.P.process_event(event);
    }
} A;

// Now that we're back at global scope, bring in the implementations of the required SDL callbacks
#define CRADLE_GAME cradle_test
#include "cradle/sdl_callbacks.h"
