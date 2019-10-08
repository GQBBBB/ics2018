#include <am.h>
#include <x86.h>
#include <amdev.h>

#define RTC_PORT 0x48

static uint32_t am_last_time;

size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  static uint64_t t = 0;
  uint32_t am_now_time = inl(RTC_PORT);
  t = am_now_time - am_last_time;
  am_last_time = am_now_time;
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _UptimeReg *uptime = (_UptimeReg *)buf;
      uptime->hi = (t >> 32) & 0xffffffff;
      uptime->lo = t & 0xffffffff;
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2018;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
  am_last_time = inl(RTC_PORT);
}
