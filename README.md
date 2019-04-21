# ds2png
Converts Sega Digitizer System III data files to split PNG images.

## Operation
The code will split each frame inside Digitizer file into its own output PNG and will generate an info file 'ds3_info.txt' with metadata extracted from the file. The frames could be positioned on the screen, and so each frame has an X/Y offset. It's unknown what resolution the screen was, though it was likely 320x240. Rendering all frames to a single output 'screen' image may come in a later version.

## Building
Requires libpng. The location of the library on your system can be specified in the makefile. Run ```make init``` the first time to create the build directories, then ```make``` to generate the executable in the bin directory.

## Usage
```--input```,```-i```

Specifies input Digitizer format file.

```--trns```,```-t```

When set, will make all pixels with tile entry 0 transparent.

### Usage Example
    cat BAT.DS | ./ds2png -t

## Notes
This is based on the reverse engineering work done by Rich Whitehouse. 
http://richwhitehouse.com/pubdata/dig3_spec.txt
