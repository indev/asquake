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

#include "quakedef.h"

qboolean SNDDMA_Init(void)
{
	return true;
}

void SNDDMA_Shutdown(void)
{
}

/*
void S_StartSound (int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol,  float attenuation)
{
	
	AS3_Val qNS = AS3_String("se.quake");
	AS3_Val qClass = AS3_NSGetS(qNS, "QuakeSoundManager");
	AS3_Val qObj = AS3_New(qClass, AS3_Array("StrType", ""));
	AS3_Val startSound = AS3_GetS(qObj, "startSound");
	
	AS3_Call(startSound, qObj, AS3_Array(""));

	AS3_Release(startSound);
	AS3_Release(qObj);
	
	
	//flashQuakeObject
	
	printf("PlaySound: %s\n", sfx->name);
	
	AS3_CallS("testFunc", flashQuakeObject, AS3_Array( "StrType", AS3_String("test") ) );
	AS3_CallS("playQuakeSound", flashQuakeObject,AS3_Array( "StrType", "test" ) );
	
	AS3_CallTS("testFunc", flashQuakeObject, "AS3_Val",AS3_Array( "StrType", "test" ) );
	AS3_CallTS("playQuakeSound", flashQuakeObject, "AS3_Val", AS3_Array( "StrType", AS3_String("test") ) );
	
}*/
