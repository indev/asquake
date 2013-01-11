// snd_mix.c -- portable code to mix sounds for snd_dma.c

#include "quakedef.h"
#include "sound.h"

#define	PAINTBUFFER_SIZE	512
#define PAINT_BUFFER_SCALE 32768.0f // Used when filling the flash sound buffer
	
portable_samplepair_t paintbuffer[PAINTBUFFER_SIZE];
int snd_scaletable[32][256];
int *snd_p, snd_linear_count, snd_vol;
short *snd_out;

AS3_Val flashSampleData;

void Snd_WriteLinearBlastStereo16(void);

void Snd_WriteLinearBlastStereo16(void)
{
    int i;
    int val;

    for (i = 0; i < snd_linear_count; i += 2) {
	val = (snd_p[i] * snd_vol) >> 8;
	if (val > 0x7fff)
	    snd_out[i] = 0x7fff;
	else if (val < (short) 0x8000)
	    snd_out[i] = (short) 0x8000;
	else
	    snd_out[i] = val;

	val = (snd_p[i + 1] * snd_vol) >> 8;
	if (val > 0x7fff)
	    snd_out[i + 1] = 0x7fff;
	else if (val < (short) 0x8000)
	    snd_out[i + 1] = (short) 0x8000;
	else
	    snd_out[i + 1] = val;
    }
}

void S_TransferStereo16(int endtime)
{
    int lpos;
    int lpaintedtime;

    snd_vol = volume.value * 256;

    snd_p = (int *) paintbuffer;
    lpaintedtime = paintedtime;

    while (lpaintedtime < endtime) {
	// handle recirculating buffer issues
	lpos = lpaintedtime & ((shm->samples >> 1) - 1);
	snd_out = (short *) shm->buffer + (lpos << 1);

	snd_linear_count = (shm->samples >> 1) - lpos;
	if (lpaintedtime + snd_linear_count > endtime)
	    snd_linear_count = endtime - lpaintedtime;

	snd_linear_count <<= 1;

	// write a linear blast of samples
	Snd_WriteLinearBlastStereo16();

	snd_p += snd_linear_count;
	lpaintedtime += (snd_linear_count >> 1);
    }
}

void S_TransferPaintBuffer(int endtime)
{
	//Transfer to the Flash sample data buffer
	float snddata[2*PAINTBUFFER_SIZE];
	int samples = (endtime - paintedtime);// * shm->channels; //Number of STEREO samples
	float f;
	int i;
	int n = 0;
	
	for(i = 0; i < samples; i++)
	{
		f = (float)paintbuffer[i].left / PAINT_BUFFER_SCALE;
		f = BigFloat(f);
		snddata[n++] = f;
		f = (float)paintbuffer[i].right / PAINT_BUFFER_SCALE;
		f = BigFloat(f);
		snddata[n++] = f;
	}
	
	AS3_ByteArray_writeBytes(flashSampleData, snddata, 2*samples*sizeof(float));
/*
    int out_idx;
    int count;
    int out_mask;
    int *p;
    int step;
    int val;
    int snd_vol;

    if (shm->samplebits == 16 && shm->channels == 2) {
		S_TransferStereo16(endtime);
		return;
	}

    p = (int *) paintbuffer;
    count = (endtime - paintedtime) * shm->channels;
    out_mask = shm->samples - 1;
    out_idx = paintedtime * shm->channels & out_mask;
    step = 3 - shm->channels;
    snd_vol = volume.value * 256;

    if (shm->samplebits == 16) {
	short *out = (short *) shm->buffer;
	while (count--) {
	    val = (*p * snd_vol) >> 8;
	    p += step;
	    if (val > 0x7fff)
		val = 0x7fff;
	    else if (val < (short) 0x8000)
		val = (short) 0x8000;
	    out[out_idx] = val;
	    out_idx = (out_idx + 1) & out_mask;
	}
    } else if (shm->samplebits == 8) {
	unsigned char *out = (unsigned char *) shm->buffer;
	while (count--) {
	    val = (*p * snd_vol) >> 8;
	    p += step;
	    if (val > 0x7fff)
		val = 0x7fff;
	    else if (val < (short) 0x8000)
		val = (short) 0x8000;
	    out[out_idx] = (val >> 8) + 128;
	    out_idx = (out_idx + 1) & out_mask;
	}
    }
*/
}

