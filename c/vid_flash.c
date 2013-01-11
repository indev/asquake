/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// vid_null.c -- null video driver to aid porting efforts

#include "quakedef.h"
#include "d_local.h"

viddef_t	vid;				// global video state

#define	BASEWIDTH	640
#define	BASEHEIGHT	480
#define SURFCACHE_SIZE_AT_640X480	1344000

byte	vid_buffer[BASEWIDTH*BASEHEIGHT];
short	zbuffer[BASEWIDTH*BASEHEIGHT];
byte	surfcache[SURFCACHE_SIZE_AT_640X480];

unsigned videobuffer32[BASEWIDTH*BASEHEIGHT];

static byte current_palette[768];

unsigned d_8to24table[256];
	
void	VID_SetPalette (unsigned char *palette)
{
	//Sys_Printf("VID_SetPalette\n");

	unsigned char *ptr;
	
	int i = 768;
	while (i--)
	current_palette[i] = palette[i];
	
	ptr = current_palette;

    for (i = 0; i < 256; i++) {
		unsigned char r, g, b;
		r = *ptr++;
		g = *ptr++;
		b = *ptr++;
		d_8to24table[i] = (255<<24) + (r<<16) + (g<<8) + (b<<0);
    }
}

void	VID_ShiftPalette (unsigned char *palette)
{
	VID_SetPalette(palette);
}

void	VID_Init (unsigned char *palette)
{
	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.width = vid.conwidth = BASEWIDTH;
	vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;
	
	Sys_Printf("VID_Init: %dx %d\n", vid.width, vid.height);

	d_pzbuffer = zbuffer;
	 
	D_InitCaches (surfcache, sizeof(surfcache));
	
	VID_SetPalette(palette);
}

void	VID_Shutdown (void)
{
}

void	VID_Update (vrect_t *rects)
{
	int len = BASEWIDTH*BASEHEIGHT;
	int i=0;
	for ( i = 0; i < len; ++i ) {
		videobuffer32[i] = d_8to24table[vid_buffer[i]];
	}
}

/*
================
D_BeginDirectRect
================
*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}


/*
================
D_EndDirectRect
================
*/
void D_EndDirectRect (int x, int y, int width, int height)
{
}


