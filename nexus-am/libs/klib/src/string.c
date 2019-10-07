#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int len = 0;
  const char* p = s;
  while(*p){
	len++;
	p++;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  return strncpy(dst, src, strlen(src));
}

char* strncpy(char* dst, const char* src, size_t n) {
  char* pd = dst;
  const char* ps = src;
  size_t i = 0;

  while(i++ < n && (*pd++ = *ps++) != '\0');
  if(*pd != '\0')
	  *pd = '\0';
  
  return dst;	
}

char* strcat(char* dst, const char* src) {
  char* p = dst + strlen(dst);
  strcpy(p, src);
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);
  return strncmp(s1, s2, len1 < len2 ? len1 : len2);
}

int strncmp(const char* s1, const char* s2, size_t n) {
  const char* p1 = s1;
  const char* p2 = s2;

  for(size_t i = 0; i < n && *p1 != '\0' && *p2 != '\0'; i++, p1++, p2++)
	if(*p1 != *p2)
	  return *p1 - *p2;

  return 0;
}

void* memset(void* v, int c, size_t n) {
  char* p = (char*) v;
  for(size_t i = 0; i < n; i++, p++)
	*p = c;

  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char* po = (char*) out;
  const char* pi = (const char*) in;
  size_t i = 0;

  while(i++ < n)
	  *po++ = *pi++;
  
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  const char* p1 = (const char*) s1;
  const char* p2 = (const char*) s2;
  size_t i = 0;

  while(i++ < n)
	if(*p1 != *p2)
	  return *p1++ - *p2++;

  return 0;
}

char* my_itoa(int d, char* str) {
  int sign = d < 0 ? 1 : 0;
  char buf[1024] = {0};
  char* p = buf;
  if (sign) {
    *p++ = '-';
  }
  //处理0的特殊情况
  if (d == 0) {
    *buf = '0';
  }
  while (d) {
    *p++ = (d % 10) + '0';
    d /= 10;
  }
  return strrev(str, buf);
}

char* strrev(char* dst, const char* src) {
  char* pd = dst;
  const char* ps = src + strlen(src) - 1;
  for (size_t i = 0; i < strlen(src); i++, pd++, ps--)
    *pd = *ps;

  *pd = '\0';

  return dst;
}

int atoi(const char* str) {
  int result = 0;
  int sign = 0;
  const char* ps = str;
  // 负值 sign=1
  if (*ps == '-') {
    sign = 1;
    ps++;
  }
  while (*ps) {
    result = result * 10 + (*ps - '0');
    ps++;
  }
  return sign ? -result : result;
}

char* i2hex(int d, char* str) {
  char hex[9] = {0};
  for (int i = 0; i < 8; i++) {
	// 把d(32位)转换为16进制存入hex[0-7]
    hex[i] = (d >> ((7 - i) * 4)) & 0xf;
	// 上一步骤存入的是数字，转换为对应的ascii字符
    if (hex[i] < 10)
      hex[i] += '0';
    else
      hex[i] += 'a' - 10;
  }
  hex[8] = '\0';

  // 抛去前面多余的‘0’
  char* ph = hex;
  while (*ph == '0') 
	  ph++;

  return strcpy(str, ph);
}

char* double2a(double f, char* str) {
  // double强制转换int，向下取整
  char* p_str = str + strlen(my_itoa(f, str));
  *p_str++ = '.';
  // 提取小数部分
  double rem = f - (int)f;
  if (rem == 0) {
    *p_str++ = '0';
  }
  assert(0 < rem && rem < 1);
  while (rem) {
    rem *= 10;
    *p_str++ = (int) rem + '0';
    rem = rem - (int) rem;
    assert(0 < rem && rem < 1);
  }
  *p_str = '\0';

  return str;
}
#endif
