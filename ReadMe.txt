Welcome to the Cradle Engine!

Cradle is a game engine framework/library collection specifically developed for high-fidelity networked applications. Cradle is still in early development, driven by the creation of Athena Core, a new twin stick retro multiplayer team action game from Bad Habit Productions.

Cradle Engine Modules:

/core - core platform and utility classes - basic types, containers, allocators, and reflection
/math - math utility library for 3D graphics, largely derived from nml
/notify_sockets - low-level connected datagram network protocol with delivery notification
/replicator - mid-level networking architecture provides remote procedure call (RPC) and partial object state replication and prioritization 
/cradle - lightweight engine framework and platform abstraction for input and rendering atop SDL3 and audio sfx atop OpenAL
/test - simple networked test application
/master - example game master server for facilitating connections between game clients and servers
/libtomcrypt - basic cryptography functions used by notify_sockets
/kt - the kt programming language in development
/voice_compression - audio compression library

Getting Started:

Cradle uses cmake for compilation/configuration. Once you've got cmake installed, type the following in a shell window from the root of the Cradle Engine tree:

cmake -B build
cmake --build build

This should build the CradleTest example as well as the example master server and master test game.

To get a high-level picture of how a cradle app is structured and how the replicator networking system works, start by reading /test/cradle_test.cpp 

History and Licensing:

The Cradle Engine traces its roots to the Darkstar libraries developed at Dynamix, Inc. for the Starseige and Tribes series of computer games, and further evolved through the development of several generations of Torque Game Engine technologies at GarageGames. Elements of the Cradle Engine were first released as open source software under the GNU Public License (v2) as part of the Torque Network Library and then under the MIT license as part of the notify_sockets web networking proof-of-concept.

Each module of the Cradle Engine contains a "license.txt" file describing how that module's code is licensed. Summarized broadly, the low-level modules such as core, math, and notify_sockets are made available under the MIT open source license. Higher level components including /replicator, /cradle, CradleTest and the example master server are made available under version 2 of the GPL. Licensing inquiries regarding the use of those components for commercial closed-source applications can be directed to cradle@athenavcore.dev .
