#ifndef TYPEDEF_H
#define TYPEDEF_H

#ifdef WIN32
#undef far
#define far
#endif

#ifndef BOOL
typedef int BOOL;
#endif
#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif
#ifndef NULL
#define NULL			(0)
#endif

#ifndef WIN32

typedef DWORD far *		LPDWORD;
typedef WORD far *		LPWORD;
typedef BYTE far *		LPBYTE;
typedef void far *		LPVOID;


#ifndef HIBYTE
#define HIBYTE(w)       ((BYTE)((WORD)w >> 8))
#endif
#ifndef LOBYTE
#define LOBYTE(w)       ((BYTE)((WORD)w & 0x00FF))
#endif
#ifndef HIWORD
#define HIWORD(dw)       ((WORD)((DWORD)dw >> 16))
#endif
#ifndef LOWORD
#define LOWORD(dw)       ((WORD)((WORD)dw & 0x0000FFFFl))
#endif
#ifndef MAKEWORD
#define MAKEWORD(lo, hi) ((WORD)lo | ((WORD)hi << 8))
#endif
#ifndef MAKEDWORD
#define MAKEDWORD(lo, hi) ((DWORD)lo | ((DWORD)hi << 16l))
#endif
#ifndef FP_SEG
#define FP_SEG(fp)      ((WORD)((DWORD)fp >> 16))
#endif
#ifndef FP_OFF
#define FP_OFF(fp)      ((WORD)((DWORD)fp & 0x0000FFFFl))
#endif
#ifndef MK_FP
#define MK_FP(seg, ofs) ((DWORD)((DWORD)ofs | ((DWORD)seg << 16)))
#endif

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#endif //WIN32

#define OUT
#define IN
#define INOUT

#endif //ifdef  TYPEDEF_H