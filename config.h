/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h> 	/* Extra media key */
#include "selfrestart.c" 	/* Restart Dwm without loggin out (bugged) */
#include "fibonacci.c"   	/* Spiral layout */
#include "push.c"		/* Relocate stacks around */
/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Hack:size=8" };
static const char dmenufont[]       = "Hack:size=9";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_pink[]  	    = "#fea6f5";
static const char col_baby_blue[]   = "#66f9fe";
static const char col_white[]	    = "#ffffff";
static const char col_yellow[]	    = "#e5e883";
static const char col_black[]	    = "#000000";
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_black, col_pink, col_pink },
	[SchemeSel]  = { col_black, col_baby_blue,  col_yellow  },
};

static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
/* static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" }; */
/* static const char *tags[] = { " ", " ", " ", " ", " ", "", "", "" }; */
static const char *tags[] = { "日", "月", "星", "辰", "太極" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 0,       0,           -1 },
	{ "st",       NULL,       NULL,       1 << 1,       0,           -1 },
	{ "mpv",      NULL,       NULL,       0, 	    1,           -1 },

};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "☯",      tile },    /* first entry is default */
	{ "",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
	{ "",      spiral },
	{ "[\\]",     dwindle },
	{ "NULL",     NULL },

};

/* key definitions */
// #define XF86AudioMute		0x1008ff12 //
// #define XF86AudioLowerVolume		0x1008ff11 //
// #define XF86AudioRaiseVolume		0x1008ff13 //
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_pink, "-nf", col_black, "-sb", col_baby_blue, "-sf", col_black, NULL };
/*static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL }; */
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34","-e", "tmux", NULL };
static const char *termcmd[]  = { "st", NULL };

//static const char *qutecmd[] = { "firejail", "qutebrowser", NULL };
static const char *slockcmd[] = { "slock", NULL };
static const char *surfcmd[] = { "tabbed", "-c", "surf", "-e", NULL };
static const char *volsupdown[] = { "amixer", "-c", "1", "-q", "sset", "Master", "5%-", NULL};
static const char *volsuperup[] = { "amixer", "-c", "1", "-q", "sset", "Master", "5%+", NULL};
// static const char *muteall[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL};
static const char *muteall[] = { "pulsemixer", "--toggle-mute", NULL};

static Key keys[] = {
	/* modifier                     key                        function        argument */
	{ 0,                            XF86XK_AudioMute,             spawn,         {.v = muteall } },
        { 0,                            XF86XK_AudioLowerVolume,      spawn,         {.v = volsuperup } },
        { 0,                            XF86XK_AudioRaiseVolume,      spawn,         {.v = volsupdown } },
	/* Super increase volume // increase volume */
	{ MODKEY|ShiftMask,             XK_k,  spawn,          {.v = volsuperup } },

	/* Super decrease volume // decrease volume */
	{ MODKEY|ShiftMask,             XK_j,  spawn,          {.v = volsupdown } },

	/* Mute sound with pactl */
	{ MODKEY|ShiftMask,             XK_m,      spawn,          {.v = muteall } },

	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
//	{ MODKEY,                       XK_w,      spawn,          {.v = qutecmd } },
	{ MODKEY|ShiftMask,             XK_x,      spawn,          {.v = slockcmd } },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          {.v = surfcmd } },

	/* Script Keys */
	{ MODKEY,                       XK_o,      spawn,	   SHCMD("linkpiper") },
	{ MODKEY,                       XK_O,      spawn,	   SHCMD("urlportal") },

	/* Layout Keys */
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY|ControlMask,           XK_j,      pushdown,       {0} },
	{ MODKEY|ControlMask,           XK_k,      pushup,         {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_v,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ShiftMask,             XK_b,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ControlMask,     	XK_comma,  cyclelayout,    {.i = -1 } }, 	
	{ MODKEY|ControlMask,     	XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
//    	{ MODKEY|ShiftMask,             XK_r,      self_restart,   {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
