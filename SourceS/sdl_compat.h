// Compatibility wrappers for SDL 1 & 2.
#include <SDL.h>

#if defined(__AMIGA__) // Add other systems that require an 8bit screen here
#define D_BPP 8
#else
#define D_BPP 0
#endif

inline int SDLC_SetColorKey(SDL_Surface *surface, Uint32 key)
{
#ifdef USE_SDL1
	return SDL_SetColorKey(surface, SDL_SRCCOLORKEY, key);
#else
	return SDL_SetColorKey(surface, SDL_TRUE, key);
#endif
}

// Copies the colors into the surface's palette.
inline int SDLC_SetSurfaceColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors)
{
#ifdef USE_SDL1
	#if D_BPP == 8
		// In SDL1, if the Video Surface is 8bit, you need to set this as well to match,
		// otherwise colors will be wrong!
		SDL_SetColors(SDL_GetVideoSurface(), colors, firstcolor, ncolors);
	#endif
	return SDL_SetPalette(surface, SDL_LOGPAL, colors, firstcolor, ncolors) - 1;
#else
	return SDL_SetPaletteColors(surface->format->palette, colors, firstcolor, ncolors);
#endif
}

// Copies the colors into the surface's palette.
inline int SDLC_SetSurfaceColors(SDL_Surface *surface, SDL_Palette *palette)
{
	return SDLC_SetSurfaceColors(surface, palette->colors, 0, palette->ncolors);
}

// Sets the palette's colors and:
// SDL2: Points the surface's palette to the given palette if necessary.
// SDL1: Sets the surface's colors.
inline int SDLC_SetSurfaceAndPaletteColors(SDL_Surface *surface, SDL_Palette *palette, SDL_Color *colors, int firstcolor, int ncolors)
{
#ifdef USE_SDL1
	if (ncolors > (palette->ncolors - firstcolor)) {
		SDL_SetError("ncolors > (palette->ncolors - firstcolor)");
		return -1;
	}
	if (colors != (palette->colors + firstcolor))
		SDL_memcpy(palette->colors + firstcolor, colors, ncolors * sizeof(*colors));

	#if D_BPP == 8
		// In SDL1, if the Video Surface is 8bit, you need to set this as well to match,
		// otherwise colors will be wrong!
		SDL_SetColors(SDL_GetVideoSurface(), colors, firstcolor, ncolors);
	#endif
	// In SDL1, the surface always has its own distinct palette, so we need to
	// update it as well.
	return SDL_SetPalette(surface, SDL_LOGPAL, colors, firstcolor, ncolors) - 1;
#else
	if (SDL_SetPaletteColors(palette, colors, firstcolor, ncolors) < 0)
		return -1;
	if (surface->format->palette != palette)
		return SDL_SetSurfacePalette(surface, palette);
	return 0;
#endif
}
