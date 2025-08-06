![TerrariaDS Logo](media/logo.png)
# TerrariaDS
A terraria remake (not port) for the Nintendo DS made by AzizBgBoss, started on the 26th of July 2025.<br>

I'm a 17 y/o that's obsessed with programming, I'm also into Terraria! I love the freedom you get in the game and the endless possibilities it has to offer. Another thing I love is the Nintendo DS, it's a great console and I wondered if I can fuse the 3 thing I love together, so I programmed Terraria into the DS! TerrariaDS!<br>
<br>
This project is a work in progress and is not affiliated with the original Terraria game. It aims to replicate the core mechanics and gameplay of Terraria on the Nintendo DS.
It aims at least to somewhat replicate the 3DS version of Terraria, but with some features from the PC version, and some sacrifices to fit the DS hardware limitations.<br>
<br>
I'm trying as hard as possible to stay loyal to the original game design, but there are some limitations due to the DS hardware and the fact that this is a remake, not a port.<br>
Also please note: ***For now***, I'm prioritizing functionality over content. I'm trying to perfect the core mechanics of the game before adding the Terraria content (Items, tiles, structures...), because without functionality, you wouldn't truly enjoy the content.<br>
<br>
I'm very open to contributions and suggestions, so if you want to help, feel free to open a pull request or an issue.<br>
<br>
If you want to try the latest changes, you can compile the project yourself, or wait for a new release to be published.<br>

### Disclaimer
This project is not affiliated with Re-Logic or Terraria in any way. It is a fan-made project and is not intended for commercial use. All assets used in this project are use for educational and entertainment purposes only. If you're a party of Re-Logic and owner of any assets used in this project and want them removed, please contact me.

## Installation:
You can either compile the game yourself (for nightly releases) or wait for stable releases in the Releases page. Please check the Tested devices part before starting.

### Tested devices:
| System                           | Functionality             | Cons                  | Notes                                                                                                       |
|----------------------------------|---------------------------|-----------------------|-------------------------------------------------------------------------------------------------------------|
| Nintendo DSi (TM++)              | Excellent                 | None                  | Make sure to set launcher as Default                                                                 |
| Nintendo 3DS (TM++)              | Not tested yet            | Not tested yet        | Should work fine as long as you're using TW++ since it basically becomes a DSi at that point               |
| Flashcarts (DS/DS Lite/DSi)      | Not tested yet            | Not tested yet        | You should probably patch the .nds rom with the proper DLDI patch depending on your flashcart type (e.g., R4) |
| NO$GBA (Emulator)                | Medium                    | No ability to save + Choppy sound | The emulator doesn't have DLDI support.                                         |
| desMuMe (Emulator)               | Non-functional            | Game doesn't even start |                                                                             |
| melonDS (Emulator)               | Non-functional            | No nitroFS              | The emulator *has* DLDI support but it doesn't work with this game, maybe they'll fix it in a future update. But for some reason NitroFS won't work with melonDS. |

*The game should probably work fine on all platforms, the only thing that can vary is speed/graphical glitches (very unlikely) and ability to save and load. More platforms will be tested soon.*

## Gameplay:
- A: Jump
- Left/Right: Move
- Up/Down: Zoom in/out
- X/Y: Switch items
- R: Open inventory
- Start: Save map
- Select: Load map
- A+B+X+Y: Toggle debug mode

## Changelog and Features
- Don't get your hopes really up, but with time, I'm sure the game will feel much more like terraria.
### Version 0.0alpha (Working on it):
Initial commit with basic project structure.
- Basic tile rendering and scrolling. ***(done)***
- Touch input handling for world interaction and inventory interaction. ***(done)***
- Map generation (terrain, caves, trees and mushrooms). ***(done)***
- Map saving and loading functionality. ***(done. you can use START to save, and SELECT to load for now)***
- Camera zooming. ***(done)***
- Basic crafting system. ***(done)***
- Add music. ***(done)***
- More testing and bug fixing needed, game proof-of-concept is going to be released soon.

### Version 0.1 (Plans):
- Add special tiles (tiles that need interaction and are not 1:1, like doors, crafting benches, chests...)
- Add health.
- Add menus.
- Add ability to change button mappings (since many DS'es (including mine) have malfunctioning shoulder buttons because of dust accumulation under them over the years).
- Add entities (NPCs, enemies, etc.) to the game world.
- Add ores and more items to the game.
- Add more biomes (desert, jungle, etc.).
- Add crimson/corruption.
- Support for maps with more height (limit is 64 for now, that means chunking should happen in both X and Y axes).
- Add breaking animation.
- Organize my messy code and split it to many files, won't change anything but will make it easier to add features.

## Planed Features for the future:
- Multiplayer support.
- Ability to change the map size.
- Skins (or changeable player styles).
- Add mana and magic items.
- Add bosses.
- Extra content and features for the DSi.

Again, I'm open to suggestions and contributions, so if you have any ideas or want to help, feel free to open a pull request or an issue.

## Compile it yourself
To compile this project, you need to have devkitPro installed. You can find instructions on how to install it on the [devkitPro website](https://devkitpro.org/wiki/Getting_Started) (Note: devkitPro NDS compilation only works on Windows as far as I know, or x86_64 architectures in general), I also always hear about some BlocksDS, I think that causes errors.

Once you have devkitPro installed, you can clone this repository by running:
```bash
git clone https://github.com/AzizBgBoss/TerrariaDS.git
```

Then, navigate to the project directory:
```bash
cd TerrariaDS
```

For good measure, clean the place first:
```bash
make clean
```

Now, you can compile the project by running:
```bash
make
```
This will create a `TerrariaDS.nds` file in the root directory of the project.

(Note: if you're not using the Msys2 shell, you may need to define the `DEVKITPRO` environment variable to point to your devkitPro installation directory, e.g. `export DEVKITPRO=/mnt/c/devkitPro` and `export DEVKITARM=/mnt/c/devkitPro/devkitARM`)

## Credits
- AzizBgBoss for the DS remake.
- DS(i) Mode Customization! Discord server for being very helpful and provide me with resources to understand how the DS works.
- The Terraria Wiki for being the best source to understand how blocks and all Terraria mechanics work.
- Re-Logic for making such a great game.