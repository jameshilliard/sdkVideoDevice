#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>  
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>  
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <pthread.h>

#include <asm/types.h>
#include <arpa/inet.h>

#include <sys/vfs.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h> 

#include <netdb.h>  
#include <net/if.h>
#include <netinet/in.h>
#include <net/route.h>
#include <net/if_arp.h>

#include <linux/fs.h>
#include <linux/sockios.h>   
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <netinet/ip.h>   
#include <netinet/ether.h>
#include <netinet/ip_icmp.h>  
#include "../LogOut/LogOut.h"
#include "joseph_g711a_h264_to_mp4.h"

#if 0
extern Queue * g_videoQuene;
#endif

/********************************************************g711a encode decode**********************************************/
static const int16_t alawtos16[256] =
{
     -5504,  -5248,  -6016,  -5760,  -4480,  -4224,  -4992,  -4736,
     -7552,  -7296,  -8064,  -7808,  -6528,  -6272,  -7040,  -6784,
     -2752,  -2624,  -3008,  -2880,  -2240,  -2112,  -2496,  -2368,
     -3776,  -3648,  -4032,  -3904,  -3264,  -3136,  -3520,  -3392,
    -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944,
    -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136,
    -11008, -10496, -12032, -11520,  -8960,  -8448,  -9984,  -9472,
    -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568,
      -344,   -328,   -376,   -360,   -280,   -264,   -312,   -296,
      -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424,
       -88,    -72,   -120,   -104,    -24,     -8,    -56,    -40,
      -216,   -200,   -248,   -232,   -152,   -136,   -184,   -168,
     -1376,  -1312,  -1504,  -1440,  -1120,  -1056,  -1248,  -1184,
     -1888,  -1824,  -2016,  -1952,  -1632,  -1568,  -1760,  -1696,
      -688,   -656,   -752,   -720,   -560,   -528,   -624,   -592,
      -944,   -912,  -1008,   -976,   -816,   -784,   -880,   -848,
      5504,   5248,   6016,   5760,   4480,   4224,   4992,   4736,
      7552,   7296,   8064,   7808,   6528,   6272,   7040,   6784,
      2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368,
      3776,   3648,   4032,   3904,   3264,   3136,   3520,   3392,
     22016,  20992,  24064,  23040,  17920,  16896,  19968,  18944,
     30208,  29184,  32256,  31232,  26112,  25088,  28160,  27136,
     11008,  10496,  12032,  11520,   8960,   8448,   9984,   9472,
     15104,  14592,  16128,  15616,  13056,  12544,  14080,  13568,
       344,    328,    376,    360,    280,    264,    312,    296,
       472,    456,    504,    488,    408,    392,    440,    424,
        88,     72,    120,    104,     24,      8,     56,     40,
       216,    200,    248,    232,    152,    136,    184,    168,
      1376,   1312,   1504,   1440,   1120,   1056,   1248,   1184,
      1888,   1824,   2016,   1952,   1632,   1568,   1760,   1696,
       688,    656,    752,    720,    560,    528,    624,    592,
       944,    912,   1008,    976,    816,    784,    880,    848
};

