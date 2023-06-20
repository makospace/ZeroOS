IFNDEF __DOSHOOK_INC__
__DOSHOOK_INC__   EQU		1

TRUE                    EQU     1
FALSE                   EQU     0

INT13Param     STRUC   1               ; must be 16 bytes
	bDrive          BYTE    00H
	bFlag           BYTE    00H
	wCyln           WORD    0000H
	bHead           BYTE    00H
	bSect           BYTE    00H
	wCylnSect       WORD    0000H
	dwTotalSect     DWORD   00000000H
	dwReserved      DWORD   00000000H
INT13Param     ENDS

DskAddrPacket   STRUC   1
	bSize           BYTE    0
	reserved1       BYTE    0
	wBlockNum       WORD    0
	dwTrfBuffer     DWORD   0
	dwStartaddrLow  DWORD   0
	dwStartaddrHigh DWORD   0
DskAddrPacket   ENDS

ENDIF 