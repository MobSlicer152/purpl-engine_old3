import os
import sys
import pathlib

try:
    name = str(sys.argv[1])
except IndexError:
    name = input('File name: ')
    while not name:
       name = input('File name: ')
       if name:
            break
       print('No file name given.')

try:
    os.chdir(sys.argv[2])
except IndexError:
    chdir = input('Change to alternate directory first [current directory]: ')
    if chdir:
        os.chdir(chdir)

# Check if the file is there and not empty, in which case prompt whether to overwrite it
if pathlib.Path(name).exists() and os.stat(name).st_size:
    res = input('File exists and is not empty, overwrite it? [no] ')
    if res == 'y' or res == 'Y' or res == 'yes' or res == 'Yes':
        print('Overwriting file.')
    else:
        print('Not overwriting file.')
        exit()

with open(name, 'wb+') as file:
    # Generate the include guard symbol for the header, then write everything to the file
    symbol = name.replace('include/', '').replace('include\\', '').replace('.',
                                                  '_').replace('/', '_').replace('\\', '_').upper()
    contents = bytes(
        F"#pragma once\n\n#ifndef {symbol}\n#define {symbol} 1\n\n#ifdef __cplusplus\nextern \"C\" " + "{\n#endif\n\n", encoding='utf8')

    contents += bytes("#ifdef __cplusplus\n}\n#endif" + F"\n\n#endif /* !{symbol} */\n", encoding='utf8')
    file.write(contents)
