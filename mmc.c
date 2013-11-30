#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "third_party/fatfs/ff.h"
#include "third_party/fatfs/diskio.h"
#include "mmc_io.h"
#include "mmc.h"
#include "debug.h"
#include <stdio.h>

FATFS fatfs;
FIL fileobj;
char lfn[_MAX_LFN+1];

void fs_unmount(void)
{
  DEBUGOUT("MMC: unmount\n");

  f_mount(0, 0);

  return;
}


void fs_mount(void)
{
  unsigned int mount_try=3;

  if(disk_status(0) & STA_NOINIT)
  {
    DEBUGOUT("MMC: init\n");
    while((disk_initialize(0) & STA_NOINIT) && --mount_try);
  }

  DEBUGOUT("MMC: mount\n");

  if(disk_status(0) == 0)
  {
    f_mount(0, &fatfs);
  }

  return;
}

unsigned int fs_checkitem(FILINFO *finfo)
{
  unsigned int len;
  char *fname, c1, c2, c3, c4, c5;

  if(!(finfo->fattrib & (AM_HID|AM_SYS))) //no system and hidden files
  {
    if(finfo->fattrib & AM_DIR) //directory
    {
      return 0;
    }
    else //file
    {
#if _USE_LFN
      fname = (*finfo->lfname)?finfo->lfname:finfo->fname;
#else
      fname = finfo->fname;
#endif
      len = strlen(fname);
      if(len > 4)
      {
        c5 = toupper(fname[len-5]);
        c4 = toupper(fname[len-4]);
        c3 = toupper(fname[len-3]);
        c2 = toupper(fname[len-2]);
        c1 = toupper(fname[len-1]);
        if((c4 == '.') &&
           (((c3 == 'A') && (c2 == 'A') && (c1 == 'C')) || //AAC
            ((c3 == 'M') && (c2 == 'P') && (c1 == '3')) || //MP3
            ((c3 == 'O') && (c2 == 'G') && (c1 == 'G')) || //OGG
            ((c3 == 'W') && (c2 == 'A') && (c1 == 'V')) || //WAV
            ((c3 == 'W') && (c2 == 'M') && (c1 == 'A'))))  //WMA
        {
          return 0;
        }
        else if((c5 == '.') &&
                ((c4 == 'F') && (c3 == 'L') && (c2 == 'A') && (c1 == 'C')))   //FLAC
        {
          return 0;
        }
      }
    }
  }

  return 1;
}


unsigned int fs_isdir(const char *path, unsigned int item)
{
  FILINFO finfo;
  DIR dir;
  unsigned int i=0;

#if _USE_LFN
  finfo.lfname = lfn;
  finfo.lfsize = sizeof(lfn);
#endif

  if(f_opendir(&dir, path) == FR_OK)
  {
    while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
    {
      if(fs_checkitem(&finfo) == 0)
      {
        if(item == i)
        {
          if(finfo.fattrib & AM_DIR)
          {
            return 0;
          }
          break;
        }
        i++;
      }
    }
  }

  return 1;
}



void fs_getitem(const char *path, unsigned int item, char *dst, unsigned int len)
{
  FILINFO finfo;
  DIR dir;
  char *fname;
  unsigned int i=0;

#if _USE_LFN
  finfo.lfname = lfn;
  finfo.lfsize = sizeof(lfn);
#endif

  *dst = 0;

  if(f_opendir(&dir, path) == FR_OK)
  {
    while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
    {
      if(fs_checkitem(&finfo) == 0)
      {
        if(item == i)
        {
#if _USE_LFN
          fname = (*finfo.lfname)?finfo.lfname:finfo.fname;
#else
          fname = finfo.fname;
#endif
          if(finfo.fattrib & AM_DIR)
          {
            *dst = '/';
            strncpy(dst+1, fname, len-1-1);
            dst[len-1] = 0;
          }
          else
          {
            strncpy(dst, fname, len-1);
            dst[len-1] = 0;
          }
          break;
        }
        i++;
      }
    }
  }

  return;
}

unsigned int fs_items(const char *path)
{
  FILINFO finfo;
  DIR dir;
  unsigned int i=0;

#if _USE_LFN
  finfo.lfname = lfn;
  finfo.lfsize = sizeof(lfn);
#endif

  if(disk_status(0) & STA_NOINIT)
  {
    return 0;
  }

  if(f_opendir(&dir, path) == FR_OK)
  {
    while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
    {
      if(fs_checkitem(&finfo) == 0)
      {
        i++;
      }
    }
  }

  return i;
}
