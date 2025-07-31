# TerrariaDS
A terraria remake (not port) for the Nintendo DS made by AzizBgBoss.

This project is a work in progress and is not affiliated with the original Terraria game. It aims to replicate the core mechanics and gameplay of Terraria on the Nintendo DS.
It aims at least to somewhat replicate the 3DS version of Terraria, but with some features from the PC version, and some sacrifices to fit the DS hardware limitations.

I'm trying as hard as possible to stay loyal to the original game design, but there are some limitations due to the DS hardware and the fact that this is a remake, not a port.

I'm very open to contributions and suggestions, so if you want to help, feel free to open a pull request or an issue.

## Changelog and Features
- Version naming follows the Terraria PC versioning system, with a "pre" suffix for incomplete versions.
### Version 0.0.0.0pre (soon):
- Initial commit with basic project structure.
- Basic tile rendering and scrolling.
- Touch input handling for world coordinates.
- Map generation with default tiles.
- Map saving and loading functionality.
- Camera zooming.

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