#include "datetime.h"

void printCurrentTime()
{
time_t currentTime = time(NULL); //取得當前時間戳
printf("Current time: %s", ctime(&currentTime)); //顯示出現在的時間
}
