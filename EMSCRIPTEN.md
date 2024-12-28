# Emscripten

## Build

```
./autogen.sh
emconfigure ./configure SF_RUN_IN_PLACE=1 SF_NOFONT=1 SF_OLD_MUSIC=1 'CFLAGS=-O3 -flto -fno-rtti -fno-exceptions'
emmake make
```

## Link

```
emcc -flto -O3 -fno-rtti -fno-exceptions src/*.o -o index.html -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png","jpg"]' -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS='["ogg","mod"]' -sASYNCIFY -sASYNCIFY_IGNORE_INDIRECT -sASYNCIFY_ONLY=@funcs.txt -sINITIAL_MEMORY=128mb -sENVIRONMENT=web --preload-file gfx/ --preload-file music/ --preload-file sound/ --preload-file data/ --closure 1 -sEXPORTED_RUNTIME_METHODS=['allocate']
```
