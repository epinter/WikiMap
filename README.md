# ***Wiki & Map Integration for Skyrim***
[![C++20](https://img.shields.io/static/v1?label=standard&message=C%2B%2B20&color=blue&logo=c%2B%2B&&logoColor=white&style=flat)](https://en.cppreference.com/w/cpp/compiler_support)
[![Latest Release](https://img.shields.io/github/release/epinter/wikimap.svg)](https://github.com/epinter/wikimap/releases/latest)
[![Downloads](https://img.shields.io/github/downloads/epinter/wikimap/total.svg)](https://github.com/epinter/wikimap/releases/latest)
[![Release Date](https://img.shields.io/github/release-date/epinter/wikimap.svg)](https://github.com/epinter/wikimap/releases/latest)
[![License](https://img.shields.io/github/license/epinter/wikimap.svg)](https://github.com/epinter/wikimap/blob/main/LICENSE)
[![Site](https://img.shields.io/static/v1?label=site&message=NexusMods&color=blue)](https://www.nexusmods.com/skyrimspecialedition/mods/121342)

A shortcut to online map and information about quests and locations, with support for Steam overlay.

## ***Runtime requirements***

- [Skyrim Script Extender (SKSE)](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
- [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604)
- [Latest VC++ Redist](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## ***Build requirements***

- [CMake](https://cmake.org/)
- [vcpkg](https://vcpkg.io/en/)
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/community/)
- [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG)

## **How to use**

This mod works when Map or Quests menus are open, nothing happens outside these menus.
To view information about a quest, highlight the quest using mouse and press the configured key. Steam overlay or default browser should open.
For the map, position the cursor over a map marker and press the shortcut. If you press the key when the cursor is not over a map marker, the online map will open displaying your current position.
Steam overlay support is enabled by default. If steam is not detected Windows API will be used to open the URL, then you will see your default browser over the game. There's no way to configure default browser. ***This mod doesn't execute any program, it only requests to open an http or https url, Steam or Windows handle it.***

This mod is pre-configured to use uesp.net, but can be used with any wiki (like elderscrolls.fandom.com), see configuration.

## ***Configuration***
The WikiMap.ini has the following options:

- Debug: Send more information to logs.
- UpdateBottomBar: Set to false to disable menu changes.
- UrlSelected: Url used when mouse is over a map marker.
- UrlInterior: Url used when player location is interior.
- UrlQuest: Url used for quests.
- Worldspaces: Urls used for exterior (map), one for each worldspace, Tamriel and Solstheim are preconfigured. If you have mods that adds worlds to the game and you know a web site with an online map for that mod, add the url to the ini.

Other options like key shortcut are configurable using MCM Helper.

## ***Building***

In `Developer Command Prompt for VS 2022` or `Developer PowerShell for VS 2022`, run:

~~~
git clone https://github.com/epinter/wikimap.git
cd wikimap
~~~

then

~~~
.\cmake\build.ps1
~~~

or

~~~
.\cmake\build.ps1 -buildPreset ALL-relwithdebinfo
~~~

or

~~~
.\cmake\build.ps1 -buildPreset ALL-debug
~~~

or

~~~
cmake -B build -S . --preset ALL --fresh
cmake --build build --preset ALL-release
~~~

Then get the .dll in build/Release, or the .zip (ready to install using mod manager) in build.