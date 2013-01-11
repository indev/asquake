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
// sys_null.h -- null system driver to aid porting efforts

#include "quakedef.h"
#include "errno.h"

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES             10
FILE    *sys_handles[MAX_HANDLES];

int             findhandle (void)
{
	int             i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength (FILE *f)
{
	int             pos;
	int             end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	
	sys_handles[i] = f;
	*hndl = i;
	
	return filelength(f);
}

int Sys_FileOpenWrite (char *path)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;
	
	return i;
}

void Sys_FileClose (int handle)
{
	fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek (int handle, int position)
{
	fseek (sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	return fread (dest, 1, count, sys_handles[handle]);
}

int Sys_FileWrite (int handle, void *data, int count)
{
	return fwrite (data, 1, count, sys_handles[handle]);
}

int Sys_FileTime (char *path)
{
	return 1;
/*
	FILE    *f;
	
	f = fopen(path, "rb");
	if (f)
	{
		fclose(f);
		return 1;
	}
	
	return -1;
*/
}

void Sys_mkdir (char *path)
{
}


/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
}


void Sys_Error (char *error, ...)
{
	va_list         argptr;

	printf ("Sys_Error: ");   
	va_start (argptr,error);
	vprintf (error,argptr);
	va_end (argptr);
	printf ("\n");

	exit (1);
}

void Sys_Printf (char *fmt, ...)
{
	va_list         argptr;
	
	va_start (argptr,fmt);
	vprintf (fmt,argptr);
	va_end (argptr);
}

void Sys_Quit (void)
{
	//exit (0);
}

double Sys_FloatTime (void)
{
	static double t;
	
	t += 0.1;
	
	return t;
}

char *Sys_ConsoleInput (void)
{
	return NULL;
}

void Sys_Sleep (void)
{
}

void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}

//Flash helper functions
/* Does a FILE * read against a ByteArray */
static int readByteArray(void *cookie, char *dst, int size)
{
	return AS3_ByteArray_readBytes(dst, (AS3_Val)cookie, size);
}
 
/* Does a FILE * write against a ByteArray */
static int writeByteArray(void *cookie, const char *src, int size)
{
	return AS3_ByteArray_writeBytes((AS3_Val)cookie, (char *)src, size);
}
 
/* Does a FILE * lseek against a ByteArray */
static fpos_t seekByteArray(void *cookie, fpos_t offs, int whence)
{
	return AS3_ByteArray_seek((AS3_Val)cookie, offs, whence);
}
 
/* Does a FILE * close against a ByteArray */
static int closeByteArray(void * cookie)
{
	AS3_Val zero = AS3_Int(0);
	 
	/* just reset the position */
	AS3_SetS((AS3_Val)cookie, "position", zero);
	AS3_Release(zero);
	return 0;
}

//=============================================================================
AS3_Val quakeTick();
AS3_Val quakeInit();
AS3_Val quakeGetVideoBufferPtr();
AS3_Val quakePaintSoundBuffer();
AS3_Val quakeSetKeys();
AS3_Val quakeSetMousePosition();
AS3_Val quakeRegisterFlashQuake();

int flashKeys[256];
int delta_mouse_x, delta_mouse_y;

int main (int argc, char **argv)
{
	flashQuakeObject = AS3_Undefined();
	
	AS3_Val initMethod = AS3_Function(NULL, quakeInit);
	AS3_Val tickMethod = AS3_Function(NULL, quakeTick);
	AS3_Val getVideoBufferMethod = AS3_Function(NULL, quakeGetVideoBufferPtr);
	AS3_Val setKeysMethod = AS3_Function(NULL, quakeSetKeys);
	AS3_Val setMousePositionMethod = AS3_Function(NULL, quakeSetMousePosition);
	AS3_Val registerFlashQuakeMethod = AS3_Function(NULL, quakeRegisterFlashQuake);
	AS3_Val paintSoundBufferMethod = AS3_Function(NULL, quakePaintSoundBuffer);
	
	AS3_Val asQuakeLib;
	
	asQuakeLib = AS3_Object( "quakeInit:AS3ValType, quakeTick:AS3ValType, quakeGetVideoBufferPtr:AS3ValType, quakeSetKeys:AS3ValType, quakeSetMousePosition:AS3ValType, quakeRegisterFlashQuake:AS3ValType, quakePaintSoundBuffer:AS3ValType",
						  initMethod, tickMethod, getVideoBufferMethod, setKeysMethod, setMousePositionMethod, registerFlashQuakeMethod, paintSoundBufferMethod );
	
	AS3_Release( initMethod );
	AS3_Release( tickMethod );
	AS3_Release( getVideoBufferMethod );
	AS3_Release( setKeysMethod );
	AS3_Release( setMousePositionMethod );
	AS3_Release( registerFlashQuakeMethod );
	AS3_Release( paintSoundBufferMethod );
	
	AS3_LibInit(asQuakeLib);
	
	return 0;
}

