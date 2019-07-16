#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  char *arg = strtok(args, " ");
  if(arg == NULL) {
	  cpu_exec(1);
  } else {
	  cpu_exec(atoi(arg));
  }
  printf("OK!\n");
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(args, " ");
  if(arg == NULL) {
	  printf("请输入参数\n  r : 打印寄存器状态\n  w : 打印监视点信息\n");
	  return 0;
  } else if(strcmp(arg, "r") == 0) {
	  printf("EAX : %x\n", cpu.eax);
	  printf("EDX : %x\n", cpu.edx);
	  printf("ECX : %x\n", cpu.ecx);
	  printf("EBX : %x\n", cpu.ebx);
	  printf("EBP : %x\n", cpu.ebp);
	  printf("ESI : %x\n", cpu.esi); 
	  printf("EDI : %x\n", cpu.edi);
	  printf("ESP : %x\n", cpu.esp);
  } else if(strcmp(arg, "w") == 0){
	 print_wp(); 
  }
  printf("OK!\n");
  return 0;
}

static int cmd_p(char *args){
	bool success = true;
	if(args == NULL){
		printf("请输入参数N！\n");
	    return 0;
	}
	printf("表达式： %s\n", args);
	uint32_t result = expr(args, &success);
	if (success)
		printf("结果： %d\nOK!\n", result);
	else
		printf("表达式出错！\n");
    return 0;
}

static int cmd_x(char *args) {
	char *arg = strtok(args, " ");
	if(arg == NULL){
		printf("请输入参数N！\n");
	    return 0;
	}
	int  n = atoi(arg);
	char *EXPR = strtok(NULL, " ");
	if(EXPR == NULL){
		printf("请输入内存起始地址！\n");
		return 0;
	}
	bool success = true;
	vaddr_t addr = expr(EXPR, &success);
    if (!success){
		printf("表达式出错！\n");
		return 0;
	}
	for(int i = 0; i < n; i++){
		uint32_t data = vaddr_read(addr + i * 4, 4);
		printf("0x%08x	", addr + i * 4);
		for(int j = 0; j < 4; j++){
	        printf("0x%02x	" , data & 0xff);
	        data = data >> 8;
		}
		printf("\n");
	}
	printf("OK!\n");
	return 0;
}

static int cmd_w(char *args){printf("1111111111111111111\n");
    if(args == NULL){
		printf("请输入参数N！\n");
	    return 0;
	}
    new_wp(args);
	printf("OK!\n");
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "后面可以跟参数N, 让程序单步执行N条指令后暂停执行, 当N没有给出时, 缺省为1", cmd_si },
  { "info", "后面跟参数r, 打印寄存器状态; 后面跟参数w, 打印监视点信息", cmd_info},
  { "p", "p EXPR, 求出表达式EXPR的值", cmd_p},
  { "x", "x N EXPR, 求出表达式EXPR的值, 将结果作为起始内存地址, 以十六进制形式输出连续的N个4字节", cmd_x}, 
  { "w", "w EXPR, 当表达式EXPR的值发生变化时, 暂停程序执行", cmd_w}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
