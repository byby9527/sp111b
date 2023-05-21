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

## RISC-V 指令格式:
![](https://github.com/riscv2os/riscv2os/wiki/img/riscv_format32_2.png)



