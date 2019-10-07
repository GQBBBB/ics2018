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
	/*
  const char* p1 = s1;
  const char* p2 = s2;
  size_t i = 0;

  while(i++ < n && *p1 != '\0' && *p2 != '\0')
	if(*p1 != *p2)
	  return *p1++ - *p2++;

  return 0;*/
const char* p_s1 = s1;
  const char* p_s2 = s2;
  for (size_t i = 0; i < n && *p_s1 && *p_s2; i++, p_s1++, p_s2++) {
    if (*p_s1 != *p_s2) {
      return *p_s1 - *p_s2;
    }
  }
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

#endif
