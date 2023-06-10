#ifndef SYSINT_H
#define SYSINT_H
//void sysInt_init();
void sysInt_int22(WORD addSeg, WORD addOff);
void int22_handler(union REGS far* pR, struct SREGS far* pSR);
#endif //ifdef  SYSINT_H