/*
===============================================================================

NOISE TRACK

===============================================================================
*/

#define	BGM_BUFFER	4096
typedef struct {
    int handle;
    wavinfo_t info;
    int painted;		// in shm->speed samples
    int rawread;		// in bytes     
    int rawtotal;		// in bytes     
    byte raw[BGM_BUFFER];
} bgm_t;

bgm_t bgm;

void S_StartNoiseTrack(char *name)
{
    byte buffer[8192];

    if (bgm.handle)
	Sys_FileClose(bgm.handle);
    memset(&bgm, 0, sizeof(bgm));

    COM_OpenFile(name, &bgm.handle);
    if (bgm.handle == -1) {
	Con_Printf("Couldn't open %s\n", name);
	return;
    }

    Sys_FileRead(bgm.handle, buffer, sizeof(buffer));
    bgm.info = GetWavinfo(name, buffer, sizeof(buffer));

    if (!bgm.info.samples) {
	Con_Printf("Couldn't parse %s\n", name);
	return;
    }

    bgm.rawtotal = bgm.info.width * bgm.info.samples;
    bgm.rawread = sizeof(bgm.raw);
    Sys_FileSeek(bgm.handle, bgm.info.dataofs);
    Sys_FileRead(bgm.handle, bgm.raw, sizeof(bgm.raw));
}

void S_NoiseTrack(int samples)
{
    int vol;
    portable_samplepair_t *p;
    int want, count;
    int i;
    int samplebytes;
    int srcsample;
    int sample;
    int ratescale;

    vol = bgmvolume.value * 255;

    if (!vol || !bgm.info.samples) {
	Q_memset(paintbuffer, 0, samples * sizeof(portable_samplepair_t));
	return;
    }
// resample directly into the paint buffer      
    samplebytes = bgm.info.width * bgm.info.channels;
    ratescale = bgm.info.rate * 256 / shm->speed;	// 512 == skip every other samp

    p = paintbuffer;

    while (samples) {
	count =
	    (double) bgm.rawread * 256 / (samplebytes * ratescale) -
	    bgm.painted;

	if (!count) {		// read another block of raw samples
	    if (bgm.rawread == bgm.rawtotal) {
//                      Con_Printf ("loop\n");
		Sys_FileSeek(bgm.handle, bgm.info.dataofs);
		bgm.rawread = 0;
		bgm.painted = 0;
	    }

	    want = sizeof(bgm.raw);
	    if (bgm.rawread + want > bgm.rawtotal)
		want = bgm.rawtotal - bgm.rawread;
	    count = Sys_FileRead(bgm.handle, bgm.raw, want);
	    if (count != want)
		Sys_Error("Read failure on noise track");
	    bgm.rawread += want;

	    count =
		(double) bgm.rawread * 256 / (samplebytes * ratescale) -
		bgm.painted;
	}

	if (count > samples)
	    count = samples;

	if (count <= 0)
	    Sys_Error("Nosietrack count = %i\n", count);

	for (i = 0; i < count; i++) {
	    srcsample = ((bgm.painted + i) * ratescale) >> 8;
	    srcsample = (srcsample * samplebytes) & (BGM_BUFFER - 1);

	    if (bgm.info.width == 2)
		sample = LittleShort(*((short *) (&bgm.raw[srcsample])));
	    else
		sample =
		    (int) ((unsigned char) (bgm.raw[srcsample]) -
			   128) << 8;

	    p->left = (sample * vol) >> 8;

	    if (bgm.info.channels == 2) {
		if (bgm.info.width == 2)
		    sample =
			LittleShort(*
				    ((short *) (&bgm.raw[srcsample + 2])));
		else
		    sample =
			(int) ((unsigned char) (bgm.raw[srcsample + 1]) -
			       128) << 8;
		p->right = (sample * vol) >> 8;
	    } else
		p->right = p->left;
	    p++;
	}

	bgm.painted += count;
	samples -= count;
    }

}