static const int8_t alaw_encode[2049] =
{
    0xD5, 0xD4, 0xD7, 0xD6, 0xD1, 0xD0, 0xD3, 0xD2, 0xDD, 0xDC, 0xDF, 0xDE,
    0xD9, 0xD8, 0xDB, 0xDA, 0xC5, 0xC4, 0xC7, 0xC6, 0xC1, 0xC0, 0xC3, 0xC2,
    0xCD, 0xCC, 0xCF, 0xCE, 0xC9, 0xC8, 0xCB, 0xCA, 0xF5, 0xF5, 0xF4, 0xF4,
    0xF7, 0xF7, 0xF6, 0xF6, 0xF1, 0xF1, 0xF0, 0xF0, 0xF3, 0xF3, 0xF2, 0xF2,
    0xFD, 0xFD, 0xFC, 0xFC, 0xFF, 0xFF, 0xFE, 0xFE, 0xF9, 0xF9, 0xF8, 0xF8,
    0xFB, 0xFB, 0xFA, 0xFA, 0xE5, 0xE5, 0xE5, 0xE5, 0xE4, 0xE4, 0xE4, 0xE4,
    0xE7, 0xE7, 0xE7, 0xE7, 0xE6, 0xE6, 0xE6, 0xE6, 0xE1, 0xE1, 0xE1, 0xE1,
    0xE0, 0xE0, 0xE0, 0xE0, 0xE3, 0xE3, 0xE3, 0xE3, 0xE2, 0xE2, 0xE2, 0xE2,
    0xED, 0xED, 0xED, 0xED, 0xEC, 0xEC, 0xEC, 0xEC, 0xEF, 0xEF, 0xEF, 0xEF,
    0xEE, 0xEE, 0xEE, 0xEE, 0xE9, 0xE9, 0xE9, 0xE9, 0xE8, 0xE8, 0xE8, 0xE8,
    0xEB, 0xEB, 0xEB, 0xEB, 0xEA, 0xEA, 0xEA, 0xEA, 0x95, 0x95, 0x95, 0x95,
    0x95, 0x95, 0x95, 0x95, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94,
    0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0x96, 0x96, 0x96, 0x96,
    0x96, 0x96, 0x96, 0x96, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x93, 0x93, 0x93, 0x93,
    0x93, 0x93, 0x93, 0x93, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92, 0x92,
    0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9D, 0x9C, 0x9C, 0x9C, 0x9C,
    0x9C, 0x9C, 0x9C, 0x9C, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F, 0x9F,
    0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x9E, 0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
    0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9B, 0x9A, 0x9A, 0x9A, 0x9A,
    0x9A, 0x9A, 0x9A, 0x9A, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,
    0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85, 0x84, 0x84, 0x84, 0x84,
    0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84,
    0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87,
    0x87, 0x87, 0x87, 0x87, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86,
    0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83,
    0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D,
    0x8D, 0x8D, 0x8D, 0x8D, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C,
    0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8C, 0x8F, 0x8F, 0x8F, 0x8F,
    0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F,
    0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E, 0x8E,
    0x8E, 0x8E, 0x8E, 0x8E, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
    0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
    0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B,
    0x8B, 0x8B, 0x8B, 0x8B, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A,
    0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0x8A, 0xB5, 0xB5, 0xB5, 0xB5,
    0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
    0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5, 0xB5,
    0xB5, 0xB5, 0xB5, 0xB5, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
    0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
    0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
    0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7,
    0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7,
    0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB6, 0xB6, 0xB6, 0xB6,
    0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
    0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6, 0xB6,
    0xB6, 0xB6, 0xB6, 0xB6, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
    0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
    0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1,
    0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0,
    0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0,
    0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB0, 0xB3, 0xB3, 0xB3, 0xB3,
    0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
    0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3, 0xB3,
    0xB3, 0xB3, 0xB3, 0xB3, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
    0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
    0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2, 0xB2,
    0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD,
    0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD,
    0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBD, 0xBC, 0xBC, 0xBC, 0xBC,
    0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC,
    0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC, 0xBC,
    0xBC, 0xBC, 0xBC, 0xBC, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
    0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
    0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
    0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE,
    0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE,
    0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xBE, 0xB9, 0xB9, 0xB9, 0xB9,
    0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
    0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
    0xB9, 0xB9, 0xB9, 0xB9, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
    0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
    0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
    0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBA, 0xBA, 0xBA, 0xBA,
    0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA,
    0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA, 0xBA,
    0xBA, 0xBA, 0xBA, 0xBA, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
    0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
    0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
    0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
    0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5,
    0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA4, 0xA4, 0xA4, 0xA4,
    0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
    0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
    0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
    0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
    0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4, 0xA4,
    0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
    0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
    0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
    0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
    0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7, 0xA7,
    0xA7, 0xA7, 0xA7, 0xA7, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
    0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
    0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
    0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
    0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
    0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA1, 0xA1, 0xA1, 0xA1,
    0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
    0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
    0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
    0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
    0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0,
    0xA0, 0xA0, 0xA0, 0xA0, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
    0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
    0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
    0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
    0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
    0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA2, 0xA2, 0xA2, 0xA2,
    0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
    0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
    0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
    0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
    0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2, 0xA2,
    0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
    0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
    0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
    0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
    0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD,
    0xAD, 0xAD, 0xAD, 0xAD, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
    0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
    0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
    0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
    0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
    0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAF, 0xAF, 0xAF, 0xAF,
    0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
    0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
    0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
    0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
    0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF, 0xAF,
    0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
    0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
    0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
    0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
    0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE, 0xAE,
    0xAE, 0xAE, 0xAE, 0xAE, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
    0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
    0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
    0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
    0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9,
    0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA9, 0xA8, 0xA8, 0xA8, 0xA8,
    0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
    0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
    0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
    0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
    0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8, 0xA8,
    0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
    0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
    0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
    0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
    0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB,
    0xAB, 0xAB, 0xAB, 0xAB, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x2A
}; 

