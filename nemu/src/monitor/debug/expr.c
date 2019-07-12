#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

bool check_parentheses(int p ,int q);
int dominant_operator(int p , int q);
int operator_precedence(int op);
uint32_t eval(int p, int q);

enum {
  TK_NOTYPE = 256,
  TK_EQ = 257,

  /* TODO: Add more token types */
  TK_10 = 258
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
          default: strncpy(tokens[nr_token].str,  substr_start, substr_len);
				   tokens[nr_token].type = rules[i].token_type;
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
	  Log("tokens[%d]: %s", i, tokens[i].str);
  /* TODO: Insert codes to evaluate the expression. */
  uint32_t result = eval(0, nr_token - 1);

  return result;
}

bool check_parentheses(int p ,int q){
    int tag = 0;
    for(int i = p; i <= q; i++){    
		if(tokens[i].type == '(')
		   	tag++;
	    else if(tokens[i].type == ')')
		   	tag--;
		if((tag < 0) || (tag == 0 && i < q))
			return false; // 括号错误：(4 + 3)) * ((2 - 1),没有用括号包裹整个表达式： 4 + 3 * (2 - 1)
	}                              
	if( tag != 0 )
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
		if(tokens[i].type == TK_10){ // 略过十进制数
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
		return atoi(tokens[p].str);
	}
	else if (check_parentheses(p, q) == true) {
		/* The expression is surrounded by a matched pair of parentheses.
		 * If that is the case, just throw away the parentheses.
		 */
	    return eval(p + 1, q - 1);
	}
	else {
		/* We should do more things here. */
		int op = dominant_operator(p , q);
    	uint32_t val1 = eval(p, op - 1);
		uint32_t val2 = eval(op + 1, q);
		switch (tokens[op].type) {
		    case '+': printf("'+', %d\n", val1 + val2); return val1 + val2;
			case '-': printf("'-', %d\n", val1 - val2); return val1 - val2;
		    case '*': printf("'*', %d\n", val1 * val2); return val1 * val2;
		    case '/': if(val2 == 0)
						  panic("表达式（%d, %d）结果为零！", op + 2, q + 1);
					  else
						  return val1 / val2;
		    case '%': if(val2 == 0)
						  panic("表达式（%d, %d）结果为零！", op + 2, q + 1);
					  else
						  return val1 % val2;
			default: panic("Error: tokens[%d]=%s, val1=%d, val2=%d\n", op, tokens[op].str, val1, val2);
		}
	}
}
