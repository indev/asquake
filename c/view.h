// view.h

extern cvar_t v_gamma;

void V_Init(void);
void V_RenderView(void);
float V_CalcRoll(vec3_t angles, vec3_t velocity);
void V_UpdatePalette(void);