static int g711Length=0;
static int pcmAver=0;

int g711a_decode(void *pout_buf, int *pout_len, const void *pin_buf, const int  in_len)
{
    int16_t *dst = (int16_t *)pout_buf;
	uint8_t *dst8 = (uint8_t *)pin_buf;
    uint8_t *src = (uint8_t *)pin_buf;
    uint32_t i = 0;
	int Ret = 0;
	int16_t temp=0;
    if ( (NULL == pout_buf)
        || (NULL == pout_len)
        || (NULL == pin_buf)
        || (0 == in_len) )
    {
        return -1;
    }
    
    if ( *pout_len < 2 * in_len )
    {
        return -2;
    }
    
    for( i = 0; i < in_len; i++ )
    {
        temp = alawtos16[*(src++)];
		*(dst++)=temp; 
		//*(dst8++)=temp;
		//*(dst8++)=temp>>8;
    }
    
	Ret = 2 * in_len;
	
    return Ret;
}

int g711a_encode(void *pout_buf, int *pout_len, const void *pin_buf, const int in_len)
{
    int8_t *dst = (int8_t *)pout_buf;
    int16_t *src = (int16_t *)pin_buf;
    uint32_t i = 0;
    int Ret = 0;
	
    if ( (NULL == pout_buf)
        || (NULL == pout_len)
        || (NULL == pin_buf)
        || (0 == in_len) )
    {
        return -1;
    }
    
    if ( *pout_len < in_len / 2 )
    {
        return -2;
    }
    
    for( i = 0; i < in_len / 2; i++ )
    {
        int16_t s = *(src++);
        if( s >= 0)
        {
            *(dst++) = alaw_encode[s / 16];
        }
        else
        {
            *(dst++) = 0x7F & alaw_encode[s / -16];
        }
    }
    
	Ret = in_len / 2;
    
	return Ret;
}

int joseph_get_g711a_frame(JOSEPH_MP4_CONFIG* joseph_mp4_config, unsigned char *nVideoBuffer, int nVideoRet)
{		
	int nVideoSize = 0;
	char nVideoNum[32] = {0};
	memset(nVideoNum, 0, 32);
	sprintf(nVideoNum, "%s%d", JOSEPH_H264_LOCALTION, nVideoRet);
	joseph_mp4_config->fpInVideo = fopen(nVideoNum, "rb");
	memset(nVideoBuffer, 0, 1048576);
	nVideoSize = fread(nVideoBuffer, 1, 1048576, joseph_mp4_config->fpInVideo);

	fclose(joseph_mp4_config->fpInVideo);
	
	return nVideoSize;
}


int CloseMp4Encoder(JOSEPH_MP4_CONFIG* joseph_mp4_config)
{
	if(joseph_mp4_config==NULL)
		return -1;
	if(joseph_mp4_config->hFile)
	{  
		MP4Close(joseph_mp4_config->hFile, 0);  
		joseph_mp4_config->hFile = NULL;  
	}
	joseph_mp4_config->valid=0;
	return 0;
}