/*
===============================================================================

CHANNEL MIXING

===============================================================================
*/

void SND_PaintChannelFrom8(channel_t * ch, sfxcache_t * sc, int endtime);
void SND_PaintChannelFrom16(channel_t * ch, sfxcache_t * sc, int endtime);

void S_PaintChannels(int endtime)
{
	//printf("S_PaintChannels\n");

    int i;
    int end;
    channel_t *ch;
    sfxcache_t *sc;
    int ltime, count;

    while (paintedtime < endtime) {
	// if paintbuffer is smaller than DMA buffer
	end = endtime;
	if (endtime - paintedtime > PAINTBUFFER_SIZE)
	    end = paintedtime + PAINTBUFFER_SIZE;

	// clear the paint buffer
	S_NoiseTrack(end - paintedtime);

	// paint in the channels.
	ch = channels;
	for (i = 0; i < total_channels; i++, ch++) {
	    if (!ch->sfx)
		continue;
	    if (!ch->leftvol && !ch->rightvol)
		continue;
	    sc = S_LoadSound(ch->sfx);
	    if (!sc)
		continue;

	    ltime = paintedtime;

	    while (ltime < end) {	// paint up to end
		if (ch->end < end)
		    count = ch->end - ltime;
		else
		    count = end - ltime;

		if (count > 0) {
		    if (sc->width == 1)
			SND_PaintChannelFrom8(ch, sc, count);
		    else
			SND_PaintChannelFrom16(ch, sc, count);

		    ltime += count;
		}
		// if at end of loop, restart
		if (ltime >= ch->end) {
		    if (sc->loopstart >= 0) {
			ch->pos = sc->loopstart;
			ch->end = ltime + sc->length - ch->pos;
		    } else {	// channel just stopped
			ch->sfx = NULL;
			break;
		    }
		}
	    }

	}

	// transfer out according to DMA format
	S_TransferPaintBuffer(end);
	paintedtime = end;
    }

}

void SND_InitScaletable(void)
{
    int i, j;

    for (i = 0; i < 32; i++)
	for (j = 0; j < 256; j++)
	    snd_scaletable[i][j] = ((signed char) j) * i * 8;
}


void SND_PaintChannelFrom8(channel_t * ch, sfxcache_t * sc, int count)
{
    int data;
    int *lscale, *rscale;
    unsigned char *sfx;
    int i;

    if (ch->leftvol > 255)
	ch->leftvol = 255;
    if (ch->rightvol > 255)
	ch->rightvol = 255;

    lscale = snd_scaletable[ch->leftvol >> 3];
    rscale = snd_scaletable[ch->rightvol >> 3];
    sfx = (unsigned char *) sc->data + ch->pos;

    for (i = 0; i < count; i++) {
	data = sfx[i];
	paintbuffer[i].left += lscale[data];
	paintbuffer[i].right += rscale[data];
    }

    ch->pos += count;
}

void SND_PaintChannelFrom16(channel_t * ch, sfxcache_t * sc, int count)
{
    int data;
    int left, right;
    int leftvol, rightvol;
    signed short *sfx;
    int i;

    leftvol = ch->leftvol;
    rightvol = ch->rightvol;
    sfx = (signed short *) sc->data + ch->pos;

    for (i = 0; i < count; i++) {
	data = sfx[i];
	left = (data * leftvol) >> 8;
	right = (data * rightvol) >> 8;
	paintbuffer[i].left += left;
	paintbuffer[i].right += right;
    }

    ch->pos += count;
}
