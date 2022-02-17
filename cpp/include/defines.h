#ifndef __DEFINES_H__
#define __DEFINES_H__

#ifndef VERSION
#define AUTHOR   "Diogo César Ferreira"
#define AUTHID   "11/0027931"
#define VERSION  "1.0"
#define VDATE    "17/02/2022"
#endif

#define EMPH_0 "m"
#define EMPH_1 ";1m"
#define ANSI_WHI "\x1B[0"
#define ANSI_RED "\x1B[31"
#define ANSI_GRE "\x1b[32"
#define ANSI_YEL "\x1b[33"
#define ANSI_BLU "\x1b[34"
#define ANSI_VIO "\x1b[35"
#define ANSI_CYA "\x1b[36"
#define CLR(clr, em, str) ANSI_##clr EMPH_##em str ANSI_WHI EMPH_0

#endif