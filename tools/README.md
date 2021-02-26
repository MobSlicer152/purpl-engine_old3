## Purpl Engine Tools
This file is a guide to using the tools contained in the `<build dir>/tools/` folder.

### `mkembed`
This program converts a binary file into C source code, providing three symbols, `base_start`, `base_end`, and `base_size` ("base" is used as a stand-in here for whatever the second argument to the program is).
```
Usage: mkembed <binary file> <symbols basename> [<output>]
```
