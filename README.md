# lscpp

This is a rewrite of our beloved `ls` in C++.

> This is only tested on my macOS.

## Build

```
mkdir build
cd build
cmake ..
make
```

Then you get an executable `build/lscpp`.

## Supported Options

**List all files**
```
$ ./lscpp -a
_deps                   .cmake                  compile_commands.json
.                       cmake_install.cmake     lscpp
..                      CMakeCache.txt          Makefile
.cache                  CMakeFiles              tmep
```

**Long format**
```
$ ./lscpp -la
total 3920
drwxr-xr-x  8 natsucamellia  staff      256 Jan 14 15:12 _deps
drwxr-xr-x 12 natsucamellia  staff      384 Jan 19 15:04 .
drwxr-xr-x 10 natsucamellia  staff      320 Jan 15 16:16 ..
drwxr-x---  3 natsucamellia  staff       96 Jan 14 15:16 .cache
drwxr-xr-x  3 natsucamellia  staff       96 Jan 14 15:15 .cmake
-rw-r--r--  1 natsucamellia  staff     2437 Jan 14 15:12 cmake_install.cmake
-rw-r--r--  1 natsucamellia  staff    19385 Jan 14 15:12 CMakeCache.txt
drwxr-xr-x 12 natsucamellia  staff      384 Jan 19 15:04 CMakeFiles
-rw-r--r--  1 natsucamellia  staff     1993 Jan 15 16:20 compile_commands.json
-rwxr-xr-x  1 natsucamellia  staff  1967240 Jan 19 15:04 lscpp
-rw-r--r--  1 natsucamellia  staff     6137 Jan 15 16:20 Makefile
drwxr-xr-x  2 natsucamellia  staff       64 Jan 18 15:43 tmep
```

**Sort by time**
```
$ ./lscpp -lt
total 3920
drwxr-xr-x 12 natsucamellia  staff      384 Jan 19 15:04 CMakeFiles
-rwxr-xr-x  1 natsucamellia  staff  1967240 Jan 19 15:04 lscpp
drwxr-xr-x  2 natsucamellia  staff       64 Jan 18 15:43 tmep
-rw-r--r--  1 natsucamellia  staff     1993 Jan 15 16:20 compile_commands.json
-rw-r--r--  1 natsucamellia  staff     6137 Jan 15 16:20 Makefile
-rw-r--r--  1 natsucamellia  staff     2437 Jan 14 15:12 cmake_install.cmake
-rw-r--r--  1 natsucamellia  staff    19385 Jan 14 15:12 CMakeCache.txt
drwxr-xr-x  8 natsucamellia  staff      256 Jan 14 15:12 _deps
```

**Sort in reverse order**
```
$ ./lscpp -ltr
total 3920
drwxr-xr-x  8 natsucamellia  staff      256 Jan 14 15:12 _deps
-rw-r--r--  1 natsucamellia  staff    19385 Jan 14 15:12 CMakeCache.txt
-rw-r--r--  1 natsucamellia  staff     2437 Jan 14 15:12 cmake_install.cmake
-rw-r--r--  1 natsucamellia  staff     6137 Jan 15 16:20 Makefile
-rw-r--r--  1 natsucamellia  staff     1993 Jan 15 16:20 compile_commands.json
drwxr-xr-x  2 natsucamellia  staff       64 Jan 18 15:43 tmep
-rwxr-xr-x  1 natsucamellia  staff  1967240 Jan 19 15:04 lscpp
drwxr-xr-x 12 natsucamellia  staff      384 Jan 19 15:04 CMakeFiles
```

**Recursive list**
```
$ ./lscpp -R
_deps                   CMakeFiles              Makefile
cmake_install.cmake     compile_commands.json   tmep
CMakeCache.txt          lscpp

./_deps:
cli11-build     cli11-subbuild  fmt-src
cli11-src       fmt-build       fmt-subbuild

./_deps/cli11-build:
cmake_install.cmake     fuzz                    single-include
CMakeFiles              Makefile                src

./_deps/cli11-build/CMakeFiles:
CMakeDirectoryInformation.cmake progress.marks

./_deps/cli11-build/fuzz:
cmake_install.cmake     CMakeFiles              Makefile

./_deps/cli11-build/fuzz/CMakeFiles:
CMakeDirectoryInformation.cmake progress.marks

...
```
