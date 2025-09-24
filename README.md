# ZoneTool
A fastfile dumper for various old generation Call of Duty titles. 

This tool <b>only supports</b> <i>Call of Duty 4: Modern Warfare (2007)</i> to <i>Call of Duty: Modern Warfare 3 (2011).</i> This fork <b>is only for porting assets from older Call of Duty games to new ones</b>, which is handled and compiled by [Joelrau's x64-zt](https://github.com/Joelrau/x64-zt).

⚠️ **This fork exists only for asset porting to newer Call of Duty games starting with Ghosts**, and compilation is handled by [Joelrau’s x64-zt](https://github.com/Joelrau/x64-zt) using the assets dumped with this fork. 

## Folder structure
Call of Duty\
| - zonetool\
| - zonetool.exe\
| - zonetool.dll 

## How to use
1. Download the correct ZoneTool `.exe` for your game from [ZoneTool Binaries](https://github.com/ZoneTool/zonetool-binaries).  
2. Drop the `zonetool_<game>.exe` directly into your game folder. 
3. [Download the latest ZoneTool DLL here](https://github.com/Joelrau/zonetool/releases/tag/latest).  
4. Place the `zonetool.dll` into your game directory.  
> **COD4 users:** rename `zonetool.dll` → `zoneiw3.dll` before launching `zonetool_<game>.exe`. 
5. Run `zonetool_<game>.exe`.  

## Commands
``loadzone <zonename>`` 
> loads the specified zone into memory.

``dumpzone <zonename> <?target>`` 
> dumps all assets from the specified zone. 
<br>target is `H1` by default if not found, but can be `IW6`, `S1`, `H1`, `IW7`).
<br><i>(to dump assets for H2, dump to H1 first, use [x64-zt](https://github.com/Joelrau/x64-zt), then convert H1 → H2)</i>

## Unsupported assets
Every asset type but the following can be linked with ZoneTool.

| Asset Type  | IW4 | IW5 |
|-------------|-----|-----|
| MenuFile    | ❌    | ❌    |
| Menu        | ❌    | ❌    |
| ImpactFx    | ❌    | ❌    |
| Vehicle     | ❌    | ❌    |
| AddonMapEnts | ❌    | ❌    |

## Credits
Special thank you to everyone included for contributing to the development and research of ZoneTool:

**Laupetin, NTAuthority, momo5502, TheApadayo, localhost, X3RX35, homura, Sofika, Gamecheat13, Joelrau, alice, mjkzy**

## Discord
If you need any help with zonetool or x64-zt, [feel free to join the Aurora Discord server](https://discord.com/invite/RzzXu5EVnh) and talk to the modding community.
