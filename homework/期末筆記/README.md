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




