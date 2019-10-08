#include <am.h>
#include <amdev.h>
#include <klib.h>

int main(){
  _RTCReg rtc;
  _ioe_init();
  int sec = 2;
  printf("000000000000000000000000000000000\n");
  printf("%d\n", uptime());
  while (1) {
    while(uptime() < 1000 * sec) ;
    get_timeofday(&rtc);
    printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    if (sec == 1) {
      printf("%d second).\n", sec);
    } else {
      printf("%d seconds).\n", sec);
    }
    sec ++;
  }
  return 0;
}