/***********************************************************mp4 encode***********************************************/
int InitMp4Encoder(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config)
{
	if(joseph_aac_config==NULL || joseph_mp4_config==NULL)
		return -1;
	if(joseph_mp4_config->valid==1)
	{
		CloseMp4Encoder(joseph_mp4_config);
	}
	//joseph_mp4_config =(JOSEPH_MP4_CONFIG *)malloc(sizeof(JOSEPH_MP4_CONFIG));
	//joseph_mp4_config->m_vFrameDur = 0;
	//joseph_mp4_config->video = MP4_INVALID_TRACK_ID;	
	//joseph_mp4_config->audio = MP4_INVALID_TRACK_ID;
	//joseph_mp4_config->hFile = NULL;
	//joseph_mp4_config->timeScale = 90000;	
	//joseph_mp4_config->fps = 25;              
	//joseph_mp4_config->width = 1280;          
	//joseph_mp4_config->height = 960; 
	//joseph_aac_config->nSampleRate = 8000;
	//joseph_aac_config->nChannels = 1;
	//joseph_aac_config->nPCMBitSize = 16;
	//joseph_aac_config->nInputSamples = 0;
	//joseph_aac_config->nMaxOutputBytes = 0;
	
	/*file handle*/
	//joseph_mp4_config->hFile = MP4Create(JOSEPH_MP4_FILE, 0);
	joseph_mp4_config->hFile = MP4Create(joseph_mp4_config->nFifoName, 0);
	
	if(joseph_mp4_config->hFile == MP4_INVALID_FILE_HANDLE)
	{
        LOGOUT("open file fialed.\n");
        return -1;
    }
	MP4SetTimeScale(joseph_mp4_config->hFile, joseph_mp4_config->timeScale);    //timeScale
	
	/*audio track*/
	joseph_mp4_config->audio = MP4AddAudioTrack(joseph_mp4_config->hFile, joseph_aac_config->nSampleRate, \
													joseph_aac_config->nInputSamples, MP4_MPEG4_AUDIO_TYPE);
	if(joseph_mp4_config->audio == MP4_INVALID_TRACK_ID)
    {
        printf("add audio track failed.\n");
        return -2;
    }
	
	MP4SetAudioProfileLevel(joseph_mp4_config->hFile, 0x2);
	unsigned char aacConfig[2] = {0x15, 0x88};                                               // 0001 0101 1000 1000
	MP4SetTrackESConfiguration(joseph_mp4_config->hFile, joseph_mp4_config->audio, aacConfig, 2);
	joseph_mp4_config->valid=1;
	return 0;
}

