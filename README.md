Game Boy Emulator
==

### Info

A Game Boy Emulator, which emulates a Game Boy. Currently work in progress. I'm using SDL to render graphics. I'll add compile instructions to this README once the emulator is running properly. It would be easiest to compile with Linux as setting up SDL with Visual Studio is kind of annoying, which is mainly why I'm using Linux to write this now.

The Game Boy uses the Sharp LR35902, an 8-bit CPU based on the Zilog Z80 and Intel 8080. It has a display of 160x144 pixels and uses a tile system of 8x8 pixels each to render the sprites and background/window tiles.

|  | Progress |
| ------------- | ------------- |
| CPU | Implemented (with bugs) |
| Video | Implemented (with bugs) |
| Timers | In progress |
| Interrupts | In progress |
| Input | Not implemented |
| Audio | Not implemented |

### Resources

A lot of the documentation on the Game Boy is inaccurate. I used several resources to help me in creating this.

[CPU Manual](http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf) - There are a ton of errors in this, but it was useful when writing all the operations and functions

[Imran Nazar: GameBoy Emulation in JavaScript](http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-The-CPU) 

[GBdevWiki](http://gbdev.gg8.se/wiki/articles/Main_Page) - Super helpful wiki with specific details about everything

[GBZ80 Opcode reference](https://rednex.github.io/rgbds/gbz80.7.html) An accurate guide to the operations, though there was a mistake on it that might have been fixed by now

[Blargg's Game Boy hardware test roms](https://github.com/retrio/gb-test-roms) - Seriously helpful and probably necessary considering how much documentation is inaccurate. These ROMs will test everything pretty thoroughly and output the results to the screen (assuming you have background rendering implemented). 
