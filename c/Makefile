PATH:=${ALCHEMY_HOME}/achacks:${PATH}

LDFLAGS = -lz
CFLAGS += -O3

CC = gcc
GASM = g++

DO_CC=$(CC) $(CFLAGS) $(BASE_CFLAGS) $(LDFLAGS) -swc -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

BUILDDIR=objects

all: flashquake

#############################################################################
# Flash Quake
#############################################################################

QF_OBJS = \
	$(BUILDDIR)/cl_demo.o \
	$(BUILDDIR)/cl_input.o \
	$(BUILDDIR)/cl_main.o \
	$(BUILDDIR)/cl_parse.o \
	$(BUILDDIR)/cl_tent.o \
	$(BUILDDIR)/cmd.o \
	$(BUILDDIR)/common.o \
	$(BUILDDIR)/console.o \
	$(BUILDDIR)/crc.o \
	$(BUILDDIR)/cvar.o \
	$(BUILDDIR)/draw.o \
	$(BUILDDIR)/d_edge.o \
	$(BUILDDIR)/d_fill.o \
	$(BUILDDIR)/d_init.o \
	$(BUILDDIR)/d_modech.o \
	$(BUILDDIR)/d_part.o \
	$(BUILDDIR)/d_polyse.o \
	$(BUILDDIR)/d_scan.o \
	$(BUILDDIR)/d_sky.o \
	$(BUILDDIR)/d_sprite.o \
	$(BUILDDIR)/d_surf.o \
	$(BUILDDIR)/d_vars.o \
	$(BUILDDIR)/d_zpoint.o \
	$(BUILDDIR)/host.o \
	$(BUILDDIR)/host_cmd.o \
	$(BUILDDIR)/keys.o \
	$(BUILDDIR)/menu.o \
	$(BUILDDIR)/mathlib.o \
	$(BUILDDIR)/model.o \
	$(BUILDDIR)/net_bsd.o \
	$(BUILDDIR)/net_dgrm.o \
	$(BUILDDIR)/net_loop.o \
	$(BUILDDIR)/net_main.o \
	$(BUILDDIR)/net_vcr.o \
	$(BUILDDIR)/nonintel.o \
	$(BUILDDIR)/pr_cmds.o \
	$(BUILDDIR)/pr_edict.o \
	$(BUILDDIR)/pr_exec.o \
	$(BUILDDIR)/r_aclip.o \
	$(BUILDDIR)/r_alias.o \
	$(BUILDDIR)/r_bsp.o \
	$(BUILDDIR)/r_light.o \
	$(BUILDDIR)/r_draw.o \
	$(BUILDDIR)/r_efrag.o \
	$(BUILDDIR)/r_edge.o \
	$(BUILDDIR)/r_misc.o \
	$(BUILDDIR)/r_main.o \
	$(BUILDDIR)/r_sky.o \
	$(BUILDDIR)/r_sprite.o \
	$(BUILDDIR)/r_surf.o \
	$(BUILDDIR)/r_part.o \
	$(BUILDDIR)/r_vars.o \
	$(BUILDDIR)/screen.o \
	$(BUILDDIR)/sbar.o \
	$(BUILDDIR)/sv_main.o \
	$(BUILDDIR)/sv_phys.o \
	$(BUILDDIR)/sv_move.o \
	$(BUILDDIR)/sv_user.o \
	$(BUILDDIR)/zone.o	\
	$(BUILDDIR)/view.o	\
	$(BUILDDIR)/wad.o \
	$(BUILDDIR)/world.o \
	$(BUILDDIR)/cd_flash.o \
	$(BUILDDIR)/sys_flash.o \
	$(BUILDDIR)/vid_flash.o \
	$(BUILDDIR)/in_flash.o \
	$(BUILDDIR)/snd_mem.o \
	$(BUILDDIR)/snd_mix.o \
	$(BUILDDIR)/snd_flash.o

flashquake : $(QF_OBJS)
	$(CC) $(CFLAGS) -swc -o swc/quake.swc $@ $(QF_OBJS) $(LDFLAGS)

$(BUILDDIR)/cl_demo.o :  cl_demo.c
	$(DO_CC)

$(BUILDDIR)/cl_input.o : cl_input.c
	$(DO_CC)

$(BUILDDIR)/cl_main.o : cl_main.c
	$(DO_CC)

$(BUILDDIR)/cl_parse.o : cl_parse.c
	$(DO_CC)

$(BUILDDIR)/cl_tent.o : cl_tent.c
	$(DO_CC)

$(BUILDDIR)/cmd.o :      cmd.c
	$(DO_CC)

$(BUILDDIR)/common.o :   common.c
	$(DO_CC)

$(BUILDDIR)/console.o :  console.c
	$(DO_CC)

$(BUILDDIR)/crc.o :      crc.c
	$(DO_CC)

$(BUILDDIR)/cvar.o :     cvar.c
	$(DO_CC)

$(BUILDDIR)/draw.o :     draw.c
	$(DO_CC)

$(BUILDDIR)/d_edge.o :   d_edge.c
	$(DO_CC)

$(BUILDDIR)/d_fill.o :   d_fill.c
	$(DO_CC)

