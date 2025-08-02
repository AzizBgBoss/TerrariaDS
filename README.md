![TerrariaDS Logo](media/logo.png)
# TerrariaDS
A terraria remake (not port) for the Nintendo DS made by AzizBgBoss.

This project is a work in progress and is not affiliated with the original Terraria game. It aims to replicate the core mechanics and gameplay of Terraria on the Nintendo DS.
It aims at least to somewhat replicate the 3DS version of Terraria, but with some features from the PC version, and some sacrifices to fit the DS hardware limitations.

I'm trying as hard as possible to stay loyal to the original game design, but there are some limitations due to the DS hardware and the fact that this is a remake, not a port.

I'm very open to contributions and suggestions, so if you want to help, feel free to open a pull request or an issue.

If you want to try the latest changes, you can compile the project yourself, or wait for a new release to be published.

### Disclaimer
This project is not affiliated with Re-Logic or Terraria in any way. It is a fan-made project and is not intended for commercial use. All assets used in this project are use for educational and entertainment purposes only. If you're a party of Re-Logic and owner of any assets used in this project and want them removed, please contact me.

## Gameplay:
A: Jump
Left/Right: Move
Up/Down: Zoom in/out
X/Y: Switch items
R: Open inventory
Start: Save map
Select: Load map
A+B+X+Y: Toggle debug mode

## Changelog and Features
- Version naming follows the Terraria PC versioning system, with a "pre" suffix for incomplete versions.
### Version 0.0.0.0pre (Working on it):
Initial commit with basic project structure.
- Basic tile rendering and scrolling. ***(done)***
- Touch input handling for world interaction and inventory interaction. ***(done)***
- Map generation (terrain, caves, trees and mushrooms). ***(done)***
- Map saving and loading functionality. ***(done. you can use START to save, and SELECT to load for now)***
- Camera zooming. ***(done)***
- Basic crafting system.

### Version 0.0.0.0alpha (Plans):
- Add health.
- Add menus.
- Add ability to change button mappings (since some DS have malfunctioning shoulder buttons because of dust accumulation over the years).
- Add entities (NPCs, enemies, etc.) to the game world.
- Add interactive tiles (chests, doors, etc.).
- Add ores and more items to the game.
- Add more biomes (desert, jungle, etc.).
- Support for maps with more height (limit is 64 for now).

## Planed Features for the future:
- Multiplayer support.
- Ability to change the map size.
- Skins (or changeable player styles).
- Add mana and magic items.
- Add bosses.

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

Now, you can compile the project by running:
```bash
make
```
This will create a `terraria.nds` file in the root directory of the project.

(Note: if you're not using the Msys2 shell, you may need to define the `DEVKITPRO` environment variable to point to your devkitPro installation directory, e.g. `export DEVKITPRO=/mnt/c/devkitPro` and `export DEVKITARM=/mnt/c/devkitPro/devkitARM`)