//------------------------------------------------------------------------------------------------- Mp4Encode说明
// 【h264编码出的NALU规律】
// 第一帧 SPS【0 0 0 1 0x67】 PPS【0 0 0 1 0x68】 SEI【0 0 0 1 0x6】 IDR【0 0 0 1 0x65】
// p帧      P【0 0 0 1 0x61】
// I帧    SPS【0 0 0 1 0x67】 PPS【0 0 0 1 0x68】 IDR【0 0 0 1 0x65】
// 【mp4v2封装函数MP4WriteSample】
// 此函数接收I/P nalu,该nalu需要用4字节的数据大小头替换原有的起始头，并且数据大小为big-endian格式
//-------------------------------------------------------------------------------------------------
int Mp4VEncode(JOSEPH_MP4_CONFIG* joseph_mp4_config, unsigned char* naluData, int naluSize)
{
	int index = -1;
	
	if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x67)
	{
		index = NALU_SPS;
		//printf("%s[%d]====NALU_SPS\n",__FUNCTION__,__LINE__);
	}
	if(index!=NALU_SPS && joseph_mp4_config->video == MP4_INVALID_TRACK_ID)
	{
		//printf("index!=NALU_SPS && joseph_mp4_config->video == MP4_INVALID_TRACK_ID");
		return -1;
	}
	if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x68)
	{
		index = NALU_PPS;
		//printf("%s[%d]====NALU_PPS\n",__FUNCTION__,__LINE__);
	}
	if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x65)
	{
		index = NALU_I;
		LOGOUT("%s[%d]====NALU_I",__FUNCTION__,__LINE__);
	}
	if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x61)
	{
		index = NALU_P;
		//printf("%s[%d]====NALU_P\n",__FUNCTION__,__LINE__);
	}
	if(naluData[0]==0 && naluData[1]==0 && naluData[2]==0 && naluData[3]==1 && naluData[4]==0x6)
	{
		index = NALU_SET;
		//printf("%s[%d]====NALU_SET\n",__FUNCTION__,__LINE__);
	}
	
	switch(index)
	{
		case NALU_SPS:         
			if(joseph_mp4_config->video == MP4_INVALID_TRACK_ID)
			{
				joseph_mp4_config->video = MP4AddH264VideoTrack  
					(joseph_mp4_config->hFile,   
					joseph_mp4_config->timeScale,                               //timeScale
					(joseph_mp4_config->timeScale / joseph_mp4_config->fps),    //sampleDuration    timeScale/fps
					joseph_mp4_config->width,     								// width  
					joseph_mp4_config->height,    								// height  
					naluData[5], 												// sps[1] AVCProfileIndication  
					naluData[6], 												// sps[2] profile_compat  
					naluData[7], 												// sps[3] AVCLevelIndication  
					3);           												// 4 bytes length before each NAL unit  
				if(joseph_mp4_config->video == MP4_INVALID_TRACK_ID)
				{
					printf("add video track failed.\n");
					return -1;
				}
				//MP4SetVideoProfileLevel(joseph_mp4_config->hFile, 1); //  Simple Profile @ Level 3   mp4编码标准
				MP4SetVideoProfileLevel(joseph_mp4_config->hFile, 0x7F); //  Simple Profile @ Level 3
			}  
			MP4AddH264SequenceParameterSet(joseph_mp4_config->hFile, joseph_mp4_config->video, naluData+4, naluSize-4);  
			break;
		case NALU_PPS:  
			MP4AddH264PictureParameterSet(joseph_mp4_config->hFile, joseph_mp4_config->video, naluData+4, naluSize-4);  
			break;
		case NALU_SET:  
			MP4AddH264PictureParameterSet(joseph_mp4_config->hFile, joseph_mp4_config->video, naluData+4, naluSize-4);  
			break;
		case NALU_I: 
			{
				unsigned char* IFrameData = (unsigned char*)malloc((naluSize)* sizeof(unsigned char));
				
				IFrameData[0] = (naluSize-4) >>24;  
				IFrameData[1] = (naluSize-4) >>16;  
				IFrameData[2] = (naluSize-4) >>8;  
				IFrameData[3] = (naluSize-4) &0xff;  
				
				memcpy(IFrameData+4, naluData+4, naluSize-4);
				//if(!MP4WriteSample(joseph_mp4_config->hFile, joseph_mp4_config->video, IFrameData, naluSize+1, m_vFrameDur/8000*90000, 0, 1))
				if(!MP4WriteSample(joseph_mp4_config->hFile, joseph_mp4_config->video, IFrameData, naluSize, MP4_INVALID_DURATION, 0, 1))
				{  
					return -1;  
				}  
				
				//joseph_mp4_config->m_vFrameDur = 0;
				free(IFrameData); 
				IFrameData = NULL;
				
				break;
			}
		case NALU_P:
			{
				naluData[0] = (naluSize-4) >>24;  
				naluData[1] = (naluSize-4) >>16;  
				naluData[2] = (naluSize-4) >>8;  
				naluData[3] = (naluSize-4) &0xff; 
				
				//if(!MP4WriteSample(joseph_mp4_config->hFile, joseph_mp4_config->video, naluData, naluSize, m_vFrameDur/8000*90000, 0, 1))
				if(!MP4WriteSample(joseph_mp4_config->hFile, joseph_mp4_config->video, naluData, naluSize, MP4_INVALID_DURATION, 0, 1))
				{  
					return -1;  
				}
				
				//joseph_mp4_config->m_vFrameDur = 0;
				
				break;
			}
		default:
			break;
	}
		
	return 0;
}

int Mp4AEncode(JOSEPH_MP4_CONFIG* joseph_mp4_config, unsigned char* aacData, int aacSize)
{
	if(joseph_mp4_config->video == MP4_INVALID_TRACK_ID)
	{
		return -1;
	}
	MP4WriteSample(joseph_mp4_config->hFile, joseph_mp4_config->audio, aacData, aacSize , MP4_INVALID_DURATION, 0, 1);
	
	//joseph_mp4_config->m_vFrameDur += 1024;
	
	return 0;
}

int CloseAccEncoder(JOSEPH_ACC_CONFIG* joseph_aac_config)
{
	if(joseph_aac_config==NULL)
		return -1;
	#if 0
	if(joseph_aac_config->hEncoder)
	{  
		faacEncClose(joseph_aac_config->hEncoder);  
		joseph_aac_config->hEncoder = NULL;  
	}
	#else
	if(joseph_aac_config->handle)
	{
		joseph_aac_config->codec_api.Uninit(joseph_aac_config->handle);
		joseph_aac_config->handle=NULL;
	}
	#endif
	
	if(joseph_aac_config->pbPCMBuffer)
		free(joseph_aac_config->pbPCMBuffer);
	joseph_aac_config->pbPCMBuffer = NULL;
	if(joseph_aac_config->pbAACBuffer)
		free(joseph_aac_config->pbAACBuffer);
	joseph_aac_config->pbAACBuffer = NULL;
	joseph_aac_config->valid=0;
	return 0;
}

