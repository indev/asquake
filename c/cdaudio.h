
int CDAudio_Init(void);
void CDAudio_Play(byte track, qboolean looping);
void CDAudio_Stop(void);
void CDAudio_Resume(void);
void CDAudio_Shutdown(void);
byte CDAudio_GetVolume(void);
void CDAudio_SetVolume(byte volume);
void CDAudio_Update(void);
qboolean CDAudio_Playing(void);
