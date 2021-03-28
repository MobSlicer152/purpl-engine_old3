## Purpl Engine Tools
This file is a guide to using the tools contained in the `<build dir>/tools/` folder.

### `mkembed`
This program converts a binary file into C source code, providing three symbols, `base_start`, `base_end`, and `base_size` ("base" is used as a stand-in here for whatever the second argument to the program is). Since this one is really cool and provides a portable way to embed binary files into executables, I'm considering porting it to not need any engine functions, which would be useful in general, since objcopy and similar programs are inconsistent and system dependant. Also, if you use this, no more Windows RCDATA files for custom resources (still useful for PE metadata strings and icons)
```
Usage: mkembed <binary file> <symbol basename> [<output>]
```
