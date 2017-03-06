# SFML-DOOM
[![Build status](https://ci.appveyor.com/api/projects/status/frefdhc7xop8dblt?svg=true)](https://ci.appveyor.com/project/JonnyPtn/sfml-doom)

Original DOOM source code adapted to use c++ (as modern as possible...).

Makes use of SFML for the window, graphics, input, audio and networking.

Intended to work exactly as originally intended all those years ago, so if you notice anything out of order please let me know!

I'm also very open to improvement suggestions.

## Building

SFML is the only dependency, so get it from your package manager or pre-built binaries from www.sfml-dev.org

Use cmake to generate the project files, setting the SFML_ROOT cmake variable to the path to your SFML installation
