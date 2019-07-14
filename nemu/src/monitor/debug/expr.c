#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

bool check_parentheses(int p ,int q);
int dominant_operator(int p , int q);
int operator_precedence(int op);
uint32_t eval(int p, int q);

enum {
  TK_NOTYPE = 256,
  TK_EQ = 257,

  /* TODO: Add more token types */
  TK_10 = 258,
  TK_16 = 259,
  TK_REG = 260,
  TK_NEG = 261,
  TK_POINT = 262
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // reduce
  {"\\*", '*'},         // mult
  {"\\/", '/'},         // div
  {"\\%", '%'},         // mod
  {"\\(", '('},           // (
  {"\\)", ')'},           // )

  {"==", TK_EQ},         // equal

  {"\\$[eE][a-dsA-DS][xpiXPI]|\\$[a-dsA-DS][xpiXPI]|\\$[a-dA-D][hlHL]", TK_REG}, // 寄存器
  {"0[xX][0-9a-fA-F]+", TK_16}, // 十六进制数
  {"[1-9][0-9]*", TK_10} // 十进制正整数
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
 
        switch (rules[i].token_type) {
	      case TK_NOTYPE: break;
          default: strncpy(tokens[nr_token].str, substr_start, substr_len);
                   tokens[nr_token].str[substr_len] = '\0';
				   tokens[nr_token].type = rules[i].token_type;

				   // 处理负号，指针
                   if(tokens[nr_token].type == '*' && (nr_token == 0 || (tokens[nr_token-1].type != TK_10 && tokens[nr_token-1].type != TK_REG && tokens[nr_token-1].type != TK_16)))
	                   tokens[nr_token].type = TK_POINT;
                   if(tokens[nr_token].type == '-' && (nr_token == 0 || (tokens[nr_token-1].type != TK_10 && tokens[nr_token-1].type != TK_REG && tokens[nr_token-1].type != TK_16)))
	                   tokens[nr_token].type = TK_NEG;

				   // printf("==========\n%s\n=========\n",tokens[nr_token].str);
				   nr_token++;
				   break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  for(int i = 0; i< nr_token; i++)
	  Log("tokens[%d]: '%s' (%d)", i, tokens[i].str, tokens[i].type);
  /* TODO: Insert codes to evaluate the expression. */
  uint32_t result = eval(0, nr_token - 1);

  return result;
}

bool check_parentheses(int p ,int q){
    int tag = 0, flag = 1;
    for(int i = p; i <= q; i++){    
		if(tokens[i].type == '(')
		   	tag++;
	    else if(tokens[i].type == ')')
		   	tag--;
        if(tag < 0)
			panic("Error: 括号错误！\n");
		if(tag == 0 && i < q)
			flag = 0;
	}                              
	if(tag != 0)
	   	panic("Error: 括号错误！\n");
    if(flag == 0)
	    return false;	
	return true;                   
} 

int dominant_operator(int p , int q){
    int domin = p, left = 0, flag = 0;
    for(int i = p; i <= q; i++){
		if(tokens[i].type == '('){ // 略过括号内
			left += 1;
			i++;
		    while(1){
				if(tokens[i].type == '(')
				   	left += 1;
				else if(tokens[i].type == ')')
				   	left --;
			    i++;
				if(left == 0)
				    break;
			}  
			if(i > q)
				break;
		}
		if(tokens[i].type == TK_10 || tokens[i].type == TK_16 || tokens[i].type == TK_REG){ // 略过十进制数, 十六进制数，寄存器
		   	continue;
		}
		if(operator_precedence(tokens[i].type) >= flag){ // 处理运算符优先级
	        flag = operator_precedence(tokens[i].type);
			domin = i;
			char dest[255] = "\0";
			for(int j = p; j <= q; j++)
				strcat(dest, tokens[j].str);
			Log("对于字符串'%s', 主操作符:tokens[%d]='%s'", dest, domin, tokens[domin].str);
	    }
	}          
	return domin;
}       

int operator_precedence(int op){
	if(op == 257) // '=='
		return 7;
    else if(op == '+' || op == '-') // '+', '-'
		return 4;
	else if(op == '*' || op == '/' || op == '%') // '*', '/', '%'
		return 3;
	else if(op == 261 || op ==262) // TK_NEG, TK_POINT
		return 2;
	else
		return 0;
}

uint32_t eval(int p, int q) {
    if (p > q) {
	    /* Bad expression */
		return -1;
	}
	else if (p == q) {
		/* Single token.
		 * For now this token should be a number.
		 * Return the value of the number.                         
		 */
		if(tokens[p].type == TK_10){
			// printf("10:%d\n",atoi(tokens[p].str));
            return atoi(tokens[p].str);
		}else if(tokens[p].type == TK_16){
            char *str;
	        uint32_t toi = strtol(tokens[p].str, &str, 16);
			// printf("16:%d\n",toi);
			return toi;
		}else if(tokens[p].type == TK_REG){
			char str1[32] = "\0";
			char *str2;
            if(strcmp(tokens[p].str, "$EAX") == 0 || strcmp(tokens[p].str, "$eax") == 0){
				sprintf(str1, "%8x", cpu.eax);
			}
			else if(strcmp(tokens[p].str, "$EBX") == 0 || strcmp(tokens[p].str, "$ebx") == 0){
				sprintf(str1, "%8x", cpu.ebx);
			}
			else if(strcmp(tokens[p].str, "$ECX") == 0 || strcmp(tokens[p].str, "$ecx") == 0){
				sprintf(str1, "%8x", cpu.ecx);
			}
			else if(strcmp(tokens[p].str, "$EDX") == 0 || strcmp(tokens[p].str, "$edx") == 0){
				sprintf(str1, "%8x", cpu.edx);
			}
			else if(strcmp(tokens[p].str, "$EBP") == 0 || strcmp(tokens[p].str, "$ebp") == 0){
				sprintf(str1, "%8x", cpu.ebp);
			}
			else if(strcmp(tokens[p].str, "$ESP") == 0 || strcmp(tokens[p].str, "$esp") == 0){
				sprintf(str1, "%8x", cpu.esp);
			}
			else if(strcmp(tokens[p].str, "$ESI") == 0 || strcmp(tokens[p].str, "$esi") == 0){
				sprintf(str1, "%8x", cpu.esi);
			}
			else if(strcmp(tokens[p].str, "$EDI") == 0 || strcmp(tokens[p].str, "$edi") == 0){
				sprintf(str1, "%8x", cpu.edi);
			}
			else if(strcmp(tokens[p].str, "$AX") == 0 || strcmp(tokens[p].str, "$ax") == 0){
				sprintf(str1, "%8x", cpu.eax & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$BX") == 0 || strcmp(tokens[p].str, "$bx") == 0){
				sprintf(str1, "%8x", cpu.ebx & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$CX") == 0 || strcmp(tokens[p].str, "$cx") == 0){
				sprintf(str1, "%8x", cpu.ecx & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$DX") == 0 || strcmp(tokens[p].str, "$dx") == 0){
				sprintf(str1, "%8x", cpu.edx & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$BP") == 0 || strcmp(tokens[p].str, "$bp") == 0){
				sprintf(str1, "%8x", cpu.ebp & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$SP") == 0 || strcmp(tokens[p].str, "$sp") == 0){
				sprintf(str1, "%8x", cpu.esp & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$SI") == 0 || strcmp(tokens[p].str, "$si") == 0){
				sprintf(str1, "%8x", cpu.esi & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$DI") == 0 || strcmp(tokens[p].str, "$di") == 0){
				sprintf(str1, "%8x", cpu.edi & 0xffff);
			}
			else if(strcmp(tokens[p].str, "$AH") == 0 || strcmp(tokens[p].str, "$ah") == 0){
				sprintf(str1, "%8x", (cpu.eax >> 8) & 0xff);
			}
			else if(strcmp(tokens[p].str, "$AL") == 0 || strcmp(tokens[p].str, "$al") == 0){
				sprintf(str1, "%8x", cpu.eax & 0xff);
			}
			else if(strcmp(tokens[p].str, "$BH") == 0 || strcmp(tokens[p].str, "$bh") == 0){
				sprintf(str1, "%8x", (cpu.ebx >> 8) & 0xff);
			}
			else if(strcmp(tokens[p].str, "$BL") == 0 || strcmp(tokens[p].str, "$bl") == 0){
				sprintf(str1, "%8x", cpu.ebx & 0xff);
			}
			else if(strcmp(tokens[p].str, "$CH") == 0 || strcmp(tokens[p].str, "$ch") == 0){
				sprintf(str1, "%8x", (cpu.ecx >> 8) & 0xff);
			}
			else if(strcmp(tokens[p].str, "$CL") == 0 || strcmp(tokens[p].str, "$cl") == 0){
				sprintf(str1, "%8x", cpu.ecx & 0xff);
			}
			else if(strcmp(tokens[p].str, "$DH") == 0 || strcmp(tokens[p].str, "$dh") == 0){
				sprintf(str1, "%8x", (cpu.edx >> 8) & 0xff);
			}
			else if(strcmp(tokens[p].str, "$DL") == 0 || strcmp(tokens[p].str, "$dl") == 0){
				sprintf(str1, "%8x", cpu.edx & 0xff);
			}
			uint32_t toi = strtol(str1, &str2, 16);
			// printf("reg: str1: %s, toi: %d\n",str1, toi);
			return toi;
		}
		panic("Error: tokens[%d]出错！", p);
	}
	else if (check_parentheses(p, q) == true) {
		/* The expression is surrounded by a matched pair of parentheses.
		 * If that is the case, just throw away the parentheses.
		 */
	    return eval(p + 1, q - 1);
	}
	else {
		/* We should do more things here. */
		int op = dominant_operator(p, q);
    	uint32_t val1 = eval(p, op - 1);
        uint32_t val2 = eval(op + 1, q);
		char str3[32] = "\0";
	    char *str4;
        vaddr_t addr;
		switch (tokens[op].type) {
		    case '+': return val1 + val2;
			case '-': return val1 - val2;
		    case '*': return val1 * val2;
		    case '/': if(val2 == 0)
						  panic("表达式（%d, %d）结果为零！", op + 2, q + 1);
					  else
						  return val1 / val2;
		    case '%': if(val2 == 0)
						  panic("表达式（%d, %d）结果为零！", op + 2, q + 1);
					  else
						  return val1 % val2;
		    case 261: return -val2;
			case 262: sprintf(str3, "%d", val2); 
					  addr = strtol(str3, &str4, 10);
                      printf("262: 十:%d, str:%s, address:%x, value:%d\n", val2, str3, addr, vaddr_read(addr, 1));
					  return vaddr_read(addr, 1);
			default: panic("Error: tokens[%d]=%s, val1=%d, val2=%d\n", op, tokens[op].str, val1, val2);
		}
	}
}