/*************************************************aac encode******************************************/
int InitAccEncoder(JOSEPH_ACC_CONFIG *joseph_aac_config)
{
	if(joseph_aac_config==NULL)
	{
		return -1;
	}
	if(joseph_aac_config->valid==1)
	{
		CloseAccEncoder(joseph_aac_config);
	}
	//zss++joseph_aac_config =(JOSEPH_ACC_CONFIG *)malloc(sizeof(JOSEPH_ACC_CONFIG));
	
	faacEncConfigurationPtr pConfiguration;
	int nRet = 0;
	
	joseph_aac_config->nSampleRate = 8000;
	joseph_aac_config->nChannels = 1;
	joseph_aac_config->nPCMBitSize = 16;
	joseph_aac_config->nInputSamples = 1024;
	joseph_aac_config->nMaxOutputBytes = 0;

	#if 0
	//zss++joseph_aac_config->fpIn = fopen(JOSEPH_G711A_LOCATION, "rb");
	//open FAAC engine
	LOGOUT("joseph_aac_config->nInputSamples is %d",joseph_aac_config->nInputSamples);
	joseph_aac_config->hEncoder = faacEncOpen(joseph_aac_config->nSampleRate, joseph_aac_config->nChannels, \
								&joseph_aac_config->nInputSamples, &joseph_aac_config->nMaxOutputBytes);
	if(joseph_aac_config->hEncoder == NULL)
	{
		LOGOUT("failed to call faacEncOpen()\n");
		return -2;
	}
	LOGOUT("joseph_aac_config->nInputSamples is %d",joseph_aac_config->nInputSamples);
	joseph_aac_config->nMaxInputPcmBuffer = (joseph_aac_config->nInputSamples*(joseph_aac_config->nPCMBitSize/8));
	joseph_aac_config->pbPCMBuffer=(unsigned char*)malloc(joseph_aac_config->nMaxInputPcmBuffer*sizeof(unsigned char));
	memset(joseph_aac_config->pbPCMBuffer, 0, joseph_aac_config->nMaxInputPcmBuffer);
	joseph_aac_config->pbAACBuffer=(unsigned char*)malloc(joseph_aac_config->nMaxOutputBytes*sizeof(unsigned char));
	memset(joseph_aac_config->pbAACBuffer, 0, joseph_aac_config->nMaxOutputBytes);
	
	//GET current encoding configuration
	pConfiguration = faacEncGetCurrentConfiguration(joseph_aac_config->hEncoder);
#if 1
	pConfiguration->inputFormat = FAAC_INPUT_16BIT;
	pConfiguration->outputFormat = 1;
    pConfiguration->aacObjectType = LOW;
#else
	pConfiguration->inputFormat = FAAC_INPUT_16BIT;

	/*0 - raw; 1 - ADTS*/
	pConfiguration->outputFormat = 0;
    pConfiguration->useTns = 0;
	pConfiguration->allowMidside = 1;
	pConfiguration->shortctl = SHORTCTL_NORMAL;
    pConfiguration->aacObjectType = LOW;
    pConfiguration->mpegVersion = MPEG2;
#endif	
	//set encoding configuretion
	nRet = faacEncSetConfiguration(joseph_aac_config->hEncoder, pConfiguration);
	#else
	
	joseph_aac_config->nMaxInputPcmBuffer=2*1024;
	joseph_aac_config->pbPCMBuffer = joseph_aac_config->nChannels*16*1024;
	joseph_aac_config->nMaxOutputBytes = joseph_aac_config->nChannels*16*1024;
	joseph_aac_config->pbPCMBuffer=(unsigned char*)malloc(joseph_aac_config->nMaxInputPcmBuffer*sizeof(unsigned char));
	memset(joseph_aac_config->pbPCMBuffer, 0, joseph_aac_config->nMaxInputPcmBuffer);
	joseph_aac_config->pbAACBuffer=(unsigned char*)malloc(joseph_aac_config->nMaxOutputBytes*sizeof(unsigned char));
	memset(joseph_aac_config->pbAACBuffer, 0, joseph_aac_config->nMaxOutputBytes);
	
	voGetAACEncAPI(&joseph_aac_config->codec_api);
	joseph_aac_config->mem_operator.Alloc = cmnMemAlloc;
	joseph_aac_config->mem_operator.Copy = cmnMemCopy;
	joseph_aac_config->mem_operator.Free = cmnMemFree;
	joseph_aac_config->mem_operator.Set = cmnMemSet;
	joseph_aac_config->mem_operator.Check = cmnMemCheck;
	joseph_aac_config->user_data.memflag = VO_IMF_USERMEMOPERATOR;
	joseph_aac_config->user_data.memData = &joseph_aac_config->mem_operator;
	VO_U32 vRet=joseph_aac_config->codec_api.Init(&joseph_aac_config->handle, VO_AUDIO_CodingAAC, &joseph_aac_config->user_data);
	if(vRet!=VO_ERR_NONE)
	{
		LOGOUT("failed to call codec_api.Init is %d",vRet);
		return -2;
	}
	joseph_aac_config->params.sampleRate = joseph_aac_config->nSampleRate;
	joseph_aac_config->params.bitRate = joseph_aac_config->nSampleRate*joseph_aac_config->nPCMBitSize;
	joseph_aac_config->params.nChannels = joseph_aac_config->nChannels;
	joseph_aac_config->params.adtsUsed = VOAAC_RAWDATA;
	if (joseph_aac_config->codec_api.SetParam(joseph_aac_config->handle, VO_PID_AAC_ENCPARAM, &joseph_aac_config->params) != VO_ERR_NONE)
	{
		LOGOUT("Unable to set encoding parameters\n");
		return -3;
	}
	#endif
	joseph_aac_config->valid=1;
	return 0;
}


