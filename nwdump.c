
/***************************************************************************
*
*   Copyright (c) 1998, 2022 Jeff V. Merkey  All Rights
*			    Reserved.
*
*   This program is an unpublished work of TRG, Inc. and contains
*   trade secrets and other proprietary information.  Unauthorized
*   use, copying, disclosure, or distribution of this file without the
*   consent of TRG, Inc. can subject the offender to severe criminal
*   and/or civil penalities.
*
*
*   AUTHOR   :  Jeff V. Merkey (jeffmerkey@gmail.com)
*   FILE     :  NWVOL.C
*   DESCRIP  :   NWVOL Utility
*   DATE     :  November 23, 1998
*
*
***************************************************************************/

#include "globals.h"

BYTE Sector[IO_BLOCK_SIZE];
WORD partitionSignature;
WORD stamp;

ULONG atox(BYTE *p)
{
    register ULONG c = 0;

    if (!p)
       return 0;

    while (*p)
    {
       if (*p >= '0' && *p <= '9')
	  c = (c << 4) | (*p - '0');
       else
       if (*p >= 'A' && *p <= 'F')
	  c = (c << 4) | (*p - 'A' + 10);
       else
       if (*p >= 'a' && *p <= 'f')
	  c = (c << 4) | (*p - 'a' + 10);
       else
	  break;
       p++;
    }
    return (c);
}

unsigned long ipause(void)
{
   register unsigned long key;
#if (DOS_UTIL)
   extern int getch(void);
#endif

   printf(" --- More --- ");
   key = getch();
   printf("%c              %c", '\r', '\r');
   if (key == 0x1B) // ESCAPE
      return 1;
   else
      return 0;

}

int main(int argc, char *argv[])
{
    register ULONG retCode, total, lba, disk;
    extern BYTE *dumpRecordBytes(BYTE *, ULONG);
    extern void UtilScanVolumes(void);
    
    if (argc < 2)
    {
       NWFSPrint("USAGE:  nwdump <sector> <disk>\n");
       return 0;
    }

    lba = atox(argv[1]);

    if (argc > 2)
       disk = atol(argv[2]);
    else
       disk = 0;

    if (disk > 128)
    {
       NWFSPrint("invalid disk-%d\n", (int)disk);
       return 0;
    }

    InitNWFS();
    UtilScanVolumes();

    if (!SystemDisk[disk])
       return 0;

    total = (ULONG)(SystemDisk[disk]->Cylinders *
		   SystemDisk[disk]->TracksPerCylinder *
		   SystemDisk[disk]->SectorsPerTrack);

    retCode = ReadDiskSectors(disk, lba, Sector, 1, 1);
    if (!retCode)
       return 0;

    dumpRecordBytes(Sector, 256);
    ipause();
    dumpRecordBytes(&Sector[256], 256);

    NWFSVolumeClose();
    RemoveDiskDevices();
    FreePartitionResources();

    return 0;

}




