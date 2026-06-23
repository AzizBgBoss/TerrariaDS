![TerrariaDS Logo](media/logo.png)<br>
[![Downloads](https://img.shields.io/github/downloads/AzizBgBoss/TerrariaDS/total.svg?label=Download)](https://github.com/AzizBgBoss/TerrariaDS/releases) ***(this is the total downloads of all versions, it's not an accurate way to count downloads but it's a good indicator of how popular the game is. Check below for each version's downloads)***
# TerrariaDS
A terraria remake (not port) for the Nintendo DS made by AzizBgBoss, started on the 26th of July 2025.<br>

I'm a 17 y/o that's obsessed with programming, I'm also into Terraria! I love the freedom you get in the game and the endless possibilities it has to offer. Another thing I love is the Nintendo DS, it's a great console and I wondered if I can fuse the 3 thing I love together, so I programmed Terraria into the DS! TerrariaDS! ***(update: I'm now 18 lol)***<br>
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
<br>
You can join my [Discord server](https://discord.gg/zfMwPhvDc4) for more updates and discussions.<br>

### Disclaimer
This project is not affiliated with Re-Logic or Terraria in any way. It is a fan-made project and is not intended for commercial use. All assets used in this project are use for educational and entertainment purposes only. If you're a party of Re-Logic and owner of any assets used in this project and want them removed, please contact me.

### Notes:
- v0.2 is now released! Enjoy!
- I'm using the Terraria Wiki as a reference for the game, if you've found any mistakes, please let me know.
- If you like this homebrew game, consider starring the repo or [donating](https://github.com/AzizBgBoss/TerrariaDS#donate), it helps to show your appreciation and support and it motivates me a lot :)

## Installation:
You can either compile the game yourself (for nightly releases) or check for stable releases in the Releases page. Please check the Tested devices part before starting.

### Tested devices:
| System                           | Functionality             | Cons                  | Notes                                                                                                       |
|----------------------------------|---------------------------|-----------------------|-------------------------------------------------------------------------------------------------------------|
| Nintendo DSi (XL) (TM++)         | Excellent                 | None                  | Make sure to set launcher as Default                                                                 |
| Nintendo 3DS (New) (XL) (TM++)   | Excellent                 | None                  | Should work fine as long as you're using TW++ since it basically becomes a DSi at that point. Also you can just play Terraria 3DS lol.|
| Flashcarts (DS/DS Lite/DSi)      | Not tested yet            | Not tested yet        | You should probably patch the .nds rom with the proper DLDI patch depending on your flashcart type (e.g., R4) |
| NO$GBA (Windows Emulator)        | Medium                    | No ability to save + Choppy sound | The emulator doesn't have DLDI support.                                         |
| desMuMe (Windows Emulator)       | Non-functional            | Game doesn't even start |                                                                             |
| melonDS (Windows Emulator)       | Non-functional            | No nitroFS              | The emulator *has* DLDI support but it doesn't work with this game, maybe they'll fix it in a future update. But for some reason NitroFS won't work with melonDS. |
| melonDS (Special fork)           | Excellent                 | None                  | This is a fork of the original melonDS with NitroFS and FAT support added. You can find it [here](https://github.com/AzizBgBoss/TerrariaDS/raw/refs/heads/main/extras/melonDS.exe). I got it somewhere from the DS(i) Mode Customization! Discord server. Don't worry, it shouldn't contain any malware. But as soon as I find the original link, I'll put it here. | 
*More platforms will be tested soon.*

## Download the latest version (QR code scannable with DSi Downloader)

![QR](media/frame.png)

## Gameplay:
- A: Jump
- Left/Right: Move
- Up/Down: Zoom in/out
- X/Y: Switch items
- R: Open inventory
- Start: Pause menu
- A+B+Y+X: Debug menu (you may use this to help you if you ever get stuck, at the cost of adding "the cheater" to your character name (don't worry it keeps a copy of your old character))
- Tap/Hold a block: Interact with/break the block (with the appropriate item of course)
- Tap enemies to damage them

## Changelog and Features
Don't get your hopes really up, but with time, I'm sure the game will feel much more like terraria.

### Version 0.0alpha ([![Download](https://img.shields.io/github/downloads/AzizBgBoss/TerrariaDS/0.0alpha/total.svg?label=Download)](https://github.com/AzizBgBoss/TerrariaDS/releases/tag/0.0alpha)):
Initial commit with basic project structure.
- Basic tile rendering and scrolling. ***(done)***
- Touch input handling for world interaction and inventory interaction. ***(done)***
- Map generation (terrain, caves, trees and mushrooms). ***(done)***
- Map saving and loading functionality. ***(done. you can use START to save, and SELECT to load for now)***
- Camera zooming. ***(done)***
- Basic crafting system. ***(done)***
- Add music. ***(done)***

### Version 0.1 ([![Download](https://img.shields.io/github/downloads/AzizBgBoss/TerrariaDS/0.1/total.svg?label=Download)](https://github.com/AzizBgBoss/TerrariaDS/releases/tag/0.1)):
- Ability to save/load different maps. ***(done: you can only see 10 maps for now)***
- Add special tiles (tiles that need interaction and are not 1:1, like doors, crafting benches, chests...) ***(done for now: Doors)***
- Add health. ***(done)***
- Add menus. ***(done for now: Pause menu, debug menu (they don't look the best but do the job for now))***
- Add entities (NPCs, enemies, etc.) to the game world. ***(done for now: Green Slimes)***
- Add attacking and damage mechanics for other entities like the stinky winky green slime. ***(done for now: Player can damage an entity thats directly touched with a weapon)***
- Add ores. ***(done for now: Copper, Tin)***
- Fix some screen jittering issues. ***(done: you shouldn't read nitro files in the loop every frame, it makes the scroll changement miss VBlank)***
- Fix sprites being offset from their world coordinates. ***(yeah I don't even know how I made a mess that kinda works and now I don't understand it anymore if you can please help me with it)***
- Add simple day/night cycle (just darken the rear background and start spawning enemies at night). ***(done)***
- Add sound effects. ***(done)***

### Version 0.2 ([![Download](https://img.shields.io/github/downloads/AzizBgBoss/TerrariaDS/0.2/total.svg?label=Download)](https://github.com/AzizBgBoss/TerrariaDS/releases/tag/0.2)):
- Add more music and intros. ***(done: I didn't add much intros because I'M NOT GONNA MIGRATE TO NITROFS AGAIN AGHHH)***
- Add ability to delete worlds and characters. ***(done)***
- Add acorns to keep the world sustainable if needed. ***(~~done~~ thanks to Zenic on discord that told me I forgot to implement acorns dropping from trees, you can still get them from the debug menu (use another character to place them in your world cuz it will be named "the cheater"))***
- Add swinging animation. ***(done)***
- Add characters with different styles. ***(done)***
- Show health as hearts. ***(done)***
- Add life crystals. ***(done)***
- Add particles. ***(done)***
- Add more sound effects. ***(done: I didn't really add that much)***
- ~~Fix touch misalignment when close to world edges at 2x scale.~~ ***(can't do it: I don't know why, prolly got something to do with bgSetCenter, if you're close to the edges just keep the scale 1x)***
- Add chests. ***(done: limited to 32 chests for now)***
- Add more tools. ***(done: Iron and Gold pickaxes, axes, hammers, and long swords, and a wooden hammer lol)***
- Implement dynamic crafting recipes (based on nearby stations). ***(done)***
- Add crafting stations. ***(done for now: Workbench, Furnace, Anvil)***
- Probably converting all tile handling data to a struct (even special tiles). ***(done: this is great news, now I can add a huge variety of special tiles with the same function easily)***
- Add more crafting recipes. ***(done: now implemented pages)***
- Add more biomes. ***(done: desert, snow)***
- Implement entity spawning based on biome and layer ***(done: it was a bit complicated, but now entities spawn based on time, biome, and layer (surface/underground))***
- Better and more efficient world generation. ***(done: it's a bit faster now)***
- Add currency. ***(done)***
- Add X axis velocity ***(done: this allowed to implement knockback and fix some collision bugs)***
- Probably add a bit more entities. ***(done for now: Eyes, Slimes, Bunny, Zombies)***
- Organize my messy code and split it to many files, won't change anything but will make it easier to add features. ***(done)***
- Support for maps with more height (limit is 64 for now, that means chunking should happen in both X and Y axes). ***(done, worlds come in 4 sizes, with backwards compatibility with v0.1 maps)***
- Move all assets back to ROM since my dumbass a year ago thought moving everything to NitroFS will free up RAM but it's already `const` so no need... ***(done: it's now even neater than before)***
- Fix misaligned sprites when zooming in/out ***(done: yeah I still don't understand why this happens, if you can please help me with it. But I just added more math to counter the offset)***

## Planned Features for the future:
- I'm *thinking* about making a script that scraps all tile/entity data from the wiki but it's a bit complicated.
- Use more pointers for cleaner code.
- Add ability to change button mappings (since many DS'es (including mine) have malfunctioning shoulder buttons because of dust accumulation under them over the years).
- Add crimson/corruption.
- Add improved lighting.
- Multiplayer support (based on my experience, this will be a fucking nightmare).
- Changeable style colors.
- Add mana and magic items.
- Add bosses.
- Unique content for the DS version to make it stand out a bit.
- Extra content and features for the DSi.
- Maybe migrate to BlocksDS for better performance and more features (it shouldn't be that hard I think).

Again, I'm open to suggestions and contributions, so if you have any ideas or want to help, feel free to open a pull request or an issue.

## Compile it yourself
To compile this project, you need to have devkitPro installed. You can find instructions on how to install it on the [devkitPro website](https://devkitpro.org/wiki/Getting_Started) (Note: devkitPro NDS compilation only works on Windows as far as I know, or x86_64 architectures in general).

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
- Shoutout to the Testers: Me (DSi), Vinii (New 3DS XL). 

## Donate:
This game is totally free and if you enjoy this project and would like to support its development, any donation, no matter how small, would be greatly appreciated. I'm a teenager and I really don't have much time with school and other stuff, so your generosity helps motivate me and dedicate more time to continue working on TerrariaDS and bring new features to life. Thank you for considering a contribution!
- USDT (TRX20): TVq9gPQP2xTybbA2QtWrXBuQgvxZSVdiXm
- USDT (ETH): 0x63FCB723F9EaA3100b466f229067227EfA461Bf7

## Screenshots
![Screenshot 1](media/screenshot%20(1).png)
![Screenshot 2](media/screenshot%20(2).png)
![Screenshot 3](media/screenshot%20(3).png)
![Screenshot 4](media/screenshot%20(4).png)
![Screenshot 5](media/screenshot%20(5).png)
![Screenshot 6](media/screenshot%20(6).png)