int InitMp4Module(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config)
{
	int nRet = 0;
	/*init aac */
	if((nRet=InitAccEncoder(joseph_aac_config)) != 0)
	{
		LOGOUT("init aac failed %d",nRet);
		return -1;
	}
	
	/*init mp4*/ 
	if((nRet=InitMp4Encoder(joseph_aac_config,joseph_mp4_config)) != 0)
	{
		LOGOUT("init mp4 failed %d",nRet);
		return -2;
	}	
	g711Length=0;
	pcmAver=0;
	return 0;
}

int CloseMp4Module(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config)
{
	/*Close FAAC engine*/
	CloseAccEncoder(joseph_aac_config);
	CloseMp4Encoder(joseph_mp4_config);

	return 0;
}
static void SaveRecordFile(char* pPath, unsigned char* pu8Buffer, unsigned int u32Length)
{
	FILE* fp;
	fp = fopen(pPath, "ab+");
	fwrite(pu8Buffer, 1, u32Length, fp);
	fclose(fp);
}

int Mp4FileAudioEncode(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config,unsigned char* nBuffer,unsigned int length,unsigned int *power)
{
	if(joseph_aac_config==NULL || joseph_mp4_config==NULL || nBuffer==NULL)
		return -1;
	if(length<0)
		return -2;
	if(joseph_aac_config->nPCMBitSize==0)
		return -3;
	int nPCMSize = 320;
	int nPCMWrite = 0;
	int nRet=0;
	int avr=0;
	int i=0;
	DWORD sum=0;
	signed short int oneData=0;
	signed short int *ptr=NULL;
	unsigned char pbPCMTmpBuffer[320];
	nPCMSize=320;
	*power=0;
	if((nPCMSize = g711a_decode(pbPCMTmpBuffer,&nPCMSize,nBuffer,length)) < 0)
	{
		LOGOUT(" G711A -> PCM  fail %d",nPCMSize);
		nRet=-2;
		return nRet;
	}
	ptr=pbPCMTmpBuffer;
	for(i=0;i<length;i++)
	{
		oneData=*(ptr+i);
		if(oneData<0)
			oneData=-oneData;
		sum+=oneData;
	}
	int aver=sum/length;
	if(pcmAver!=0)
		pcmAver=(pcmAver+aver)/2;
	else
		pcmAver=aver;
	g711Length+=length;
	if(g711Length==1600)
	{
		*power=pcmAver;
		g711Length=0;
		//printf("---pcmAver=%d==time=%d==\n",pcmAver,getTickCountMs());
		pcmAver=0;
		
	}
	do
	{
		if((joseph_aac_config->nPcmBufferLelf+nPCMSize)>joseph_aac_config->nMaxInputPcmBuffer)
		{
			if(nPCMSize>0)
			{
				nPCMWrite=joseph_aac_config->nMaxInputPcmBuffer-joseph_aac_config->nPcmBufferLelf;
				memcpy(joseph_aac_config->pbPCMBuffer+joseph_aac_config->nPcmBufferLelf,pbPCMTmpBuffer,nPCMWrite);
				nPCMSize=nPCMSize-nPCMWrite;
				joseph_aac_config->nPcmBufferLelf=joseph_aac_config->nMaxInputPcmBuffer;
			}
		}
		else
		{
			if(nPCMSize>0)
			{
				memcpy(joseph_aac_config->pbPCMBuffer+joseph_aac_config->nPcmBufferLelf,pbPCMTmpBuffer+nPCMWrite,nPCMSize);
				joseph_aac_config->nPcmBufferLelf+=nPCMSize;
				nPCMSize=0;
				nRet=0;
			}
			break;
		}
		#if 0
		nRet = faacEncEncode(joseph_aac_config->hEncoder, (int*) (joseph_aac_config->pbPCMBuffer), \
				joseph_aac_config->nInputSamples, joseph_aac_config->pbAACBuffer,\
				joseph_aac_config->nMaxOutputBytes);
		#else 
		joseph_aac_config->nInputSamples = joseph_aac_config->nPcmBufferLelf;
		joseph_aac_config->input.Buffer = (uint8_t*) joseph_aac_config->pbPCMBuffer;
		joseph_aac_config->input.Length = joseph_aac_config->nInputSamples;
		VO_U32 vRet=joseph_aac_config->codec_api.SetInputData(joseph_aac_config->handle, &joseph_aac_config->input);
		if (vRet != VO_ERR_NONE) 
		{
			LOGOUT("SetInputData is %x",vRet);
			nRet=-1;
		}
		else
		{
			nRet=1;
		}
		joseph_aac_config->output.Buffer = joseph_aac_config->pbAACBuffer;
		joseph_aac_config->output.Length = joseph_aac_config->nMaxOutputBytes;
		vRet=joseph_aac_config->codec_api.GetOutputData(joseph_aac_config->handle, &joseph_aac_config->output, &joseph_aac_config->output_info);
		if (vRet != VO_ERR_NONE) 
		{
			LOGOUT("GetOutputData is %x",vRet);	
			nRet=-1;
		}
		else
		{
			//LOGOUT("GetOutputData datas is %d",joseph_aac_config->output.Length);
			nRet=joseph_aac_config->output.Length;
		}
		#endif
		//LOGOUT("nRet  %d  joseph_aac_config->nInputSamples %d",nRet,joseph_aac_config->nInputSamples);
		if(nRet<=0)
		{
			LOGOUT("faacEncEncode fail %d",nRet);
			nRet=-3;
			joseph_aac_config->nPcmBufferLelf=0;
			continue;
		}
		//SaveRecordFile("my.aac",joseph_aac_config->pbAACBuffer, nRet);
		/*write audio frame to mp4*/
		if((nRet=Mp4AEncode(joseph_mp4_config, joseph_aac_config->pbAACBuffer, nRet)) < 0)
		{
			LOGOUT("write audio frame failed %d",nRet);
			nRet=-4;
			break;
		}
		joseph_aac_config->nPcmBufferLelf=0;
	}while(1);
	return nRet;
}
/*********************************************main**************************************************/
int Mp4FileVideoEncode(JOSEPH_ACC_CONFIG* joseph_aac_config,JOSEPH_MP4_CONFIG *joseph_mp4_config,unsigned char* nBuffer,unsigned int length)
{	
	if(joseph_aac_config==NULL || joseph_mp4_config==NULL || nBuffer==NULL)
		return -1;
	int nRet = 0;
	int i=0;
	/*write video frame to mp4*/
	if((nRet=Mp4VEncode(joseph_mp4_config,nBuffer,(int)length)) < 0)
	{
		LOGOUT("write video frame failed %d",nRet);
		return -1;
	}
	return nRet;
}