AS3_Val quakeTick (void *data, AS3_Val args) 
{
	double tick = 0.05;
	AS3_ArrayValue(args, "DoubleType", &tick);
	
	if ( tick < 0.05 ) tick = 0.05;
	
	Host_Frame ( (float)tick );
	
	return AS3_Int(0);
}

AS3_Val quakeInit (void *data, AS3_Val args) 
{
	printf ("Flash_init\n");

	in_mlook.state = 1;
	
	delta_mouse_x = 0;
	delta_mouse_y = 0;
	
	static quakeparms_t    parms;

	parms.memsize = 8*1024*1024;
	parms.membase = malloc (parms.memsize);
	parms.basedir = ".";
	
	COM_InitArgv (0, NULL);

	parms.argc = com_argc;
	parms.argv = com_argv;

	Host_Init (&parms);

	return AS3_Int(0);
}

AS3_Val quakeGetVideoBufferPtr (void *data, AS3_Val args) 
{
	extern unsigned videobuffer32[];
	return AS3_Ptr((unsigned*)videobuffer32);
}

AS3_Val quakeRegisterFlashQuake (void *data, AS3_Val args) 
{
	printf("quakeRegisterFlashQuake");
	AS3_ArrayValue( args, "AS3ValType", &flashQuakeObject);

	return AS3_Int(0);
}

AS3_Val quakeSetKeys(void *data, AS3_Val args)
{
	int index;
	int value;
	
	AS3_ArrayValue(args, "IntType,IntType", &index, &value);

	//printf("NUMMER_INDEX: %d\n", index );
	//printf("NUMMER_VALUE: %d\n", value );
	
	flashKeys[index] = value;
	
	Key_Event( index, value == 1 );
	
	return AS3_Int(0);
}

AS3_Val quakeSetMousePosition(void *data, AS3_Val args)
{
	AS3_ArrayValue(args, "IntType,IntType", &delta_mouse_x, &delta_mouse_y);

	return AS3_Int(0);
}

void Sys_SendKeyEvents (void)
{
	//Key_Event (key_index + i, true);
}

extern AS3_Val flashSampleData;
extern int soundtime;
void S_Update_();

AS3_Val quakePaintSoundBuffer(void *data, AS3_Val args) 
{
	int tick;

	AS3_ArrayValue(args, "AS3ValType,IntType", &flashSampleData, &tick);
	soundtime += tick;
	S_Update_();

	return NULL;
}

FILE* flashOpenWriteFile(const char* filename)
{
	FILE* ret;
	AS3_Val byteArray;

	AS3_Val params = AS3_Array("AS3ValType", AS3_String(filename));
	
	byteArray = AS3_CallS("fileWriteSharedObject", flashQuakeObject, params);
	AS3_Release(params);

	//This opens a file for writing on a ByteArray, as explained in http://blog.debit.nl/?p=79
	//It does NOT reset its length to 0, so this must already have been done.
	ret = funopen((void *)byteArray, readByteArray, writeByteArray, seekByteArray, closeByteArray);
	
	return ret;
}

void flashCloseWriteFile(const char* filename)
{
	AS3_Val params = AS3_Array("AS3ValType", AS3_String(filename));

	AS3_CallS("fileUpdateSharedObject", flashQuakeObject, params);

	AS3_Release(params);
}

void IN_Move (usercmd_t *cmd)
{
	delta_mouse_x *= sensitivity.value;
	delta_mouse_y *= sensitivity.value;
   
	if ( (in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side.value * delta_mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * delta_mouse_x;
	if (in_mlook.state & 1)
		V_StopPitchDrift ();
   
	if ( (in_mlook.state & 1) && !(in_strafe.state & 1)) {
		cl.viewangles[PITCH] += m_pitch.value * delta_mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	} else {
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * delta_mouse_y;
		else
			cmd->forwardmove -= m_forward.value * delta_mouse_y;
	}
	delta_mouse_x = delta_mouse_y = 0.0;
}