/* 
 * @Author: Zheng Yunkun
 * @Date: 2024-8-1 00:48:45
 * @LastEditTime: 2024-8-23 17:30:00
 * @LastEditors: Larry Kinn
 * @Description: Linux Server Start up
 */

#include "src/EventLoop.h"
#include "src/Server.h"

int main()
{
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    loop->loop();   // 事件驱动开始
    return 0;
}