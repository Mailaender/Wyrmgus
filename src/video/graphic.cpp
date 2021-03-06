//       _________ __                 __
//      /   _____//  |_____________ _/  |______     ____  __ __  ______
//      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
//      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ |
//     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
//             \/                  \/          \//_____/            \/
//  ______________________                           ______________________
//                        T H E   W A R   B E G I N S
//         Stratagus - A free fantasy real time strategy game engine
//
/**@name graphic.cpp - The general graphic functions. */
//
//      (c) Copyright 1999-2011 by Lutz Sammer, Nehal Mistry, Jimmy Salmon and
//                                 Pali Rohár
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//

//@{

/*----------------------------------------------------------------------------
--  Includes
----------------------------------------------------------------------------*/

#include "stratagus.h"

#include <string>
#include <map>
#include <list>

//Wyrmgus start
#include "grand_strategy.h"
//Wyrmgus end
#include "video.h"
#include "player.h"
#include "intern_video.h"
#include "iocompat.h"
#include "iolib.h"
//Wyrmgus start
#include "results.h"
//Wyrmgus end
#include "ui.h"
//Wyrmgus start
#include "xbrz.h"
//Wyrmgus end

/*----------------------------------------------------------------------------
--  Variables
----------------------------------------------------------------------------*/

static int HashCount;
static std::map<std::string, CGraphic *> GraphicHash;
static std::list<CGraphic *> Graphics;

/*----------------------------------------------------------------------------
--  Functions
----------------------------------------------------------------------------*/

/**
**  Video draw the graphic clipped.
**
**  @param x   X screen position
**  @param y   Y screen position
*/
void CGraphic::DrawClip(int x, int y) const
{
	int oldx = x;
	int oldy = y;
	int w = Width;
	int h = Height;
	CLIP_RECTANGLE(x, y, w, h);
	DrawSub(x - oldx, y - oldy, w, h, x, y);
}

/**
**  Video draw part of graphic.
**
**  @param gx  X offset into object
**  @param gy  Y offset into object
**  @param w   width to display
**  @param h   height to display
**  @param x   X screen position
**  @param y   Y screen position
*/
void CGraphic::DrawSub(int gx, int gy, int w, int h, int x, int y) const
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		DrawTexture(this, Textures, gx, gy, gx + w, gy + h, x, y, 0);
	} else
#endif
	{
		SDL_Rect srect = {Sint16(gx), Sint16(gy), Uint16(w), Uint16(h)};
		SDL_Rect drect = {Sint16(x), Sint16(y), 0, 0};
		SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
		//Wyrmgus start
		//code for drawing a scaled image under xBRZ - use later for implementing zoom mode
		/*
		SDL_Rect srect = {Sint16(gx * 2), Sint16(gy * 2), Uint16(w * 2), Uint16(h * 2)};
		SDL_Rect drect = {Sint16(x), Sint16(y), 0, 0};
		SDL_LockSurface(Surface);
		
		SDL_Surface *neutral_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,1,1,32,0xFF0000,0xFF00,0xFF,0xFF000000);
		SDL_PixelFormat format = *neutral_surface->format;
		
		int Rmask = format.Rmask;
		int Gmask = format.Gmask;
		int Bmask = format.Bmask;
		int Amask = format.Amask;
		int bpp = format.BitsPerPixel;
		
		SDL_Surface *src = SDL_ConvertSurface(Surface,&format,SDL_SWSURFACE);
							 
		SDL_Surface *dst = SDL_CreateRGBSurface(SDL_SWSURFACE, Width * 2, Height * 2,
							 bpp, Rmask, Gmask, Bmask, Amask);
		SDL_LockSurface(src);
		SDL_LockSurface(dst);
		const Uint32* old_pixels = reinterpret_cast<const Uint32*>(src->pixels);
		Uint32* new_pixels = reinterpret_cast<Uint32*>(dst->pixels);
		xbrz::scale(2, old_pixels, new_pixels, Width, Height);
		SDL_SetAlpha(SDL_DisplayFormatAlpha(dst),SDL_SRCALPHA|SDL_RLEACCEL,SDL_ALPHA_OPAQUE);
		SDL_UnlockSurface(Surface);
		SDL_UnlockSurface(src);
		SDL_UnlockSurface(dst);
		SDL_BlitSurface(dst, &srect, TheScreen, &drect);

		unsigned char *src_pixels = NULL;

		if (src->flags & SDL_PREALLOC) {
			src_pixels = (unsigned char *)src->pixels;
		}
		SDL_FreeSurface(src);
		delete[] src_pixels;
		src = NULL;

		unsigned char *dst_pixels = NULL;

		if (dst->flags & SDL_PREALLOC) {
			dst_pixels = (unsigned char *)dst->pixels;
		}
		SDL_FreeSurface(dst);
		delete[] dst_pixels;
		dst = NULL;
		*/
		//Wyrmgus end
	}
}

//Wyrmgus start
/**
**  Video draw part of a player color graphic.
**
**  @param player  player number
**  @param gx  X offset into object
**  @param gy  Y offset into object
**  @param w   width to display
**  @param h   height to display
**  @param x   X screen position
**  @param y   Y screen position
*/
void CPlayerColorGraphic::DrawPlayerColorSub(int player, int gx, int gy, int w, int h, int x, int y)
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		if (!PlayerColorTextures[player]) {
			MakePlayerColorTexture(this, player);
		}
		DrawTexture(this, PlayerColorTextures[player], gx, gy, gx + w, gy + h, x, y, 0);
		//Wyrmgus end
	} else
#endif
	{
		GraphicPlayerPixels(player, *this);
		SDL_Rect srect = {Sint16(gx), Sint16(gy), Uint16(w), Uint16(h)};
		SDL_Rect drect = {Sint16(x), Sint16(y), 0, 0};
		SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
	}
}
//Wyrmgus end

/**
**  Video draw part of graphic clipped.
**
**  @param gx  X offset into object
**  @param gy  Y offset into object
**  @param w   width to display
**  @param h   height to display
**  @param x   X screen position
**  @param y   Y screen position
*/
void CGraphic::DrawSubClip(int gx, int gy, int w, int h, int x, int y) const
{
	int oldx = x;
	int oldy = y;
	CLIP_RECTANGLE(x, y, w, h);
	DrawSub(gx + x - oldx, gy + y - oldy, w, h, x, y);
}

//Wyrmgus start
/**
**  Video draw part of graphic clipped.
**
**  @param gx  X offset into object
**  @param gy  Y offset into object
**  @param w   width to display
**  @param h   height to display
**  @param x   X screen position
**  @param y   Y screen position
*/
void CPlayerColorGraphic::DrawPlayerColorSubClip(int player, int gx, int gy, int w, int h, int x, int y)
{
	int oldx = x;
	int oldy = y;
	CLIP_RECTANGLE(x, y, w, h);
	DrawPlayerColorSub(player, gx + x - oldx, gy + y - oldy, w, h, x, y);
}
//Wyrmgus end

/**
**  Video draw part of graphic with alpha.
**
**  @param gx     X offset into object
**  @param gy     Y offset into object
**  @param w      width to display
**  @param h      height to display
**  @param x      X screen position
**  @param y      Y screen position
**  @param alpha  Alpha
*/
void CGraphic::DrawSubTrans(int gx, int gy, int w, int h, int x, int y,
							unsigned char alpha) const
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(255, 255, 255, alpha);
		DrawSub(gx, gy, w, h, x, y);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else
#endif
	{
		int oldalpha = Surface->format->alpha;
		SDL_SetAlpha(Surface, SDL_SRCALPHA, alpha);
		DrawSub(gx, gy, w, h, x, y);
		SDL_SetAlpha(Surface, SDL_SRCALPHA, oldalpha);
	}
}

/**
**  Video draw part of graphic with alpha and clipped.
**
**  @param gx     X offset into object
**  @param gy     Y offset into object
**  @param w      width to display
**  @param h      height to display
**  @param x      X screen position
**  @param y      Y screen position
**  @param alpha  Alpha
*/
void CGraphic::DrawSubClipTrans(int gx, int gy, int w, int h, int x, int y,
								unsigned char alpha) const
{
	int oldx = x;
	int oldy = y;
	CLIP_RECTANGLE(x, y, w, h);
	DrawSubTrans(gx + x - oldx, gy + y - oldy, w, h, x, y, alpha);
}

