## 02-軟體/07-系統程式/08-ipcchat/05-udp/chat2.c:


### 任何ID進來的封包都會接收
```
 saddr.sin_addr.s_addr = INADDR_ANY;
 
```

### 取得此結構的長度 ，sizeof在c語言中很常會運用到
```
 socklen_t rAddrLen = sizeof(struct sockaddr);
```

###  對方連線的請求

```
printf("receive: %s from client addr %s\n", msg, inet_ntoa(raddr.sin_addr));
```

### 不管server或client都會叫fork分裂成2個部分，一個負責接收訊息，一個負責送出訊息

### 這段是server和client一起共用的，sendto會送出訊息，recvfrom接收訊息

```
  if (fork() == 0) {
        // child: receive message and print
        while (1) {
            socklen_t rAddrLen = sizeof(struct sockaddr);
            recvfrom(sfd, msg, SMAX, 0, (struct sockaddr*) &raddr, &rAddrLen);
            printf("receive: %s", msg);
        }
    } else {
        // parent: readline and send msg
        while (1) {
            fgets(msg, SMAX, stdin);
            sendto(sfd, msg, strlen(msg)+1, 0, (struct sockaddr*) &raddr, sizeof(struct sockaddr));
        }
    }
```

## 02-軟體/07-系統程式/08-ipcchat/06-tcp/chat.c:

### TCP必須做listena這個動作，當對方的請求連線進來後，通過accept接受後，連線就開始建立，因為TCP是連線導向協定，直到斷線為止，此連線會一直存在，在client端方面，要用connect接受，在server端對應的是accept。

```
if (argc==1) { // server
        printf("I am server...\n");
        saddr.sin_addr.s_addr = INADDR_ANY;
        bind(sfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr));
        listen(sfd, 1);
        socklen_t rAddrLen = sizeof(struct sockaddr);
        cfd = accept(sfd, (struct sockaddr*) &raddr, &rAddrLen);
        printf("accept: cfd=%d client addr %s\n", cfd, inet_ntoa(raddr.sin_addr));
        fd = cfd;
    } else { // client
        printf("I am client...\n");
        saddr.sin_addr.s_addr = inet_addr(argv[1]);
        connect(sfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr));
        fd = sfd;
        printf("connect success: sfd=%d server addr=%s\n", sfd, inet_ntoa(saddr.sin_addr));
    }
```

### 這段與udp很相似，使用fork分裂成2個部分，一個接收一個送出，接收是使用recv，送出給receive
### 在TCP中使用 recv，而在UDP才是使用 recvfrom，不能混著使用，否則會接收不到訊息發生錯誤

```
if (fork() == 0) {
        // child: receive message and print
        while (1) {
            int n = recv(fd, msg, SMAX, 0);
            if (n <=0) break;
            printf("receive: %s", msg);
        }
    } else {
        // parent: readline and send msg
        while (1) {
            fgets(msg, SMAX, stdin);
            send(fd, msg, strlen(msg)+1, 0);
        }
    }
```
### TCP與UDP的差異:

