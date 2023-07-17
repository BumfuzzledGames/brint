/* Brint                                                                    */
/* Copyright (C) 2023 BumfuzzledGames                                       */
/*                                                                          */
/* This program is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
#include "SDL.h"
#include "SDL_ttf.h"
#include <locale.h>

// Return 0xFF if the pixel is not zero, return 0 if the pixel is zero or outside the image
int GetPixel(SDL_Surface *s, int x, int y) {
  return 0xFF *
    (
      x >= 0 && x < s->w && 
      y >= 0 && y < s->h &&
      ((Uint8*)s->pixels)[y * s->pitch + x * s->format->BytesPerPixel] != 0
    );
}

int
main(
    int argc,
    char *argv[]
) {
  setlocale(LC_ALL, "en_US.UTF-8");

  // Correct number of arguments
  if(argc != 4) {
    fprintf(stderr, "Usage: %s <fontfile> <size> <text>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Init TTF
  if(TTF_Init()) {
    SDL_Log("Failed to init TTF: %s", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  // Parse size
  int size;
  if(sscanf(argv[2], "%d", &size) != 1) {
    fprintf(stderr, "Invalid size argument '%s'\n", argv[2]);
    exit(EXIT_FAILURE);
  }

  // Load font
  TTF_Font *font = TTF_OpenFont(argv[1], size);
  if(!font) {
    fprintf(stderr, "Cannot open font '%s' at size %d: %s\n", argv[1], size, TTF_GetError());
    exit(EXIT_FAILURE);
  }

  // Render text into surface
  SDL_Surface *surface = TTF_RenderText_Solid(font, argv[3], (SDL_Color){255, 255, 255, 255});
  if(!surface) {
    fprintf(stderr, "Cannot render text: %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  // Render text as braille
  for(int y = 0; y < surface->h; y += 4) {
    for(int x = 0; x < surface->w; x += 2) {
      wchar_t b = 0x2800 + (
          (0x01 & GetPixel(surface, x + 0, y + 0)) |
          (0x02 & GetPixel(surface, x + 0, y + 1)) |
          (0x04 & GetPixel(surface, x + 0, y + 2)) |
          (0x08 & GetPixel(surface, x + 1, y + 0)) |
          (0x10 & GetPixel(surface, x + 1, y + 1)) |
          (0x20 & GetPixel(surface, x + 1, y + 2)) |
          (0x40 & GetPixel(surface, x + 0, y + 3)) |
          (0x80 & GetPixel(surface, x + 1, y + 3))
        );
      wprintf(L"%lc", b);
    }
    wprintf(L"\n");
  }
}
