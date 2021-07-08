# D3Foxed

<div align="center">

![](resources/icon.png)

[About](#about) | 
[Changes](#changes) | 
[Roadmap](#roadmap) |
[Installation](#installation) |
[FAQ](#faq) |
[Building](#building-fhdoom)

</div>

## About

D3Foxed is a modified fork of the [fhDOOM](https://github.com/eXistence/fhDOOM) engine, which itself was a modified fork of the DOOM3 engine. Seeing as fhDOOM doesn't appear to be being developed anymore and I don't care much for retaining compatibility with Doom 3, I decided to spin my fork into it's own thing.

Because I don't have time to dedicate 110% of my time to this project, I would recommend perhaps looking [around a bit more](#similar-projects) before choosing this fork, as there are other forks that excell in areas where this fork is unlikely to focus on.

## Changes

* CMake build system
  * Support for x86 and x64 compilation under Windows with Visual Studio<br>(you need to have the MFC Multibyte Addon installed)
* Uses GLEW for OpenGL extension handling (replaced all qgl\* calls by normal gl\* calls)
* OpenGL 3.3 core profile, using GLSL for shaders
* Shadow mapping support (PCSS)
  * Shadow mapping and Stencil Shadows can be freely mixed
  * Parallel/directional lights use cascade shadow mapping
  * modified 'dmap' compiler to generate optimized occlusion geometry for shadow map rendering (soft shadows)
* Fixed VRAM detection for modern GPUs
* occlusion geometry is stored in a separate *.ocl file (*.map and *.proc are still the same)
* ocl file optional, maps without ocl file work just fine (at the expense of performance)
* dmap option 'exportObj' will export occlusion geometry to obj files
* D3Foxed includes ocl files for all maps of the original game  
* Soft particles
* Parallax occlusion mapping (expects height data in the alpha channel of the specular map)
* Added support for Qt based tools
  * optional (can be excluded from the build, so you are not forced to install Qt to build D3Foxed)
  * Implemented some basic helpers
    * RenderWidget, so the engine can render easily to the GUI
    * several input widgets for colors, numbers and vectors
    * utility functions to convert between Qt and id types (Strings, Colors, etc.)
  * re-implemented the light editor as an example (+some improvements)
    * useable ingame and from editor/radiant
    * cleaned up layout
    * removed unused stuff
    * added advanced shadow options (related to shadow mapping)
    * show additional material properties (eg the file where it is defined)
* Several smaller changes
  * Detect base directory automatically by walking up current directory path
  * Added new r_mode's for HD/widescreen resolutions (r_mode also sets the aspect ratio)
  * Set font size of console via con_fontScale
  * Set size of console via con_size
  * Minor fixes and cleanups in editor code (e.g. fixed auto save, removed some unused options)
  * Renamed executable and game dll (the intention was to have fhDOOM installed next to the original doom binaries. Not sure if that still works)
  * Moved maya import stuff from dll to a stand-alone command line tool (maya2md5)
  * Compile most 3rd party stuff as separate libraries
  * Fixed tons of warnings
  * image_usePrecompressedTextures is 0 by default, so HD texture packs (like Wulfen) can be used more easily.
  * Contains all game assets from latest official patch 1.31

See also [changes.md](changes.md) for complete history of all changes.

## Roadmap

*todo*

## Installation

* ~~Download Binaries here: <http://www.facinghell.com/fhdoom/fhDOOM-1.5.2-1414.zip>~~
* The recommended way to install D3Foxed is to unpack D3Foxed into its own directory and copy only those files from the original Doom3 that are needed:
  * Extract zip to a location of your choice (e.g. c:\games)
  * Copy 5 files from the original DOOM3 (CD or Steam) to base directory of D3Foxed (e.g. c:\games\D3Foxed\base):
    * pak000.pk4
    * pak001.pk4
    * pak002.pk4
    * pak003.pk4
    * pak004.pk4
  * Start game by clicking on fhDOOM.exe
  * Don't forget to change the game's resolution (D3Foxed does not yet select your native resolution automatically).
* Alternatively you can unpack the downloaded zip directly into an existing  Doom3 installation.
  * As mentioned before, D3Foxed contains the last official patch (1.31), so some files (pak00[5-8].pk4 and Default.cfg) will be overwritten if this patch is already installed (e.g. you installed patch 1.31 manually or installed the game via Steam). Skipping these files should be fine as well.
  * D3Foxed is usually not tested in combination with other mods, so if you have other stuff installed all bets are off.
  * If you run into any issues, please try the clean and recommended installation as described above.
  * Future releases will very likely include a variant without the additional files from patch 1.31 to make this a bit easier.

## FAQ

### Q1. Are Mods supported?

Yes!
Mod support is pretty much the same as in vanilla Doom3 1.3.1 (only difference: game dll is named 'fhGame-x86' instead of 'gamex86').

### Q2. Is D3Foxed compatible with mods for vanilla Doom3 1.3.1?

It depends.
Pure content mods containing only textures, scripts, maps and such things should work just fine (only exception: custom ARB2 shaders won't work).
Mods that come with a compiled game dll (gamex86.dll on windows, gamex86.so on linux) won't work. Those game dlls must be recompiled for D3Foxed.

### Q3. Can i use SikkMod with D3Foxed?

No, because SikkMod is based on ARB2 shaders (see Q2).

### Q4. Can i use HD texture mods (Wulfen, Monoxead, etc.) with D3Foxed?

Yes (see Q2).

### Q5. How do i (re)compile a mods game dll for D3Foxed?

Unfortunately that's currently not that easy. You have basically two options:

 1. If you don't care about existing installations of officially released D3Foxed binaries, you could just clone the latest D3Foxed version from github. You apply your changes to the game code and distribute the whole thing (executable and game dlls).
 2. You clone the D3Foxed version from github that matches the latest official binary release. You apply your changes to the game code and distribute only the game dll to the user (the user must have D3Foxed installed). Pretty much like vanilla Doom3 1.3.1.

Both options are far from being good, but since i am working for the most part on the engine itself and not on the game code, i never felt the need to improve this. If you want to make a mod and need to compile your own game dll, let me know. If there is enough interest in better support for this, i will set up and release some kind of SDK to easily compile only the game code.

### Q6. Does multiplayer work?

I suppose it does... but i don't know for sure. Feel free to test it out and share your findings :)

## cvars

fhDOOM added and changed a couple of cvars. This list of cvars might be interesting:

* r_mode <0..15>: set resolution and aspect ratio. Resolution can also be set via GUI.
  * 0..8: (unchanged, 4:3)
  * 9: 1280x720 (16:9)
  * 10: 1366x768 (16:9)
  * 11: 1440x900 (16:10)
  * 12: 1600x900 (16:9)
  * 13: 1680x1050 (16:10)
  * 14: 1920x1080 (16:9)
  * 15: 1920x1200 (16:10)
* r_shadows <0|1|2>: default shadow mode, can also be selected via GUI.
  * 0: force shadows off
  * 1: use stencil shadows
  * 2: use shadow mapping
* r_specularScale <float>: scale specularity globally
* r_specularExp <float>: sharpness of specular reflections
* r_shading <0|1>: shading model
  * 0: Blinn-Phong
  * 1: Phong
* r_pomEnabled <0|1>: enable/disable parallax occlusion mapping, requires special specular maps with height information in alpha channel
* r_pomMaxHeight <float>: adjust max displacement
* r_smBrightness <float>: adjust default brightness of soft shadows
* r_smSoftness <float>: scale default softness of soft shadows (higher values will lead to artifacts)
* r_smPolyOffsetFactor <float>: depth-offset of shadow maps (increasing this will fix artifacts from high softness, but will also increase light bleeding)
* r_smForceLod <-1|0|1|2>: set lod (quality/size) of shadow maps for all lights
  * -1: choose dynamically based on light size and distance from viewer
  * 0: high quality (1024x1024)
  * 1: mid quality (512x512)
  * 2: low quality (256x256)
* r_smLodBias <0|1|2>: apply lod bias to lights to reduce quality without forcing all lights to the same lod.
* r_smUseStaticOcclusion <0|1>: enable/disable static occlusion geometry from ocl files
* r_glCoreProfile <0|1>: enable/disable opengl core profile (requires full restart of the game)
* r_glDebugOutput <0|1|2>: OpenGL debug messages, requires core profile
  * 0: No debug message
  * 1: async debug messages
  * 2: sync debug messages
* con_fontScale <float>: scale font size in console
* con_size <float>: scale console (not immediately visible, close and re-open the console to see it)
* com_showFPS <0|1|2|3>: 0=Off, 1=FPS, 2=ms, 3=FPS+ms
* com_showBackendStats <0|1>: show various backend perf counters
* g_projectileLightLodBias <0|1|2>: reduce shadow quality from projectile lights, usually not noticable
* g_muzzleFlashLightLodBias <0|1|2>: reduce shadow quality from muzzle flashes, usually not noticable

## Notes  

* The maps of the original game were not designed with shadow mapping in mind. I tried to find sensible default shadow parameters, but those parameters are not the perfect fit in every case, so if you look closely enough you will notice a few glitches here and there
  * light bleeding
  * low-res/blocky shadows
* Parallax Occlusion Mapping is disabled by default, because it looks weird and wrong in a lot of places.
* Shaders from RoE expansion pack not rewritten yet
* There was a time, where you could switch from core profile to compatibility profile (r_glCoreProfile). In compatibility profile,
    you were still able to use ARB2 shaders for special effects (like heat haze). This feature stopped working for some reason.
    Not sure if i will ever investigate this issue, i am fine with core profile.  
* The font size of the console is scalable (via con_fontScale), but the width of each line is still fixed (will be fixed later).
* Doom3's editor code is still a giant mess  
* I already had a linux version up and running, but its not tested very well. I am currently focussed on windows.
* I added support for Qt based tools purely out of curiosity. I have currently no plans to port more tools to Qt.  
* Other stuff on my ToDo list (no particular order):
  * Some ideas to improve render performance (related to shadow mapping). The engine runs pretty decent on my (fairly dated) machine, not sure if its worth the effort. Might change with more advanced rendering features.
  * Expose different sampling techniques (currently hard coded)
  * HDR rendering/Bloom
  * Tesselation/Displacement Mapping
  * Multi threading
  * Hardware/GPU skinning
  * Port deprecated MBCS MFC stuff to unicode, so you don't need that MBCS Addon für VS2013.
  * Doom3 contains a very early and simple form of Megatexturing. Might be interesting to re-enable that for modern OpenGL and GLSL.
  * 64bit support would simplify the build process on linux (totally irrelevant on windows, the actual game doesn't really need that much memory...)
    * Get rid of ASM code. Rewrite with compiler intrinsics? Not sure if its worth the effort... the generic C/C++ implementation might be sufficient (needs some testing)
    * Get rid of EAX stuff
  * Look into Doom3's texture compression and modern alternatives. Does the wulfen texture pack (and others) really need to be that big? How does it effect rendering performance? Pretty sure it could improve loading times...
  * Render everything to an offscreen buffer
    * simple super sampling
    * fast switching between different r_modes
    * simplifies messy fullscreen switch
* i am pretty sure i forgot a lot of things, so you might discover more things that are pretty hacky or not working at all ;)

## Building D3Foxed

Dependencies:

* Visual Studio 2019. Community versions are just fine.
* cmake 3.2 (make sure cmake.exe is in your PATH)
* Windows 8.1 SDK
* optional: Qt 5.4 (32bit)
* optional: Maya 2011 SDK (32bit)

Setup:

* Clone repository
* Just as with the regular installation, copy these files from the original DOOM3 game into the base directory of your local repository (git will ignore them):
  * pak000.pk4
  * pak001.pk4
  * pak002.pk4
  * pak003.pk4
  * pak004.pk4
* Run `cmake_msvc201x.cmd` to generate
  * if you want to build with Qt tools enabled, there are two ways to tell cmake where Qt is installed:
    * run `cmake_msvc201x.cmd -DQTDIR=<QTDIR>`
    * or set an env var in windows: `QT_MSVC201x_X86=<QTDIR>`
    * `<QTDIR>` must point to your Qt installation so that `<QTDIR>/bin` contains all the Qt binaries (dll and exe files).
    * Please keep in mind that D3Foxed needs a 32bit build of Qt
* Compile D3Foxed from Visual Studio
  * Debug and Run D3Foxed directly from Visual Studio
  * D3Foxed will pick up the base directory in your repository automatically
* The zip file from a D3Foxed release contains a pk4 file `pak100fhdoom.pk4`. This file contains all the new shaders and required asset files.
    You may have noticed that this file cannot be found in the git repository. For easier editing all asset files are loosely placed in the base
    directory of the repository. You can directly edit these files or add new ones and D3Foxed will use them. These files are automatically packed up into  `pak100fhdoom.pk4` when you generate a distributable zip file (e.g. for release), see below for details.
* Generating distributable zip files: There are three special build targets that generate distributable zip files:
  * `dist`: generates a zip file that contains D3Foxed and all required files from the official 1.31 patch (this is what is usually released)
  * `dist_nopatch`: same as `dist` but without the files from the 1.31 patch
  * `sdk`: generates a SDK to build only a game dll for D3Foxed (this is currently not used, not sure if its still working)

### Similar Projects

  There exist other forks of the DOOM3 engine and even id software released a modernized version of DOOM3 and its engine as "DOOM 3 - BFG Edition".

* [fhDOOM](https://github.com/eXistence/fhDOOM) (what this fork is based on)
* [DOOM-3-BFG](https://github.com/id-Software/DOOM-3-BFG)
* [dhewm3](https://github.com/dhewm/dhewm3) (makes DOOM3 available on many platforms via SDL)
* [RBDOOM-3-BFG](https://github.com/RobertBeckebans/RBDOOM-3-BFG) (based on DOOM-3-BFG)
* [OpenTechEngine](https://github.com/OpenTechEngine/OpenTechBFG) (creation of standalone games, based on RBDOOM-3-BFG)
* [Storm Engine 2](https://github.com/motorsep/StormEngine2) (creation of standalone games, based on RBDOOM-3-BFG)
* [The Dark Mod](http://www.thedarkmod.com/) (total conversion, not sure if it can still run the original DOOM3 game)
* <https://github.com/raynorpat/Doom3> Attempt to port rendersystem from DOOM-3-BFG to classic DOOM3
