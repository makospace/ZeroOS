#ifndef FLOPPY_H
#define FLOPPY_H
	BOOL floppyRead(BYTE bCount, BYTE bTrack, BYTE bSector, BYTE bHead, BYTE bDriveNO, void far *pBuffer);
	BOOL floppyWrite(BYTE bCount, BYTE bTrack, BYTE bSector, BYTE bHead, BYTE bDriveNO, void far *pBuffer);
	BOOL Int13Floppy(BYTE bKey,BYTE bCount, BYTE bTrack, BYTE bSector, BYTE bHead, BYTE bDriveNO, void far *pBuf);
#endif //ifdef  FLOPPY_H