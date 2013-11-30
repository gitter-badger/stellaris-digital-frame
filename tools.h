#ifndef _TOOLS_H_
#define _TOOLS_H_


//----- DEFINES -----
typedef struct
{
  unsigned int s;     //0-59
  unsigned int m;     //0-59
  unsigned int h;     //0-23
  unsigned int day;   //1-31
  unsigned int month; //1-12
  unsigned int year;  //1970-...
  unsigned int wday;  //0-6 (0=Sunday)
} TIME;


//----- PROTOTYPES -----
void                                   strshrinkpath(char *path);
char*                                  strrmvspace(char *dst, const char *src);
char*                                  strtoupper(char *dst, const char *src);
int                                    strstrk(char *dst, const char *src, const char *key);
const char*                            strstri(const char *s, const char *pattern);


unsigned int                           atou_hex(const char *s);
unsigned int                           atou(const char *s);
unsigned int                           atorgb(const char *s);

void                                   sectotime(unsigned long s, TIME *t);
unsigned long                          timetosec(unsigned int s, unsigned int m, unsigned int h, unsigned int day, unsigned int month, unsigned int year);
void                                   daytime(char *s, TIME *t);


#endif //_TOOLS_H_