/**
**  Draw graphic object unclipped.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CGraphic::DrawFrame(unsigned frame, int x, int y) const
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		DrawTexture(this, Textures, frame_map[frame].x, frame_map[frame].y,
					frame_map[frame].x +  Width, frame_map[frame].y + Height, x, y, 0);
	} else
#endif
	{
		DrawSub(frame_map[frame].x, frame_map[frame].y,
				Width, Height, x, y);
	}
}

#if defined(USE_OPENGL) || defined(USE_GLES)
void CGraphic::DoDrawFrameClip(GLuint *textures,
							   unsigned frame, int x, int y) const
{
	int ox;
	int oy;
	int skip;
	int w = Width;
	int h = Height;

	CLIP_RECTANGLE_OFS(x, y, w, h, ox, oy, skip);
	UNUSED(skip);
	DrawTexture(this, textures, frame_map[frame].x + ox,
				frame_map[frame].y + oy,
				frame_map[frame].x + ox + w,
				frame_map[frame].y + oy + h, x, y, 0);
}
#endif

/**
**  Draw graphic object clipped.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
//Wyrmgus start
//void CGraphic::DrawFrameClip(unsigned frame, int x, int y) const
void CGraphic::DrawFrameClip(unsigned frame, int x, int y, bool ignore_time_of_day)
//Wyrmgus end
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		//Wyrmgus start
//		DoDrawFrameClip(Textures, frame, x, y);
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			DoDrawFrameClip(Textures, frame, x, y);
		} else if (GameTimeOfDay == 1) {
			if (!TexturesDawn) {
				MakeTexture(this, GameTimeOfDay);
			}
			DoDrawFrameClip(TexturesDawn, frame, x, y);
		} else if (GameTimeOfDay == 5) {
			if (!TexturesDusk) {
				MakeTexture(this, GameTimeOfDay);
			}
			DoDrawFrameClip(TexturesDusk, frame, x, y);
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			if (!TexturesNight) {
				MakeTexture(this, GameTimeOfDay);
			}
			DoDrawFrameClip(TexturesNight, frame, x, y);
		}
		//Wyrmgus end
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
		//Wyrmgus end
		DrawSubClip(frame_map[frame].x, frame_map[frame].y,
					Width, Height, x, y);
	}
}

void CGraphic::DrawFrameTrans(unsigned frame, int x, int y, int alpha) const
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(255, 255, 255, alpha);
		DrawFrame(frame, x, y);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else
#endif
	{
		DrawSubTrans(frame_map[frame].x, frame_map[frame].y,
					 Width, Height, x, y, alpha);
	}
}

//Wyrmgus start
//void CGraphic::DrawFrameClipTrans(unsigned frame, int x, int y, int alpha) const
void CGraphic::DrawFrameClipTrans(unsigned frame, int x, int y, int alpha, bool ignore_time_of_day)
//Wyrmgus end
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(255, 255, 255, alpha);
		//Wyrmgus start
//		DrawFrameClip(frame, x, y);
		DrawFrameClip(frame, x, y, ignore_time_of_day);
		//Wyrmgus end
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
		//Wyrmgus end
		DrawSubClipTrans(frame_map[frame].x, frame_map[frame].y,
						 Width, Height, x, y, alpha);
	}
}

/**
**  Draw graphic object clipped and with player colors.
**
**  @param player  player number
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CPlayerColorGraphic::DrawPlayerColorFrameClip(int player, unsigned frame,
//Wyrmgus start
//												   int x, int y)
												   int x, int y, bool ignore_time_of_day)
//Wyrmgus end
{
	//Wyrmgus start
	if (!(GrandStrategy && !GameRunning && GameResult == GameNoResult)) {
		for (int i = 0; i < PlayerColorMax; ++i) {
			if (PlayerColors[i][0] == Players[player].Color) {
				player = i;
				break;
			}		
		}
	}
	//if in grand strategy mode, then treat the "player" variable as the player color index (necessary for drawing a player color graphic outside of a game)
	
	//Wyrmgus end
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		//Wyrmgus start
		/*
		if (!PlayerColorTextures[player]) {
			MakePlayerColorTexture(this, player);
		}
		DoDrawFrameClip(PlayerColorTextures[player], frame, x, y);
		*/
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			if (!PlayerColorTextures[player]) {
				MakePlayerColorTexture(this, player);
			}
			DoDrawFrameClip(PlayerColorTextures[player], frame, x, y);
		} else if (GameTimeOfDay == 1) {
			if (!PlayerColorTexturesDawn[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
			DoDrawFrameClip(PlayerColorTexturesDawn[player], frame, x, y);
		} else if (GameTimeOfDay == 5) {
			if (!PlayerColorTexturesDusk[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
			DoDrawFrameClip(PlayerColorTexturesDusk[player], frame, x, y);
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			if (!PlayerColorTexturesNight[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
			DoDrawFrameClip(PlayerColorTexturesNight[player], frame, x, y);
		}
		//Wyrmgus end
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
//		GraphicPlayerPixels(Players[player], *this);
		GraphicPlayerPixels(player, *this);
		//Wyrmgus end
		DrawFrameClip(frame, x, y);
	}
}

//Wyrmgus start
void CPlayerColorGraphic::DrawPlayerColorFrameClipTrans(int player, unsigned frame, int x, int y, int alpha, bool ignore_time_of_day)
{
	for (int i = 0; i < PlayerColorMax; ++i) {
		if (PlayerColors[i][0] == Players[player].Color) {
			player = i;
			break;
		}		
	}
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		//Wyrmgus start
		/*
		if (!PlayerColorTextures[player]) {
			MakePlayerColorTexture(this, player);
		}
		*/
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			if (!PlayerColorTextures[player]) {
				MakePlayerColorTexture(this, player);
			}
		} else if (GameTimeOfDay == 1) {
			if (!PlayerColorTexturesDawn[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
		} else if (GameTimeOfDay == 5) {
			if (!PlayerColorTexturesDusk[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			if (!PlayerColorTexturesNight[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
		}
		//Wyrmgus end
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(255, 255, 255, alpha);
		//Wyrmgus start
//		DoDrawFrameClip(PlayerColorTextures[player], frame, x, y);
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			DoDrawFrameClip(PlayerColorTextures[player], frame, x, y);
		} else if (GameTimeOfDay == 1) {
			DoDrawFrameClip(PlayerColorTexturesDawn[player], frame, x, y);
		} else if (GameTimeOfDay == 5) {
			DoDrawFrameClip(PlayerColorTexturesDusk[player], frame, x, y);
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			DoDrawFrameClip(PlayerColorTexturesNight[player], frame, x, y);
		}
		//Wyrmgus end
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
//		GraphicPlayerPixels(Players[player], *this);
		GraphicPlayerPixels(player, *this);
		//Wyrmgus end
		DrawFrameClipTrans(frame, x, y, alpha);
	}
}

//Wyrmgus start
//void CPlayerColorGraphic::DrawPlayerColorFrameClipTransX(int player, unsigned frame, int x, int y, int alpha)
void CPlayerColorGraphic::DrawPlayerColorFrameClipTransX(int player, unsigned frame, int x, int y, int alpha, bool ignore_time_of_day)
//Wyrmgus end
{
	//Wyrmgus start
	for (int i = 0; i < PlayerColorMax; ++i) {
		if (PlayerColors[i][0] == Players[player].Color) {
			player = i;
			break;
		}		
	}
	//Wyrmgus end
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		//Wyrmgus start
		/*
		if (!PlayerColorTextures[player]) {
			MakePlayerColorTexture(this, player);
		}
		*/
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			if (!PlayerColorTextures[player]) {
				MakePlayerColorTexture(this, player);
			}
		} else if (GameTimeOfDay == 1) {
			if (!PlayerColorTexturesDawn[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
		} else if (GameTimeOfDay == 5) {
			if (!PlayerColorTexturesDusk[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			if (!PlayerColorTexturesNight[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
		}
		//Wyrmgus end
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(255, 255, 255, alpha);
		//Wyrmgus start
//		DoDrawFrameClipX(PlayerColorTextures[player], frame, x, y);
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			DoDrawFrameClipX(PlayerColorTextures[player], frame, x, y);
		} else if (GameTimeOfDay == 1) {
			DoDrawFrameClipX(PlayerColorTexturesDawn[player], frame, x, y);
		} else if (GameTimeOfDay == 5) {
			DoDrawFrameClipX(PlayerColorTexturesDusk[player], frame, x, y);
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			DoDrawFrameClipX(PlayerColorTexturesNight[player], frame, x, y);
		}
		//Wyrmgus end
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
//		GraphicPlayerPixels(Players[player], *this);
		GraphicPlayerPixels(player, *this);
		//Wyrmgus end
		DrawFrameClipTransX(frame, x, y, alpha);
	}
}
//Wyrmgus end

/**
**  Draw graphic object unclipped and flipped in X direction.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CGraphic::DrawFrameX(unsigned frame, int x, int y) const
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		DrawTexture(this, Textures, frame_map[frame].x, frame_map[frame].y,
					frame_map[frame].x +  Width, frame_map[frame].y + Height, x, y, 1);
	} else
#endif
	{
		SDL_Rect srect = {frameFlip_map[frame].x, frameFlip_map[frame].y, Uint16(Width), Uint16(Height)};
		SDL_Rect drect = {Sint16(x), Sint16(y), 0, 0};

		SDL_BlitSurface(SurfaceFlip, &srect, TheScreen, &drect);
	}
}

#if defined(USE_OPENGL) || defined(USE_GLES)
void CGraphic::DoDrawFrameClipX(GLuint *textures, unsigned frame,
								int x, int y) const
{
	int ox;
	int oy;
	int skip;
	int w = Width;
	int h = Height;
	CLIP_RECTANGLE_OFS(x, y, w, h, ox, oy, skip);
	UNUSED(skip);

	if (w < Width) {
		if (ox == 0) {
			ox = Width - w;
		} else {
			ox = 0;
		}
	}

	DrawTexture(this, textures, frame_map[frame].x + ox,
				frame_map[frame].y + oy,
				frame_map[frame].x + ox + w,
				frame_map[frame].y + oy + h, x, y, 1);
}
#endif

/**
**  Draw graphic object clipped and flipped in X direction.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
//Wyrmgus start
//void CGraphic::DrawFrameClipX(unsigned frame, int x, int y) const
void CGraphic::DrawFrameClipX(unsigned frame, int x, int y, bool ignore_time_of_day)
//Wyrmgus end
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		//Wyrmgus start
		//DoDrawFrameClipX(Textures, frame, x, y);
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			DoDrawFrameClipX(Textures, frame, x, y);
		} else if (GameTimeOfDay == 1) {
			if (!TexturesDawn) {
				MakeTexture(this, GameTimeOfDay);
			}
			DoDrawFrameClipX(TexturesDawn, frame, x, y);
		} else if (GameTimeOfDay == 5) {
			if (!TexturesDusk) {
				MakeTexture(this, GameTimeOfDay);
			}
			DoDrawFrameClipX(TexturesDusk, frame, x, y);
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			if (!TexturesNight) {
				MakeTexture(this, GameTimeOfDay);
			}
			DoDrawFrameClipX(TexturesNight, frame, x, y);
		}
		//Wyrmgus end
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
		//Wyrmgus end
		SDL_Rect srect = {frameFlip_map[frame].x, frameFlip_map[frame].y, Uint16(Width), Uint16(Height)};

		const int oldx = x;
		const int oldy = y;
		CLIP_RECTANGLE(x, y, srect.w, srect.h);
		srect.x += x - oldx;
		srect.y += y - oldy;

		SDL_Rect drect = {Sint16(x), Sint16(y), 0, 0};

		SDL_BlitSurface(SurfaceFlip, &srect, TheScreen, &drect);
	}
}

void CGraphic::DrawFrameTransX(unsigned frame, int x, int y, int alpha) const
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(255, 255, 255, alpha);
		DrawFrameX(frame, x, y);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else
#endif
	{
		SDL_Rect srect = {frameFlip_map[frame].x, frameFlip_map[frame].y, Uint16(Width), Uint16(Height)};
		SDL_Rect drect = {Sint16(x), Sint16(y), 0, 0};
		const int oldalpha = Surface->format->alpha;

		SDL_SetAlpha(Surface, SDL_SRCALPHA, alpha);
		SDL_BlitSurface(SurfaceFlip, &srect, TheScreen, &drect);
		SDL_SetAlpha(Surface, SDL_SRCALPHA, oldalpha);
	}
}

//Wyrmgus start
//void CGraphic::DrawFrameClipTransX(unsigned frame, int x, int y, int alpha) const
void CGraphic::DrawFrameClipTransX(unsigned frame, int x, int y, int alpha, bool ignore_time_of_day)
//Wyrmgus end
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4ub(255, 255, 255, alpha);
		//Wyrmgus start
//		DrawFrameClipX(frame, x, y);
		DrawFrameClipX(frame, x, y, ignore_time_of_day);
		//Wyrmgus end
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
		//Wyrmgus end
		SDL_Rect srect = {frameFlip_map[frame].x, frameFlip_map[frame].y, Uint16(Width), Uint16(Height)};

		const int oldx = x;
		const int oldy = y;
		CLIP_RECTANGLE(x, y, srect.w, srect.h);
		srect.x += x - oldx;
		srect.y += y - oldy;

		SDL_Rect drect = {Sint16(x), Sint16(y), 0, 0};
		const int oldalpha = SurfaceFlip->format->alpha;

		SDL_SetAlpha(SurfaceFlip, SDL_SRCALPHA, alpha);
		SDL_BlitSurface(SurfaceFlip, &srect, TheScreen, &drect);
		SDL_SetAlpha(SurfaceFlip, SDL_SRCALPHA, oldalpha);
	}
}

/**
**  Draw graphic object clipped, flipped, and with player colors.
**
**  @param player  player number
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CPlayerColorGraphic::DrawPlayerColorFrameClipX(int player, unsigned frame,
//Wyrmgus start
//												   int x, int y)
												   int x, int y, bool ignore_time_of_day)
//Wyrmgus end
{
	//Wyrmgus start
	for (int i = 0; i < PlayerColorMax; ++i) {
		if (PlayerColors[i][0] == Players[player].Color) {
			player = i;
			break;
		}		
	}
	//Wyrmgus end
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		//Wyrmgus start
		/*
		if (!PlayerColorTextures[player]) {
			MakePlayerColorTexture(this, player);
		}
		DoDrawFrameClipX(PlayerColorTextures[player], frame, x, y);
		*/
		if (ignore_time_of_day || !GameTimeOfDay || GameTimeOfDay == 2 || GameTimeOfDay == 3 || GameTimeOfDay == 4) {
			if (!PlayerColorTextures[player]) {
				MakePlayerColorTexture(this, player);
			}
			DoDrawFrameClipX(PlayerColorTextures[player], frame, x, y);
		} else if (GameTimeOfDay == 1) {
			if (!PlayerColorTexturesDawn[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
			DoDrawFrameClipX(PlayerColorTexturesDawn[player], frame, x, y);
		} else if (GameTimeOfDay == 5) {
			if (!PlayerColorTexturesDusk[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
			DoDrawFrameClipX(PlayerColorTexturesDusk[player], frame, x, y);
		} else if (GameTimeOfDay == 6 || GameTimeOfDay == 7 || GameTimeOfDay == 8) {
			if (!PlayerColorTexturesNight[player]) {
				MakePlayerColorTexture(this, player, GameTimeOfDay);
			}
			DoDrawFrameClipX(PlayerColorTexturesNight[player], frame, x, y);
		}
		//Wyrmgus end
	} else
#endif
	{
		//Wyrmgus start
		if (!ignore_time_of_day) {
			SetTimeOfDay(GameTimeOfDay);
		}
//		GraphicPlayerPixels(Players[player], *this);
		GraphicPlayerPixels(player, *this);
		//Wyrmgus end
		DrawFrameClipX(frame, x, y);
	}
}

/*----------------------------------------------------------------------------
--  Global functions
----------------------------------------------------------------------------*/

/**
**  Make a new graphic object.
**
**  @param filename  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CGraphic *CGraphic::New(const std::string &filename, int w, int h)
{
	if (filename.empty()) {
		return new CGraphic;
	}

	const std::string file = LibraryFileName(filename.c_str());
	CGraphic *&g = GraphicHash[file];
	if (g == NULL) {
		g = new CGraphic;
		if (!g) {
			fprintf(stderr, "Out of memory\n");
			ExitFatal(-1);
		}
		// FIXME: use a constructor for this
		g->File = file;
		g->HashFile = g->File;
		g->Width = w;
		g->Height = h;
	} else {
		++g->Refs;
		Assert((w == 0 || g->Width == w) && (g->Height == h || h == 0));
	}

	return g;
}

/**
**  Make a new player color graphic object.
**
**  @param filename  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CPlayerColorGraphic *CPlayerColorGraphic::New(const std::string &filename, int w, int h)
{
	if (filename.empty()) {
		return new CPlayerColorGraphic;
	}

	const std::string file = LibraryFileName(filename.c_str());
	CPlayerColorGraphic *g = dynamic_cast<CPlayerColorGraphic *>(GraphicHash[file]);
	if (g == NULL) {
		g = new CPlayerColorGraphic;
		if (!g) {
			fprintf(stderr, "Out of memory\n");
			ExitFatal(-1);
		}
		// FIXME: use a constructor for this
		g->File = file;
		g->HashFile = g->File;
		g->Width = w;
		g->Height = h;
		GraphicHash[g->HashFile] = g;
	} else {
		++g->Refs;
		Assert((w == 0 || g->Width == w) && (g->Height == h || h == 0));
	}

	return g;
}

/**
**  Make a new graphic object.  Don't reuse a graphic from the hash table.
**
**  @param file  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CGraphic *CGraphic::ForceNew(const std::string &file, int w, int h)
{
	CGraphic *g = new CGraphic;
	if (!g) {
		fprintf(stderr, "Out of memory\n");
		ExitFatal(-1);
	}
	g->File = file;
	int bufSize = file.size() + 32;
	char *hashfile = new char[bufSize];
	snprintf(hashfile, bufSize, "%s%d", file.c_str(), HashCount++);
	g->HashFile = hashfile;
	delete[] hashfile;
	g->Width = w;
	g->Height = h;
	GraphicHash[g->HashFile] = g;

	return g;
}

/**
**  Clone a graphic
**
**  @param grayscale  Make grayscale texture
*/
CPlayerColorGraphic *CPlayerColorGraphic::Clone(bool grayscale) const
{
	CPlayerColorGraphic *g = CPlayerColorGraphic::ForceNew(this->File, this->Width, this->Height);

	if (this->IsLoaded()) {
		g->Load(grayscale);
	}

	return g;
}

/**
**  Make a new player color graphic object.  Don't reuse a graphic from the
**  hash table.
**
**  @param file  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CPlayerColorGraphic *CPlayerColorGraphic::ForceNew(const std::string &file, int w, int h)
{
	CPlayerColorGraphic *g = new CPlayerColorGraphic;
	if (!g) {
		fprintf(stderr, "Out of memory\n");
		ExitFatal(-1);
	}
	g->File = file;
	size_t bufSize = file.size() + 32;
	char *hashfile = new char[bufSize];
	snprintf(hashfile, bufSize, "%s%d", file.c_str(), HashCount++);
	g->HashFile = hashfile;
	delete[] hashfile;
	g->Width = w;
	g->Height = h;
	GraphicHash[g->HashFile] = g;

	return g;
}

/**
**  Get a graphic object.
**
**  @param filename  Filename
**
**  @return      Graphic object
*/
CGraphic *CGraphic::Get(const std::string &filename)
{
	if (filename.empty()) {
		return NULL;
	}

	const std::string file = LibraryFileName(filename.c_str());
	CGraphic *&g = GraphicHash[file];

	return g;
}

/**
**  Get a player color graphic object.
**
**  @param filename  Filename
**
**  @return      Graphic object
*/
CPlayerColorGraphic *CPlayerColorGraphic::Get(const std::string &filename)
{
	if (filename.empty()) {
		return NULL;
	}

	const std::string file = LibraryFileName(filename.c_str());
	CPlayerColorGraphic *g = dynamic_cast<CPlayerColorGraphic *>(GraphicHash[file]);

	return g;
}

void CGraphic::GenFramesMap()
{
	Assert(NumFrames != 0);
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		Assert(GraphicWidth != 0);
	} else
#endif
	{
		Assert(Surface != NULL);
	}
	Assert(Width != 0);
	Assert(Height != 0);

	delete[] frame_map;

	frame_map = new frame_pos_t[NumFrames];

	for (int frame = 0; frame < NumFrames; ++frame) {
#if defined(USE_OPENGL) || defined(USE_GLES)
		if (UseOpenGL) {
			frame_map[frame].x = (frame % (GraphicWidth / Width)) * Width;
			frame_map[frame].y = (frame / (GraphicWidth / Width)) * Height;
		} else
#endif
		{
			frame_map[frame].x = (frame % (Surface->w / Width)) * Width;
			frame_map[frame].y = (frame / (Surface->w / Width)) * Height;
		}
	}
}

static void ApplyGrayScale(SDL_Surface *Surface, int Width, int Height)
{
	SDL_LockSurface(Surface);
	const SDL_PixelFormat *f = Surface->format;
	const int bpp = Surface->format->BytesPerPixel;
	const double redGray = 0.21;
	const double greenGray = 0.72;
	const double blueGray = 0.07;

	switch (bpp) {
		case 1: {
			SDL_Color colors[256];
			SDL_Palette &pal = *Surface->format->palette;
			for (int i = 0; i < 256; ++i) {
				const int gray = redGray * pal.colors[i].r + greenGray * pal.colors[i].g + blueGray * pal.colors[i].b;
				colors[i].r = colors[i].g = colors[i].b = gray;
			}
			SDL_SetColors(Surface, &colors[0], 0, 256);
			break;
		}
		case 4: {
			Uint32 *p;
			for (int i = 0; i < Height; ++i) {
				for (int j = 0; j < Width; ++j) {
					p = (Uint32 *)(Surface->pixels) + i * Width + j * bpp;
					const Uint32 gray = ((Uint8)((*p) * redGray) >> f->Rshift) +
										((Uint8)(*(p + 1) * greenGray) >> f->Gshift) +
										((Uint8)(*(p + 2) * blueGray) >> f->Bshift) +
										((Uint8)(*(p + 3)) >> f->Ashift);
					*p = gray;
				}
			}
			break;
		}
	}
	SDL_UnlockSurface(Surface);
}

/**
**  Load a graphic
**
**  @param grayscale  Make a grayscale surface
*/
void CGraphic::Load(bool grayscale)
{
	if (Surface) {
		return;
	}

	// TODO: More formats?
	if (LoadGraphicPNG(this) == -1) {
		fprintf(stderr, "Can't load the graphic '%s'\n", File.c_str());
		ExitFatal(-1);
	}

	if (Surface->format->BytesPerPixel == 1) {
		VideoPaletteListAdd(Surface);
	}

	if (!Width) {
		Width = GraphicWidth;
	}
	if (!Height) {
		Height = GraphicHeight;
	}

	Assert(Width <= GraphicWidth && Height <= GraphicHeight);

	if ((GraphicWidth / Width) * Width != GraphicWidth ||
		(GraphicHeight / Height) * Height != GraphicHeight) {
		fprintf(stderr, "Invalid graphic (width, height) %s\n", File.c_str());
		fprintf(stderr, "Expected: (%d,%d)  Found: (%d,%d)\n",
				Width, Height, GraphicWidth, GraphicHeight);
		ExitFatal(-1);
	}

	NumFrames = GraphicWidth / Width * GraphicHeight / Height;

	if (grayscale) {
		ApplyGrayScale(Surface, Width, Height);
	}

#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		MakeTexture(this);
		Graphics.push_back(this);
	}
#endif
	GenFramesMap();
}

/**
**  Free a SDL surface
**
**  @param surface  SDL surface to free
*/
static void FreeSurface(SDL_Surface **surface)
{
	if (!*surface) {
		return;
	}
	VideoPaletteListRemove(*surface);

	unsigned char *pixels = NULL;

	if ((*surface)->flags & SDL_PREALLOC) {
		pixels = (unsigned char *)(*surface)->pixels;
	}

	SDL_FreeSurface(*surface);
	delete[] pixels;
	*surface = NULL;
}

/**
**  Free a graphic
**
**  @param g  Pointer to the graphic
*/
void CGraphic::Free(CGraphic *g)
{
	if (!g) {
		return;
	}

	Assert(g->Refs);

	--g->Refs;
	if (!g->Refs) {
#if defined(USE_OPENGL) || defined(USE_GLES)
		// No more uses of this graphic
		if (UseOpenGL) {
			if (g->Textures) {
				glDeleteTextures(g->NumTextures, g->Textures);
				delete[] g->Textures;
			}
			//Wyrmgus start
			if (g->TexturesDawn) {
				glDeleteTextures(g->NumTextures, g->TexturesDawn);
				delete[] g->TexturesDawn;
			}
			if (g->TexturesDusk) {
				glDeleteTextures(g->NumTextures, g->TexturesDusk);
				delete[] g->TexturesDusk;
			}
			if (g->TexturesNight) {
				glDeleteTextures(g->NumTextures, g->TexturesNight);
				delete[] g->TexturesNight;
			}
			//Wyrmgus end
			CPlayerColorGraphic *cg = dynamic_cast<CPlayerColorGraphic *>(g);
			if (cg) {
				//Wyrmgus start
//				for (int i = 0; i < PlayerMax; ++i) {
				for (int i = 0; i < PlayerColorMax; ++i) {
				//Wyrmgus end
					if (cg->PlayerColorTextures[i]) {
						glDeleteTextures(cg->NumTextures, cg->PlayerColorTextures[i]);
						delete[] cg->PlayerColorTextures[i];
					}
					//Wyrmgus start
					if (cg->PlayerColorTexturesDawn[i]) {
						glDeleteTextures(cg->NumTextures, cg->PlayerColorTexturesDawn[i]);
						delete[] cg->PlayerColorTexturesDawn[i];
					}
					if (cg->PlayerColorTexturesDusk[i]) {
						glDeleteTextures(cg->NumTextures, cg->PlayerColorTexturesDusk[i]);
						delete[] cg->PlayerColorTexturesDusk[i];
					}
					if (cg->PlayerColorTexturesNight[i]) {
						glDeleteTextures(cg->NumTextures, cg->PlayerColorTexturesNight[i]);
						delete[] cg->PlayerColorTexturesNight[i];
					}
					//Wyrmgus end
				}
			}
			Graphics.remove(g);
		}
#endif

		FreeSurface(&g->Surface);
		delete[] g->frame_map;
		g->frame_map = NULL;

#if defined(USE_OPENGL) || defined(USE_GLES)
		if (!UseOpenGL)
#endif
		{
			FreeSurface(&g->SurfaceFlip);
			delete[] g->frameFlip_map;
			g->frameFlip_map = NULL;
		}

		if (!g->HashFile.empty()) {
			GraphicHash.erase(g->HashFile);
		}
		delete g;
	}
}

#if defined(USE_OPENGL) || defined(USE_GLES)

/**
**  Free OpenGL graphics
*/
void FreeOpenGLGraphics()
{
	std::list<CGraphic *>::iterator i;
	for (i = Graphics.begin(); i != Graphics.end(); ++i) {
		if ((*i)->Textures) {
			glDeleteTextures((*i)->NumTextures, (*i)->Textures);
		}
		//Wyrmgus start
		if ((*i)->TexturesDawn) {
			glDeleteTextures((*i)->NumTextures, (*i)->TexturesDawn);
		}
		if ((*i)->TexturesDusk) {
			glDeleteTextures((*i)->NumTextures, (*i)->TexturesDusk);
		}
		if ((*i)->TexturesNight) {
			glDeleteTextures((*i)->NumTextures, (*i)->TexturesNight);
		}
		//Wyrmgus end
		CPlayerColorGraphic *cg = dynamic_cast<CPlayerColorGraphic *>(*i);
		if (cg) {
			//Wyrmgus start
//			for (int j = 0; j < PlayerMax; ++j) {
			for (int j = 0; j < PlayerColorMax; ++j) {
			//Wyrmgus end
				if (cg->PlayerColorTextures[j]) {
					glDeleteTextures(cg->NumTextures, cg->PlayerColorTextures[j]);
				}
				//Wyrmgus start
				if (cg->PlayerColorTexturesDawn[j]) {
					glDeleteTextures(cg->NumTextures, cg->PlayerColorTexturesDawn[j]);
				}
				if (cg->PlayerColorTexturesDusk[j]) {
					glDeleteTextures(cg->NumTextures, cg->PlayerColorTexturesDusk[j]);
				}
				if (cg->PlayerColorTexturesNight[j]) {
					glDeleteTextures(cg->NumTextures, cg->PlayerColorTexturesNight[j]);
				}
				//Wyrmgus end
			}
		}
	}
}

/**
**  Reload OpenGL graphics
*/
void ReloadGraphics()
{
	std::list<CGraphic *>::iterator i;
	for (i = Graphics.begin(); i != Graphics.end(); ++i) {
		if ((*i)->Textures) {
			delete[](*i)->Textures;
			(*i)->Textures = NULL;
			MakeTexture(*i);
		}
		//Wyrmgus start
		if ((*i)->TexturesDawn) {
			delete[](*i)->TexturesDawn;
			(*i)->TexturesDawn = NULL;
			MakeTexture(*i, 1);
		}
		if ((*i)->TexturesDusk) {
			delete[](*i)->TexturesDusk;
			(*i)->TexturesDusk = NULL;
			MakeTexture(*i, 5);
		}
		if ((*i)->TexturesNight) {
			delete[](*i)->TexturesNight;
			(*i)->TexturesNight = NULL;
			MakeTexture(*i, 7);
		}
		//Wyrmgus end
		CPlayerColorGraphic *cg = dynamic_cast<CPlayerColorGraphic *>(*i);
		if (cg) {
			//Wyrmgus start
//			for (int j = 0; j < PlayerMax; ++j) {
			for (int j = 0; j < PlayerColorMax; ++j) {
			//Wyrmgus end
				if (cg->PlayerColorTextures[j]) {
					delete[] cg->PlayerColorTextures[j];
					cg->PlayerColorTextures[j] = NULL;
					MakePlayerColorTexture(cg, j);
				}
				//Wyrmgus start
				if (cg->PlayerColorTexturesDawn[j]) {
					delete[] cg->PlayerColorTexturesDawn[j];
					cg->PlayerColorTexturesDawn[j] = NULL;
					MakePlayerColorTexture(cg, j, 1);
				}
				if (cg->PlayerColorTexturesDusk[j]) {
					delete[] cg->PlayerColorTexturesDusk[j];
					cg->PlayerColorTexturesDusk[j] = NULL;
					MakePlayerColorTexture(cg, j, 5);
				}
				if (cg->PlayerColorTexturesNight[j]) {
					delete[] cg->PlayerColorTexturesNight[j];
					cg->PlayerColorTexturesNight[j] = NULL;
					MakePlayerColorTexture(cg, j, 7);
				}
				//Wyrmgus end
			}
		}
	}
}

#endif

/**
**  Flip graphic and store in graphic->SurfaceFlip
*/
void CGraphic::Flip()
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		return;
	}
#endif
	if (SurfaceFlip) {
		return;
	}

	SDL_Surface *s = SurfaceFlip = SDL_ConvertSurface(Surface, Surface->format, SDL_SWSURFACE);
	if (Surface->flags & SDL_SRCCOLORKEY) {
		SDL_SetColorKey(SurfaceFlip, SDL_SRCCOLORKEY | SDL_RLEACCEL, Surface->format->colorkey);
	}
	if (SurfaceFlip->format->BytesPerPixel == 1) {
		VideoPaletteListAdd(SurfaceFlip);
	}
	SDL_LockSurface(Surface);
	SDL_LockSurface(s);
	switch (s->format->BytesPerPixel) {
		case 1:
			for (int i = 0; i < s->h; ++i) {
				for (int j = 0; j < s->w; ++j) {
					((char *)s->pixels)[j + i * s->pitch] =
						((char *)Surface->pixels)[s->w - j - 1 + i * Surface->pitch];
				}
			}
			break;
		case 3:
		//Wyrmgus start
		case 4: // doesn't work, but at least doesn't cause a crash
		//Wyrmgus end
			for (int i = 0; i < s->h; ++i) {
				for (int j = 0; j < s->w; ++j) {
					memcpy(&((char *)s->pixels)[j + i * s->pitch],
						   &((char *)Surface->pixels)[(s->w - j - 1) * 3 + i * Surface->pitch], 3);
				}
			}
			break;
		//Wyrmgus start
		/*
		case 4: {
			unsigned int p0 = s->pitch;
			unsigned int p1 = Surface->pitch;
			const int width = s->w;
			int j = 0;
			for (int i = 0; i < s->h; ++i) {
#ifdef _MSC_VER
				for (j = 0; j < width; ++j) {
					*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
						*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
				}
#else
				int n = (width + 7) / 8;
				switch (width & 7) {
					case 0: do {
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						case 7:
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						case 6:
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						case 5:
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						case 4:
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						case 3:
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						case 2:
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						case 1:
							*(Uint32 *)&((char *)s->pixels)[j * 4 + p0] =
								*(Uint32 *) & ((char *)Surface->pixels)[(width - j - 1) * 4 + p1];
							j++;
						} while (--n > 0);
				}
#endif
				p0 += s->pitch;
				p1 += Surface->pitch;
			}
		}
		break;
		*/
		//Wyrmgus end
	}
	SDL_UnlockSurface(Surface);
	SDL_UnlockSurface(s);

	delete[] frameFlip_map;

	frameFlip_map = new frame_pos_t[NumFrames];

	for (int frame = 0; frame < NumFrames; ++frame) {
		frameFlip_map[frame].x = (SurfaceFlip->w - (frame % (SurfaceFlip->w /
															 Width)) * Width) - Width;
		frameFlip_map[frame].y = (frame / (SurfaceFlip->w / Width)) * Height;
	}
}

/**
**  Convert the SDL surface to the display format
*/
void CGraphic::UseDisplayFormat()
{
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) { return; }
#endif

	SDL_Surface *s = Surface;

	if (s->format->Amask != 0) {
		Surface = SDL_DisplayFormatAlpha(s);
	} else {
		Surface = SDL_DisplayFormat(s);
	}
	VideoPaletteListRemove(s);
	SDL_FreeSurface(s);

	if (SurfaceFlip) {
		s = SurfaceFlip;
		if (s->format->Amask != 0) {
			SurfaceFlip = SDL_DisplayFormatAlpha(s);
		} else {
			SurfaceFlip = SDL_DisplayFormat(s);
		}
		VideoPaletteListRemove(s);
		SDL_FreeSurface(s);
	}
}

#if defined(USE_OPENGL) || defined(USE_GLES)

/**
**  Find the next power of 2 >= x
*/
static int PowerOf2(int x)
{
	int i;
	for (i = 1; i < x; i <<= 1) ;
	return i;
}

/**
**  Make an OpenGL texture or textures out of a graphic object.
**
**  @param g        The graphic object.
**  @param texture  Texture.
**  @param colors   Unit colors.
**  @param ow       Offset width.
**  @param oh       Offset height.
*/
//Wyrmgus start
//static void MakeTextures2(CGraphic *g, GLuint texture, CUnitColors *colors,
void MakeTextures2(CGraphic *g, GLuint texture, CUnitColors *colors,
//Wyrmgus end
						//Wyrmgus start
//						  int ow, int oh)
						  int ow, int oh, int time_of_day)
						//Wyrmgus end
{
	int useckey = g->Surface->flags & SDL_SRCCOLORKEY;
	SDL_PixelFormat *f = g->Surface->format;
	int bpp = f->BytesPerPixel;
	Uint32 ckey = f->colorkey;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	int maxw = std::min<int>(g->GraphicWidth - ow, GLMaxTextureSize);
	int maxh = std::min<int>(g->GraphicHeight - oh, GLMaxTextureSize);
	int w = PowerOf2(maxw);
	int h = PowerOf2(maxh);
	unsigned char *tex = new unsigned char[w * h * 4];
	memset(tex, 0, w * h * 4);
	unsigned char alpha;
	if (g->Surface->flags & SDL_SRCALPHA) {
		alpha = f->alpha;
	} else {
		alpha = 0xff;
	}

	SDL_LockSurface(g->Surface);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char *tp;
	const unsigned char *sp;
	Uint32 b;
	Uint32 c;
	Uint32 pc;
	
	//Wyrmgus start
	int time_of_day_red = 0;
	int time_of_day_green = 0;
	int time_of_day_blue = 0;
	
	if (time_of_day == 1) { // dawn
		time_of_day_red = -20;
		time_of_day_green = -20;
		time_of_day_blue = 0;
	} else if (time_of_day == 2) { // morning
		time_of_day_red = 0;
		time_of_day_green = 0;
		time_of_day_blue = 0;
	} else if (time_of_day == 3) { // midday
		time_of_day_red = 0;
		time_of_day_green = 0;
		time_of_day_blue = 0;
	} else if (time_of_day == 4) { // afternoon
		time_of_day_red = 0;
		time_of_day_green = 0;
		time_of_day_blue = 0;
	} else if (time_of_day == 5) { // dusk
		time_of_day_red = 0;
		time_of_day_green = -20;
		time_of_day_blue = -20;
	} else if (time_of_day == 6) { // first watch
		time_of_day_red = -45;
		time_of_day_green = -35;
		time_of_day_blue = -10;
	} else if (time_of_day == 7) { // midnight
		time_of_day_red = -45;
		time_of_day_green = -35;
		time_of_day_blue = -10;
	} else if (time_of_day == 8) { // second watch
		time_of_day_red = -45;
		time_of_day_green = -35;
		time_of_day_blue = -10;
	}
	//Wyrmgus end

	for (int i = 0; i < maxh; ++i) {
		sp = (const unsigned char *)g->Surface->pixels + ow * bpp +
			 (oh + i) * g->Surface->pitch;
		tp = tex + i * w * 4;
		for (int j = 0; j < maxw; ++j) {
			if (bpp == 1) {
				if (useckey && *sp == ckey) {
					tp[3] = 0;
				} else {
					SDL_Color p = f->palette->colors[*sp];
					//Wyrmgus start
					/*
					tp[0] = p.r;
					tp[1] = p.g;
					tp[2] = p.b;
					*/
					tp[0] = std::max<int>(0,std::min<int>(255,int(p.r) + time_of_day_red));
					tp[1] = std::max<int>(0,std::min<int>(255,int(p.g) + time_of_day_green));
					tp[2] = std::max<int>(0,std::min<int>(255,int(p.b) + time_of_day_blue));;
					//Wyrmgus end
					tp[3] = alpha;
				}
				if (colors) {
					for (int z = 0; z < PlayerColorIndexCount; ++z) {
						if (*sp == PlayerColorIndexStart + z) {
							SDL_Color p = colors->Colors[z];
							//Wyrmgus start
							/*
							tp[0] = p.r;
							tp[1] = p.g;
							tp[2] = p.b;
							*/
							tp[0] = std::max<int>(0,std::min<int>(255,int(p.r) + time_of_day_red));
							tp[1] = std::max<int>(0,std::min<int>(255,int(p.g) + time_of_day_green));
							tp[2] = std::max<int>(0,std::min<int>(255,int(p.b) + time_of_day_blue));;
							//Wyrmgus end
							tp[3] = 0xff;
							break;
						}
					}
				}
				++sp;
			} else {
				if (bpp == 4) {
					c = *(Uint32 *)sp;
				} else {
					c = (sp[f->Rshift >> 3] << f->Rshift) |
						(sp[f->Gshift >> 3] << f->Gshift) |
						(sp[f->Bshift >> 3] << f->Bshift);
					c |= ((alpha | (alpha << 8) | (alpha << 16) | (alpha << 24)) ^
						  (f->Rmask | f->Gmask | f->Bmask));
				}
				*(Uint32 *)tp = c;
				if (colors) {
					b = (c & f->Bmask) >> f->Bshift;
					if (b && ((c & f->Rmask) >> f->Rshift) == 0 &&
						((c & f->Gmask) >> f->Gshift) == b) {
						//Wyrmgus start
						/*
						pc = ((colors->Colors[0].R * b / 255) << f->Rshift) |
							 ((colors->Colors[0].G * b / 255) << f->Gshift) |
							 ((colors->Colors[0].B * b / 255) << f->Bshift);
						*/
						pc = ((std::max<int>(0,std::min<int>(255,int(colors->Colors[0].R) + time_of_day_red)) * b / 255) << f->Rshift) |
							 ((std::max<int>(0,std::min<int>(255,int(colors->Colors[0].G) + time_of_day_green)) * b / 255) << f->Gshift) |
							 ((std::max<int>(0,std::min<int>(255,int(colors->Colors[0].B) + time_of_day_blue)) * b / 255) << f->Bshift);
						//Wyrmgus end
						if (bpp == 4) {
							pc |= (c & f->Amask);
						} else {
							pc |= (0xFFFFFFFF ^ (f->Rmask | f->Gmask | f->Bmask));
						}
						*(Uint32 *)tp = pc;
					}
				}
				sp += bpp;
			}
			tp += 4;
		}
	}

	GLint internalformat = GL_RGBA;
#ifdef USE_OPENGL
	if (GLTextureCompressionSupported && UseGLTextureCompression) {
		internalformat = GL_COMPRESSED_RGBA;
	}
#endif

	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);

#ifdef DEBUG
	int x;
	if ((x = glGetError())) {
		DebugPrint("glTexImage2D(%x)\n" _C_ x);
	}
#endif
	SDL_UnlockSurface(g->Surface);
	delete[] tex;
}

/**
**  Make an OpenGL texture or textures out of a graphic object.
**
**  @param g       The graphic object.
**  @param player  Player number.
**  @param colors  Unit colors.
*/
//Wyrmgus start
//static void MakeTextures(CGraphic *g, int player, CUnitColors *colors)
static void MakeTextures(CGraphic *g, int player, CUnitColors *colors, int time_of_day)
//Wyrmgus end
{
	int tw = (g->GraphicWidth - 1) / GLMaxTextureSize + 1;
	const int th = (g->GraphicHeight - 1) / GLMaxTextureSize + 1;

	int w = g->GraphicWidth % GLMaxTextureSize;
	if (w == 0) {
		w = GLMaxTextureSize;
	}
	g->TextureWidth = (GLfloat)w / PowerOf2(w);

	int h = g->GraphicHeight % GLMaxTextureSize;
	if (h == 0) {
		h = GLMaxTextureSize;
	}
	g->TextureHeight = (GLfloat)h / PowerOf2(h);

	g->NumTextures = tw * th;

	CPlayerColorGraphic *cg = dynamic_cast<CPlayerColorGraphic *>(g);
	GLuint *textures;
	if (!colors || !cg) {
		//Wyrmgus start
//		textures = g->Textures = new GLuint[g->NumTextures];
//		glGenTextures(g->NumTextures, g->Textures);
		if (time_of_day == 1) {
			textures = g->TexturesDawn = new GLuint[g->NumTextures];
			glGenTextures(g->NumTextures, g->TexturesDawn);
		} else if (time_of_day == 5) {
			textures = g->TexturesDusk = new GLuint[g->NumTextures];
			glGenTextures(g->NumTextures, g->TexturesDusk);
		} else if (time_of_day == 6 || time_of_day == 7 || time_of_day == 8) {
			textures = g->TexturesNight = new GLuint[g->NumTextures];
			glGenTextures(g->NumTextures, g->TexturesNight);
		} else {
			textures = g->Textures = new GLuint[g->NumTextures];
			glGenTextures(g->NumTextures, g->Textures);
		}
		//Wyrmgus end
	//Wyrmgus start
	} else if (time_of_day == 1) {
		textures = cg->PlayerColorTexturesDawn[player] = new GLuint[cg->NumTextures];
		glGenTextures(cg->NumTextures, cg->PlayerColorTexturesDawn[player]);
	} else if (time_of_day == 5) {
		textures = cg->PlayerColorTexturesDusk[player] = new GLuint[cg->NumTextures];
		glGenTextures(cg->NumTextures, cg->PlayerColorTexturesDusk[player]);
	} else if (time_of_day == 6 || time_of_day == 7 || time_of_day == 8) {
		textures = cg->PlayerColorTexturesNight[player] = new GLuint[cg->NumTextures];
		glGenTextures(cg->NumTextures, cg->PlayerColorTexturesNight[player]);
	//Wyrmgus end
	} else {
		textures = cg->PlayerColorTextures[player] = new GLuint[cg->NumTextures];
		glGenTextures(cg->NumTextures, cg->PlayerColorTextures[player]);
	}

	for (int j = 0; j < th; ++j) {
		for (int i = 0; i < tw; ++i) {
			//Wyrmgus start
//			MakeTextures2(g, textures[j * tw + i], colors, GLMaxTextureSize * i, GLMaxTextureSize * j);
			MakeTextures2(g, textures[j * tw + i], colors, GLMaxTextureSize * i, GLMaxTextureSize * j, time_of_day);
			//Wyrmgus end
		}
	}
}

/**
**  Make an OpenGL texture or textures out of a graphic object.
**
**  @param g  The graphic object.
*/
//Wyrmgus start
//void MakeTexture(CGraphic *g)
void MakeTexture(CGraphic *g, int time_of_day)
//Wyrmgus end
{
	//Wyrmgus start
//	if (g->Textures) {
//		return;
//	}
	if (time_of_day == 1) {
		if (g->TexturesDawn) {
			return;
		}
	} else if (time_of_day == 5) {
		if (g->TexturesDusk) {
			return;
		}
	} else if (time_of_day == 6 || time_of_day == 7 || time_of_day == 8) {
		if (g->TexturesNight) {
			return;
		}
	} else {
		if (g->Textures) {
			return;
		}
	}
	//Wyrmgus end

	//Wyrmgus start
//	MakeTextures(g, 0, NULL);
	MakeTextures(g, 0, NULL, time_of_day);
	//Wyrmgus end
}

/**
**  Make an OpenGL texture with the player colors.
**
**  @param g       The graphic to texture with player colors.
**  @param player  Player number to make textures for.
*/
//Wyrmgus start
//void MakePlayerColorTexture(CPlayerColorGraphic *g, int player)
void MakePlayerColorTexture(CPlayerColorGraphic *g, int player, int time_of_day)
//Wyrmgus end
{
	//Wyrmgus start
	/*
	if (g->PlayerColorTextures[player]) {
		return;
	}
	*/
	if (time_of_day == 1) {
		if (g->PlayerColorTexturesDawn[player]) {
			return;
		}
	} else if (time_of_day == 5) {
		if (g->PlayerColorTexturesDusk[player]) {
			return;
		}
	} else if (time_of_day == 6 || time_of_day == 7 || time_of_day == 8) {
		if (g->PlayerColorTexturesNight[player]) {
			return;
		}
	} else {
		if (g->PlayerColorTextures[player]) {
			return;
		}
	}
	//Wyrmgus end

	//Wyrmgus start
//	MakeTextures(g, player, &Players[player].UnitColors);
	CUnitColors texture_unit_colors;
	texture_unit_colors.Colors = PlayerColorsRGB[player];
	MakeTextures(g, player, &texture_unit_colors, time_of_day);
	//Wyrmgus end
}

#endif

/**
**  Resize a graphic
**
**  @param w  New width of graphic.
**  @param h  New height of graphic.
*/
void CGraphic::Resize(int w, int h)
{
	Assert(Surface); // can't resize before it's been loaded

	if (GraphicWidth == w && GraphicHeight == h) {
		return;
	}

	// Resizing the same image multiple times looks horrible
	// If the image has already been resized then get a clean copy first
	if (Resized) {
		this->SetOriginalSize();
		if (GraphicWidth == w && GraphicHeight == h) {
			return;
		}
	}

	Resized = true;
	Uint32 ckey = Surface->format->colorkey;
	int useckey = Surface->flags & SDL_SRCCOLORKEY;

	int bpp = Surface->format->BytesPerPixel;
	if (bpp == 1) {
		SDL_Color pal[256];

		SDL_LockSurface(Surface);

		unsigned char *pixels = (unsigned char *)Surface->pixels;
		unsigned char *data = new unsigned char[w * h];
		int x = 0;

		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				data[x] = pixels[(i * Height / h) * Surface->pitch + j * Width / w];
				++x;
			}
		}

		SDL_UnlockSurface(Surface);
		VideoPaletteListRemove(Surface);

		memcpy(pal, Surface->format->palette->colors, sizeof(SDL_Color) * 256);
		SDL_FreeSurface(Surface);

		Surface = SDL_CreateRGBSurfaceFrom(data, w, h, 8, w, 0, 0, 0, 0);
		if (Surface->format->BytesPerPixel == 1) {
			VideoPaletteListAdd(Surface);
		}
		SDL_SetPalette(Surface, SDL_LOGPAL | SDL_PHYSPAL, pal, 0, 256);
	} else {
		SDL_LockSurface(Surface);

		unsigned char *pixels = (unsigned char *)Surface->pixels;
		unsigned char *data = new unsigned char[w * h * bpp];
		int x = 0;

		for (int i = 0; i < h; ++i) {
			float fy = (float)i * Height / h;
			int iy = (int)fy;
			fy -= iy;
			for (int j = 0; j < w; ++j) {
				float fx = (float)j * Width / w;
				int ix = (int)fx;
				fx -= ix;
				float fz = (fx + fy) / 2;

				unsigned char *p1 = &pixels[iy * Surface->pitch + ix * bpp];
				unsigned char *p2 = (iy != Surface->h - 1) ?
									&pixels[(iy + 1) * Surface->pitch + ix * bpp] :
									p1;
				unsigned char *p3 = (ix != Surface->w - 1) ?
									&pixels[iy * Surface->pitch + (ix + 1) * bpp] :
									p1;
				unsigned char *p4 = (iy != Surface->h - 1 && ix != Surface->w - 1) ?
									&pixels[(iy + 1) * Surface->pitch + (ix + 1) * bpp] :
									p1;

				data[x * bpp + 0] = static_cast<unsigned char>(
										(p1[0] * (1 - fy) + p2[0] * fy +
										 p1[0] * (1 - fx) + p3[0] * fx +
										 p1[0] * (1 - fz) + p4[0] * fz) / 3.0 + .5);
				data[x * bpp + 1] = static_cast<unsigned char>(
										(p1[1] * (1 - fy) + p2[1] * fy +
										 p1[1] * (1 - fx) + p3[1] * fx +
										 p1[1] * (1 - fz) + p4[1] * fz) / 3.0 + .5);
				data[x * bpp + 2] = static_cast<unsigned char>(
										(p1[2] * (1 - fy) + p2[2] * fy +
										 p1[2] * (1 - fx) + p3[2] * fx +
										 p1[2] * (1 - fz) + p4[2] * fz) / 3.0 + .5);
				if (bpp == 4) {
					data[x * bpp + 3] = static_cast<unsigned char>(
											(p1[3] * (1 - fy) + p2[3] * fy +
											 p1[3] * (1 - fx) + p3[3] * fx +
											 p1[3] * (1 - fz) + p4[3] * fz) / 3.0 + .5);
				}
				++x;
			}
		}

		int Rmask = Surface->format->Rmask;
		int Gmask = Surface->format->Gmask;
		int Bmask = Surface->format->Bmask;
		int Amask = Surface->format->Amask;

		SDL_UnlockSurface(Surface);
		VideoPaletteListRemove(Surface);
		SDL_FreeSurface(Surface);

		Surface = SDL_CreateRGBSurfaceFrom(data, w, h, 8 * bpp, w * bpp,
										   Rmask, Gmask, Bmask, Amask);
	}
	if (useckey) {
		SDL_SetColorKey(Surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, ckey);
	}
	Width = GraphicWidth = w;
	Height = GraphicHeight = h;

#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL && Textures) {
		glDeleteTextures(NumTextures, Textures);
		delete[] Textures;
		Textures = NULL;
		MakeTexture(this);
	}
#endif
	GenFramesMap();
}

/**
**  Sets the original size for a graphic
**
*/
void CGraphic::SetOriginalSize()
{
	Assert(Surface); // can't resize before it's been loaded

	if (!Resized) {
		return;
	}

	
	if (Surface) {
		FreeSurface(&Surface);
		Surface = NULL;
	}
	delete[] frame_map;
	frame_map = NULL;
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (!UseOpenGL)
#endif
	{
		if (SurfaceFlip) {
			FreeSurface(&SurfaceFlip);
			SurfaceFlip = NULL;
		}
		delete[] frameFlip_map;
		frameFlip_map = NULL;
	}

#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL && Textures) {
		glDeleteTextures(NumTextures, Textures);
		delete[] Textures;
		Textures = NULL;
	}
#endif

	this->Width = this->Height = 0;
	this->Surface = NULL;
	this->Load();

	Resized = false;
}

//Wyrmgus start
/**
**  Set a graphic's time of day
**
**  @param time  New time of day of graphic.
*/
void CGraphic::SetTimeOfDay(int time)
{
	Assert(Surface); // can't resize before it's been loaded

	if (TimeOfDay == time) {
		return;
	}

	// If the image has already had a time of day change, get a clean copy first
	if (TimeOfDay) {
		this->ResetTimeOfDay();
		if (TimeOfDay == time) {
			return;
		}
	}

	TimeOfDay = time;
	
	int time_of_day_red = 0;
	int time_of_day_green = 0;
	int time_of_day_blue = 0;
	
	if (time == 1) { // dawn
		time_of_day_red = -20;
		time_of_day_green = -20;
		time_of_day_blue = 0;
	} else if (time == 2) { // morning
		time_of_day_red = 0;
		time_of_day_green = 0;
		time_of_day_blue = 0;
	} else if (time == 3) { // midday
		time_of_day_red = 0;
		time_of_day_green = 0;
		time_of_day_blue = 0;
	} else if (time == 4) { // afternoon
		time_of_day_red = 0;
		time_of_day_green = 0;
		time_of_day_blue = 0;
	} else if (time == 5) { // dusk
		time_of_day_red = 0;
		time_of_day_green = -20;
		time_of_day_blue = -20;
	} else if (time == 6) { // first watch
		time_of_day_red = -45;
		time_of_day_green = -35;
		time_of_day_blue = -10;
	} else if (time == 7) { // midnight
		time_of_day_red = -45;
		time_of_day_green = -35;
		time_of_day_blue = -10;
	} else if (time == 8) { // second watch
		time_of_day_red = -45;
		time_of_day_green = -35;
		time_of_day_blue = -10;
	}
	
	if (time && (time_of_day_red != 0 || time_of_day_green != 0 || time_of_day_blue != 0)) {
		const int bpp = Surface->format->BytesPerPixel;
		if (bpp == 1) {
			SDL_LockSurface(Surface);
			SDL_Color colors[256];
			SDL_Palette &pal = *Surface->format->palette;
			for (int i = 0; i < 256; ++i) {
				colors[i].r = std::max<int>(0,std::min<int>(255,int(pal.colors[i].r) + time_of_day_red));
				colors[i].g = std::max<int>(0,std::min<int>(255,int(pal.colors[i].g) + time_of_day_green));
				colors[i].b = std::max<int>(0,std::min<int>(255,int(pal.colors[i].b) + time_of_day_blue));
			}
			SDL_SetColors(Surface, &colors[0], 0, 256);
			if (SurfaceFlip) {
				SDL_SetColors(SurfaceFlip, &colors[0], 0, 256);
			}
			SDL_UnlockSurface(Surface);
		} else if (bpp == 4) {
		}
	}
}

/**
**  Resets time of day for a graphic
**
*/
void CGraphic::ResetTimeOfDay()
{
	Assert(Surface); // can't resize before it's been loaded

	if (!TimeOfDay) {
		return;
	}
	
	bool flip_surface = false;

	if (Surface) {
		FreeSurface(&Surface);
		Surface = NULL;
	}
#if defined(USE_OPENGL) || defined(USE_GLES)
	if (!UseOpenGL)
#endif
	{
		if (SurfaceFlip) {
			flip_surface = true;
			FreeSurface(&SurfaceFlip);
			SurfaceFlip = NULL;
		}
	}

#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL && Textures) {
		glDeleteTextures(NumTextures, Textures);
		delete[] Textures;
		Textures = NULL;
		
		//Wyrmgus start
		if (TexturesDawn) {
			glDeleteTextures(NumTextures, TexturesDawn);
			delete[] TexturesDawn;
			TexturesDawn = NULL;
		}
		
		if (TexturesDusk) {
			glDeleteTextures(NumTextures, TexturesDusk);
			delete[] TexturesDusk;
			TexturesDusk = NULL;
		}
		
		if (TexturesNight) {
			glDeleteTextures(NumTextures, TexturesNight);
			delete[] TexturesNight;
			TexturesNight = NULL;
		}
		//Wyrmgus end
		
		CPlayerColorGraphic *cg = dynamic_cast<CPlayerColorGraphic *>(this);
		if (cg) {
			//Wyrmgus start
//			for (int j = 0; j < PlayerMax; ++j) {
			for (int j = 0; j < PlayerColorMax; ++j) {
			//Wyrmgus end
				if (cg->PlayerColorTextures[j]) {
					delete[] cg->PlayerColorTextures[j];
					cg->PlayerColorTextures[j] = NULL;
				}
				//Wyrmgus start
				if (cg->PlayerColorTexturesDawn[j]) {
					delete[] cg->PlayerColorTexturesDawn[j];
					cg->PlayerColorTexturesDawn[j] = NULL;
				}
				if (cg->PlayerColorTexturesDusk[j]) {
					delete[] cg->PlayerColorTexturesDusk[j];
					cg->PlayerColorTexturesDusk[j] = NULL;
				}
				if (cg->PlayerColorTexturesNight[j]) {
					delete[] cg->PlayerColorTexturesNight[j];
					cg->PlayerColorTexturesNight[j] = NULL;
				}
				//Wyrmgus end
			}
		}
	}
#endif

	this->Surface = NULL;
	this->Load();
	
	if (flip_surface) {
		this->Flip();
	}

	TimeOfDay = 0;	
}
//Wyrmgus end

/**
**  Check if a pixel is transparent
**
**  @param x  X coordinate
**  @param y  Y coordinate
**
**  @return   True if the pixel is transparent, False otherwise
*/
bool CGraphic::TransparentPixel(int x, int y)
{
	int bpp = Surface->format->BytesPerPixel;
	if ((bpp == 1 && !(Surface->flags & SDL_SRCCOLORKEY)) || bpp == 3) {
		return false;
	}

	bool ret = false;
	SDL_LockSurface(Surface);
	unsigned char *p = (unsigned char *)Surface->pixels + y * Surface->pitch + x * bpp;
	if (bpp == 1) {
		if (*p == Surface->format->colorkey) {
			ret = true;
		}
	} else {
		bool ckey = (Surface->flags & SDL_SRCCOLORKEY) > 0;
		if (ckey && *p == Surface->format->colorkey) {
			ret = true;
		} else if (p[Surface->format->Ashift >> 3] == 255) {
			ret = true;
		}
	}
	SDL_UnlockSurface(Surface);

	return ret;
}

/**
**  Make shadow sprite
**
**  @todo FIXME: 32bpp
*/
void CGraphic::MakeShadow()
{
	SDL_Color colors[256];

	// Set all colors in the palette to black and use 50% alpha
	memset(colors, 0, sizeof(colors));

	SDL_SetPalette(Surface, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, 256);
	//Wyrmgus start
//	SDL_SetAlpha(Surface, SDL_SRCALPHA | SDL_RLEACCEL, 128);
	SDL_SetAlpha(Surface, SDL_SRCALPHA | SDL_RLEACCEL, 192);
	//Wyrmgus end

#if defined(USE_OPENGL) || defined(USE_GLES)
	if (UseOpenGL) {
		if (Textures) {
			glDeleteTextures(NumTextures, Textures);
			delete[] Textures;
			Textures = NULL;
		}
		MakeTexture(this);
	} else
#endif
	{
		if (SurfaceFlip) {
			SDL_SetPalette(SurfaceFlip, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, 256);
			//Wyrmgus start
//			SDL_SetAlpha(SurfaceFlip, SDL_SRCALPHA | SDL_RLEACCEL, 128);
			SDL_SetAlpha(SurfaceFlip, SDL_SRCALPHA | SDL_RLEACCEL, 192);
			//Wyrmgus end
		}
	}
}

//Wyrmgus start
//#ifdef DEBUG
//Wyrmgus end
void FreeGraphics()
{
	std::map<std::string, CGraphic *>::iterator i;
	while (!GraphicHash.empty()) {
		i = GraphicHash.begin();
		CGraphic::Free((*i).second);
	}
}
//Wyrmgus start
//#endif
//Wyrmgus end

CFiller::bits_map::~bits_map()
{
	if (bstore) {
		free(bstore);
		bstore = NULL;
	}
	Width = 0;
	Height = 0;
}

void CFiller::bits_map::Init(CGraphic *g)
{
	SDL_Surface *s = g->Surface;
	int bpp = s->format->BytesPerPixel;

	if (bstore) {
		free(bstore);
		bstore = NULL;
		Width = 0;
		Height = 0;
	}

	if ((bpp == 1 && !(s->flags & SDL_SRCCOLORKEY)) || bpp == 3) {
		return;
	}

	Width = g->Width;
	Height = g->Height;

	size_t line = (Width + (sizeof(int) * 8) - 1) / (sizeof(int) * 8);
	size_t size = line * Height;

	bstore = (unsigned int *)calloc(size, sizeof(unsigned int));

	SDL_LockSurface(s);

	switch (s->format->BytesPerPixel) {
		case 1: {
			int ckey = s->format->colorkey;
			unsigned char *ptr = (unsigned char *)s->pixels;

			for (int i = 0; i < Height; ++i) {
				int l = i * Width;
				int lm = i * line;
				int k = 0;
				int p = 0;
				for (int j = 0; j < Width; ++j) {
					bstore[lm + k] |= ((ptr[j + l] != ckey) ? (1 << p) : 0);
					if (++p > 31) {
						p = 0;
						k++;
					}
				}
			}
			break;
		}
		case 2:
		case 3:
			break;
		case 4:
			if ((s->flags & SDL_SRCCOLORKEY) == SDL_SRCCOLORKEY) {
				unsigned int ckey = s->format->colorkey;
				unsigned int *ptr = (unsigned int *)s->pixels;

				for (int i = 0; i < Height; ++i) {
					int l = i * Width;
					int lm = i * line;
					int k = 0;
					int p = 0;
					for (int j = 0; j < Width; ++j) {
						bstore[lm + k] |= ((ptr[j + l] != ckey) ? (1 << p) : 0);
						if (++p > 31) {
							p = 0;
							k++;
						}
					}
				}
			} else {
				unsigned int *ptr = (unsigned int *)s->pixels;

				for (int i = 0; i < Height; ++i) {
					int l = i * Width;
					int lm = i * line;
					int k = 0;
					int p = 0;
					for (int j = 0; j < Width; ++j) {
						bstore[lm + k] |= ((ptr[j + l] & AMASK) ? (1 << p) : 0);
						if (++p > 31) {
							p = 0;
							k++;
						}
					}
				}
			}
			break;
		default:
			break;
	}

	SDL_UnlockSurface(s);
}

void CFiller::Load()
{
	if (G) {
		G->Load();
		map.Init(G);
		G->UseDisplayFormat();
	}
}

//@}