$(BUILDDIR)/d_init.o :   d_init.c
	$(DO_CC)

$(BUILDDIR)/d_modech.o : d_modech.c
	$(DO_CC)

$(BUILDDIR)/d_part.o :   d_part.c
	$(DO_CC)

$(BUILDDIR)/d_polyse.o : d_polyse.c
	$(DO_CC)

$(BUILDDIR)/d_scan.o :   d_scan.c
	$(DO_CC)

$(BUILDDIR)/d_sky.o :    d_sky.c
	$(DO_CC)

$(BUILDDIR)/d_sprite.o : d_sprite.c
	$(DO_CC)

$(BUILDDIR)/d_surf.o :   d_surf.c
	$(DO_CC)

$(BUILDDIR)/d_vars.o :   d_vars.c
	$(DO_CC)

$(BUILDDIR)/d_zpoint.o : d_zpoint.c
	$(DO_CC)

$(BUILDDIR)/host.o :     host.c
	$(DO_CC)

$(BUILDDIR)/host_cmd.o : host_cmd.c
	$(DO_CC)

$(BUILDDIR)/keys.o :     keys.c
	$(DO_CC)

$(BUILDDIR)/menu.o :     menu.c
	$(DO_CC)

$(BUILDDIR)/mathlib.o :  mathlib.c
	$(DO_CC)

$(BUILDDIR)/model.o :    model.c
	$(DO_CC)

$(BUILDDIR)/net_bsd.o : net_bsd.c
	$(DO_CC)
	
$(BUILDDIR)/net_dgrm.o : net_dgrm.c
	$(DO_CC)

$(BUILDDIR)/net_loop.o : net_loop.c
	$(DO_CC)

$(BUILDDIR)/net_main.o : net_main.c
	$(DO_CC)

$(BUILDDIR)/net_vcr.o :  net_vcr.c
	$(DO_CC)

$(BUILDDIR)/nonintel.o : nonintel.c
	$(DO_CC)

$(BUILDDIR)/pr_cmds.o :  pr_cmds.c
	$(DO_CC)

$(BUILDDIR)/pr_edict.o : pr_edict.c
	$(DO_CC)

$(BUILDDIR)/pr_exec.o :  pr_exec.c
	$(DO_CC)

$(BUILDDIR)/r_aclip.o :  r_aclip.c
	$(DO_CC)

$(BUILDDIR)/r_alias.o :  r_alias.c
	$(DO_CC)

$(BUILDDIR)/r_bsp.o :    r_bsp.c
	$(DO_CC)

$(BUILDDIR)/r_light.o :  r_light.c
	$(DO_CC)

$(BUILDDIR)/r_draw.o :   r_draw.c
	$(DO_CC)

$(BUILDDIR)/r_efrag.o :  r_efrag.c
	$(DO_CC)

$(BUILDDIR)/r_edge.o :   r_edge.c
	$(DO_CC)

$(BUILDDIR)/r_misc.o :   r_misc.c
	$(DO_CC)

$(BUILDDIR)/r_main.o :   r_main.c
	$(DO_CC)

$(BUILDDIR)/r_sky.o :    r_sky.c
	$(DO_CC)

$(BUILDDIR)/r_sprite.o : r_sprite.c
	$(DO_CC)

$(BUILDDIR)/r_surf.o :   r_surf.c
	$(DO_CC)

$(BUILDDIR)/r_part.o :   r_part.c
	$(DO_CC)

$(BUILDDIR)/r_vars.o :   r_vars.c
	$(DO_CC)

$(BUILDDIR)/screen.o :   screen.c
	$(DO_CC)

$(BUILDDIR)/sbar.o :     sbar.c
	$(DO_CC)

$(BUILDDIR)/sv_main.o :  sv_main.c
	$(DO_CC)

$(BUILDDIR)/sv_phys.o :  sv_phys.c
	$(DO_CC)

$(BUILDDIR)/sv_move.o :  sv_move.c
	$(DO_CC)

$(BUILDDIR)/sv_user.o :  sv_user.c
	$(DO_CC)

$(BUILDDIR)/zone.o	:   zone.c
	$(DO_CC)

$(BUILDDIR)/view.o	:   view.c
	$(DO_CC)

$(BUILDDIR)/wad.o :      wad.c
	$(DO_CC)

$(BUILDDIR)/world.o :    world.c
	$(DO_CC)

$(BUILDDIR)/cd_flash.o : cd_flash.c
	$(DO_CC)

$(BUILDDIR)/sys_flash.o :sys_flash.c
	$(DO_CC)

$(BUILDDIR)/vid_flash.o: vid_flash.c
	$(DO_CC)

$(BUILDDIR)/in_flash.o: in_flash.c
	$(DO_CC)
	
$(BUILDDIR)/snd_mem.o :  snd_mem.c
	$(DO_CC)

$(BUILDDIR)/snd_mix.o :  snd_mix.c
	$(DO_CC)

$(BUILDDIR)/snd_flash.o :snd_flash.c
	$(DO_CC)

#############################################################################
# CLEANUP
#############################################################################

clean: -rm -f $(QF_OBJS)