![](https://drive.google.com/uc?export=view&id=11JsO1jg0-jVonzWSI-u0uC_WbxMY0SB7)


### 當Argc==1時，不用打參數，因為是為了要檢查
```
if(Argc==1) --> ./chat
if(Argc==2) -->./chat 127.0.0.1
```
### c語言缺點: 使用的套件常常不能掛平台，幾乎都是跟作業系統綁在一起，即使是sqlite普遍的套件也沒辦法直接掛在作業系統。
### c語言優點:可以使用作業系統上任何的函數，速度快。

## RISC-V指令集:
### RV32I: 基礎整數指令集(固定不變了)
### RV32M: 乘法和除法
### RV32F:單精度浮點操作(和RV32D:雙精度浮點操作)
### RV32A:原子操作
### RV32C:可選的壓縮擴展(對應32位元的RV32G)
### RV32B:基本擴展
### RV32V:向量擴展(SIMD)指令
### RV64G:RISC-V的64位地址版本

### RV32I 是必備的指令集，其他都可不支援。

### 以下是其指令與格式如下所式：
![](https://github.com/riscv2os/riscv2os/wiki/img/rv32i1.png)

## RISC-V 指令格式:
![](https://github.com/riscv2os/riscv2os/wiki/img/riscv_format32_2.png)
### 立即值的部分有時被分散成幾區，像是 S-Type 分為 imm[4:0]+imm[11:5]，而 B,J 等類型的指令，imm 被分為更多部分，這主要是為了相容與擴充性的考量，在用 Verilog 或 VHDL 設計電路時可以用 decoder 先將立即值組合成單一欄位，然後才開始執行該指令。


## hackcpu指令:

### A指令:功能為把一個數值傳送到cpu中的A暫存器，第一個為0就是A指令
### C指令:前面為111後面都是C指令

## 壓縮指令集:

### 為了提升RISC-V 處理器具有商業競爭力，指令集支援了壓縮模式，如果最後2碼aa 不是 11，那就是16位元的指令，反之，就是32位元以上指令。
![](https://github.com/riscv2os/riscv2os/wiki/img/riscv_format.png)

## 浮點指令集:

### 由第一行來決定是什麼指令，但有時候是第四行，所以1和4行要互相比較是否是一樣的

### 以下是單精度浮點指令 RV32F 的格式表:
![](https://github.com/riscv2os/riscv2os/wiki/img/rv32f.png)

### 雙精度 RV32D 的格式類似，如下表所示:
![](https://github.com/riscv2os/riscv2os/wiki/img/rv32d.png)


## 原子指令集:

### 作業系統為了讓 thread 或 process 能共享變數卻不會產生競爭情況，處理完再歸還回去，此時需要 RV32A《原子指令集》的支援。

### 其指令格式如下:

![](https://github.com/riscv2os/riscv2os/wiki/img/rv32a.png)
### 有時候編譯器為了速度起見，會調整一些指令的順序，這叫做錯序執行。但可能會造成問題，因此RISC-V 提供了 Fence.i 這樣的指令來避免錯誤產生。
### 如圖:
![](https://github.com/riscv2os/riscv2os/wiki/img/rvzext.png)


## 64 位元指令集:
### 對於 64 位元的架構，其基本指令和 32 位元相同，指令也仍然是 32 位元的架構，但是暫存器卻是 64 位元的。
## 如圖:
![](https://github.com/riscv2os/riscv2os/wiki/img/rv64isa.png)
### 上圖中的 word 代表 32 位元整數，doubleword 代表 64 位元整數。


## 假指令:
### 由於 RISC-V 的指令集相當精簡，因此其他處理器上具備的指令，在 RISC-V 不見得有對應的指令，就會有假指令的存在

### 如圖顯示，第一行是假指令，其中la rd=auipc rd，la其實是不存在，但必須用auipc rd來合成，需要假指令是因為進auipc rd這指令有效位元數不足，才會分成2個指令。

![](https://github.com/riscv2os/riscv2os/wiki/img/pseudo1.png)


### 如圖顯示， RISC-V 沒有移動 (mv) 之類的指令，mv是指暫存器的移動，使用 addi rs, rd, 0 這樣的指令可以做到mv移動，rd=rs+0，就等於移動。

![](https://github.com/riscv2os/riscv2os/wiki/img/pseudo2.png)







### mini-riscv-os/02-ContextSwitch/os.c:

```
#include "os.h"

#define STACK_SIZE 1024
reg_t task0_stack[STACK_SIZE];
#宣告2個結構並可保留2個內容
struct context ctx_os;
struct context ctx_task;

extern void sys_switch(); #宣告一個sys_switch()，連接到主程式

void user_task0(void)
{
	lib_puts("Task0: Context Switch Success !\n");
	while (1) {} // stop here.
}

int os_main(void)    #作業系統主程式
{
	lib_puts("OS start\n");
	ctx_task.ra = (reg_t) user_task0;
	ctx_task.sp = (reg_t) &task0_stack[STACK_SIZE-1];
	sys_switch(&ctx_os, &ctx_task);
	return 0;
}
```


### mini-riscv-os/02-ContextSwitch/start.s:
```
.equ STACK_SIZE, 8192

.global _start

_start:
    csrr a0, mhartid                # 讀取核心代號
    bnez a0, park                   # 若不是 0 號核心，跳到 park 停止
    la   sp, stacks + STACK_SIZE    # 0 號核心設定堆疊
    j    os_main                    # 0 號核心跳到主程式 os_main

park:
    wfi
    j park

stacks:
    .skip STACK_SIZE                # 分配堆疊空間
```

### mini-riscv-os/03-MultiTasking/sys.s:
```
sys_switch:
        ctx_save a0  # a0 => struct context *old  #a0指第0個參數，把目前CPU那些暫存器都存到a0(old)
        ctx_load a1  # a1 => struct context *new  #a1指第1個參數，把new載入，等於新的把整套換掉
        ret          # pc=ra; swtch to new task (new->ra) # ret會採取pc=ra此動作，前面2個動作把整套換掉，因此ra是新的，pc等於ra，會跳到新的之前返回位置去，就會切換過去
```



## 04-TimerInterrupt:
### Build & Run:
### 此程式是顯示出timer_handler(時間中斷)從1開始直接按下Ctrl-A and then X才會離開程式，每個間隔約0.1秒，以達到實現時間中斷的目的
### 主要目的為示範如何設定中斷向量，設定時間中斷使它固定時間發生。
```
$ make
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s sys.s lib.c timer.c os.c
$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf        
OS start
timer_handler: 1
timer_handler: 2
timer_handler: 3
timer_handler: 4
timer_handler: 5
QEMU: Terminated
```

### mini-riscv-os/04-TimerInterrupt/os.c:
### 此程式會產生無窮迴圈，一直顯示timer_handler:
### 這邊無窮迴圈什麼事情都沒做但會印出來，是因為有設定中斷函數、中斷時間，中斷時間到了會中斷一次，就會在設定下次的中斷時間，會不斷延續同樣的動作
```
#include "os.h"

int os_main(void)
{
	lib_puts("OS start\n");
	timer_init(); // 開始時間中斷
	while (1) {} //每次時間到就會觸發中斷
	return 0;
}

```

### mini-riscv-os/04-TimerInterrupt/timer.c:
### 有分三種模式，機器、使用者、特權等模式，此程式是執行在機器模式下，把機器模式裡的時間中斷向量改成sys_timer
```
#include "timer.h"

#define interval 10000000 // cycles; about 1 second in qemu.

void timer_init()
{
  // each CPU has a separate source of timer interrupts.
  int id = r_mhartid();

  // ask the CLINT for a timer interrupt.
  *(reg_t*)CLINT_MTIMECMP(id) = *(reg_t*)CLINT_MTIME + interval; #把下次中斷的時間設定好

  // set the machine-mode trap handler.
  w_mtvec((reg_t)sys_timer);   #下次中斷要做哪個函數設定好，設定好後，時間到就會中斷，就會把mepc儲存起來，再跳到sys_timer(組合語言那邊)。

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}

static int timer_count = 0;  

reg_t timer_handler(reg_t epc, reg_t cause)  #時間中斷會透過sys_timer執行timer_handler
{

  reg_t return_pc = epc; #保存暫存器
  // disable machine-mode timer interrupts.
  w_mie(~((~r_mie()) | (1 << 7)));  #禁止中斷
  lib_printf("timer_handler: %d\n", ++timer_count);  #前面有設定 timer_count = 0 ，每次呼叫timer_handler時，timer_count就會加1，從1開始印
  #如果這行的程式碼是timer_count++，就會從0開始印，先取值在加
  int id = r_mhartid();
  *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval; //設置下次中斷時間點，中斷時會呼叫sys_timer，繼續一樣的動作，這樣才會連續間隔時間印出timer_handler
  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);   #允許中斷
  #至於為甚麼要禁止中斷再來允許中斷，此用意是不希望在處理中斷的時間之內又發生中斷，如果在處理中斷的時間又發生中斷，這種就做可從入式中斷，但在這程式是不允許的，因此要處理中斷的時候，一開始就要先禁止中斷，等到最後在允許中斷。
  return return_pc;
}
```
### mini-riscv-os/04-TimerInterrupt/sys.s:
### 原本是叫sys_switch，當時間中斷到時，會先讀取機器模式程式計數器備份和中斷原因，並呼叫timer_handler(c語言的函數)，再設定成mepc並還原，呼叫mret時會跳到原來的中斷
### 主要目的為保存暫存器，並呼叫c語言的timer_handler，在恢復暫存器，最後就返回，唯一有動作的地方是會在timer_handler這部分執行。
```
sys_timer:

	# call the C timer_handler(reg_t epc, reg_t cause)
	csrr	a0, mepc     #mepc=機器模式程式計數器備份，mepc是硬體發生中斷所產生的，但在哪裡發生中斷式不一定的
	csrr	a1, mcause   #mcause=機器中斷原因
	call	timer_handler  

	# timer_handler will return the return address via a0.
	csrw	mepc, a0    #設定成mepc，並還原

	mret # back to interrupt location (pc=mepc)   #當中斷時會自動儲存mepc，也就是呼叫mret時，pc=mepc，就會跳到原本被中斷的位置
```
### mini-riscv-os/04-TimerInterrupt/riscv.h:
```
static inline reg_t r_mhartid()
{
  reg_t x;
  asm volatile("csrr %0, mhartid" : "=r" (x) );  #asm指此括號裡面都是組合語言，volatile指這些組合語言可能會造成暫存器突然的改變，這些暫存器不能假設如果編譯器沒有動他就不會自己動，csrr函數指把內容讀取到x裡面
  return x;
}


static inline reg_t r_mstatus()  #r_mstatus指在讀取機器狀態
{
  reg_t x;
  asm volatile("csrr %0, mstatus" : "=r" (x) );
  return x;
}


static inline void w_mstatus(reg_t x)  # w_mstatus指在寫入機器狀態
{
  asm volatile("csrw mstatus, %0" : : "r" (x));  
}


static inline void w_mepc(reg_t x) #w_mepc指在寫入機器模式程式計數器備份，epc指當中斷時程式計數器會先丟給epc，等中斷完成後會把epc塞還給epc再跳回來
{
  asm volatile("csrw mepc, %0" : : "r" (x));
}


static inline reg_t r_mepc()  # r_mepc指在讀取機器模式程式計數器備份
{
  reg_t x;
  asm volatile("csrr %0, mepc" : "=r" (x));
  return x;
}

static inline void w_mscratch(reg_t x)  #mscratch指時間中斷臨時變數儲存的地方
{
  asm volatile("csrw mscratch, %0" : : "r" (x));
}


static inline void w_mtvec(reg_t x)  #mtvec指中斷向量，中斷向量儲存一些中斷位置
{
  asm volatile("csrw mtvec, %0" : : "r" (x));
}
剩下的部分都是為了暫存器的某些位元而使用的
```
![](https://drive.google.com/uc?export=view&id=1IvsigljeEKAmyJ7-amC4cSenbPgvwqW_)

