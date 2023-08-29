/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 5;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "xft:Bitstream Vera Sans Mono:size=12:bold:antialias=true" };
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */

static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_red[]         = "#7C0D0E";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_gray1,  col_red  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
        { "fileman",  NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
/* Media Controls */
static const char *raisevolcmd[] = {"/bin/sh", "-c", "amixer sset Master 5%+; kill -44 $(pidof dwmblocks)"};
static const char *lowervolcmd[] = {"/bin/sh", "-c", "amixer sset Master 5%-; kill -44 $(pidof dwmblocks)"};
static const char *mutecmd[]     = {"/bin/sh", "-c", "amixer sset Master toggle; kill -44 $(pidof dwmblocks)"};
static const char *nextmediacmd[] = {"playerctl", "--all-players", "next", NULL};
static const char *prevmediacmd[] = {"playerctl", "--all-players", "prev", NULL};
static const char *playmediacmd[] = {"playerctl", "--all-players", "play-pause", NULL};

/* Brightness Controls */
static const char *downbrightcmd[] = {"xbacklight", "-dec", "10", NULL};
static const char *upbrightcmd[] = {"xbacklight", "-inc", "10", NULL};

/* Power options */
static const char *choosepower[] = {"poweroptsdmenu", NULL};

/* Program Startup Shortcuts */
#define termcols "window.dimensions.columns=200"
#define termrows "window.dimensions.lines=30"
static const char *startfileman[] = {"alacritty", "--title", "fileman", "--class", "fileman", "-o", termcols, termrows, "-e", "lfrun", NULL};
static const char *startmail[] = {"alacritty", "--title", "mailclient", "--class", "mailclient", "-o", termcols, termrows, "-e", "neomutt", NULL};

static const char *startbrowser[] = {"firefox", "-p", NULL};
static const char *startemacs[] = {"emacsclient", "-c", "-a", "",  NULL};

/* Screen Shot */
static const char *screenshotcmd[] = {"scrot-select", NULL};


static const Key keys[] = {
	/* modifier                     key        function        argument */
    	{ MODKEY, 			XK_e,	  				spawn,	   	{.v = startfileman } },
	{ MODKEY|ShiftMask,		XK_e,	  				spawn,	   	{.v = startmail } },
	{ MODKEY|ShiftMask,		XK_s,					spawn,		{.v = screenshotcmd } },
	{ MODKEY,			XK_b,					spawn,		{.v = startbrowser } },
	{ MODKEY|ShiftMask,             XK_b,     				togglebar,      {0} },
	{ MODKEY,                       XK_n,                                   spawn,          {.v = startemacs } },
	{ 0,				XF86XK_AudioRaiseVolume,		spawn,		{.v = raisevolcmd } },
	{ 0,				XF86XK_AudioLowerVolume,		spawn,		{.v = lowervolcmd } },
	{ 0,				XF86XK_AudioMute,			spawn,		{.v = mutecmd } },
	{ 0,				XF86XK_MonBrightnessDown,		spawn,		{.v = downbrightcmd } },
	{ 0,				XF86XK_MonBrightnessUp,			spawn,		{.v = upbrightcmd } },
	{ 0,				XF86XK_AudioNext,			spawn,		{.v = nextmediacmd } },
	{ 0, 				XF86XK_AudioPrev,			spawn,		{.v = prevmediacmd } },
	{ 0,				XF86XK_AudioPlay,			spawn,		{.v = playmediacmd } },
	{ 0,				XK_F12,					spawn, 		{.v = choosepower } },
	{ MODKEY,	                XK_F11,     				setlayout,      {.v = &layouts[2]} },
	{ 0,                       	XK_F7,      				view,           {.ui = 1 << 7} },
	{ 0,                       	XK_F1,      				view,           {.ui = 1 << 0} }, \
	{ 0,                       	XK_F2,      				view,           {.ui = 1 << 1} }, \
	{ 0,                       	XK_F3,      				view,           {.ui = 1 << 2} }, \
	{ 0,                            XK_F4,   				view,           {0} },
        { MODKEY,                            XK_KP_Home,   spawn,          {.v = lowervolcmd}},
        { MODKEY,                            XK_KP_Prior,   spawn,          {.v = raisevolcmd}},
        { MODKEY,                            XK_KP_Up,   spawn,          {.v = mutecmd}},
        { MODKEY,                            XK_KP_Left,   spawn,          {.v = prevmediacmd}},
        { MODKEY,                            XK_KP_Right,   spawn,          {.v = nextmediacmd}},
        { MODKEY,                            XK_KP_Begin,   spawn,          {.v = playmediacmd}},

	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ControlMask,		XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_Down,   moveresize,     {.v = "0x 25y 0w 0h" } },
	{ MODKEY,                       XK_Up,     moveresize,     {.v = "0x -25y 0w 0h" } },
	{ MODKEY,                       XK_Right,  moveresize,     {.v = "25x 0y 0w 0h" } },
	{ MODKEY,                       XK_Left,   moveresize,     {.v = "-25x 0y 0w 0h" } },
	{ MODKEY|ShiftMask,             XK_Down,   moveresize,     {.v = "0x 0y 0w 25h" } },
	{ MODKEY|ShiftMask,             XK_Up,     moveresize,     {.v = "0x 0y 0w -25h" } },
	{ MODKEY|ShiftMask,             XK_Right,  moveresize,     {.v = "0x 0y 25w 0h" } },
	{ MODKEY|ShiftMask,             XK_Left,   moveresize,     {.v = "0x 0y -25w 0h" } },
	{ MODKEY|ControlMask,           XK_Up,     moveresizeedge, {.v = "t"} },
	{ MODKEY|ControlMask,           XK_Down,   moveresizeedge, {.v = "b"} },
	{ MODKEY|ControlMask,           XK_Left,   moveresizeedge, {.v = "l"} },
	{ MODKEY|ControlMask,           XK_Right,  moveresizeedge, {.v = "r"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Up,     moveresizeedge, {.v = "T"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Down,   moveresizeedge, {.v = "B"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Left,   moveresizeedge, {.v = "L"} },
	{ MODKEY|ControlMask|ShiftMask, XK_Right,  moveresizeedge, {.v = "R"} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
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
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
        { ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} },
        { ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

