# chip8-emu
A Chip-8 emulator in C++ using [SFML](https://github.com/SFML/SFML) for window creation and graphics.

### Screenshots
![No debug](http://i.imgur.com/HVClq9i.png)

With Debug Mode
![Debug](http://i.imgur.com/ti4mCFg.png)
### Requirements
* Visual Studio 2015

### Usage
`chip8-emu.exe /path/to/rom`

### Controls
The controls for the emulator are as follows:

    +-+-+-+-+
    |1|2|3|4|
    +-+-+-+-+
    |Q|W|E|R|
    +-+-+-+-+
    |A|S|D|F|
    +-+-+-+-+
    |Z|X|C|V|
    +-+-+-+-+
    
Special Emulator controls:
* **F1**: Pause/Resume Emulation
* **F2**: Step (Emulate 1 instruction)
* **F3**: Toggle Debug Mode
* **Tab**: (Hold) Disable Throttling
