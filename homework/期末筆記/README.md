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


1.36.40
