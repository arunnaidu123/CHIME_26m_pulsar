#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "time_convert.h"
int seconds2utc(long seconds, char* utc)
{
  long now = seconds;
  struct tm  ts;

  // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
  ts = *localtime(&now);
  strftime(utc, sizeof(utc), "%Y-%m-%d-%H:%M:%S", &ts);
  printf("%s\n", utc);
  return 0;
}

