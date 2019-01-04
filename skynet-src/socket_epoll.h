#ifndef poll_socket_epoll_h
#define poll_socket_epoll_h

#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

// 用于判断产生的 epoll fd 是否有效
static bool 
sp_invalid(int efd) {
	return efd == -1;
}

// 用于产生一个 epoll fd，1024是用来建议内核监听的数目，自从 linux 2.6.8 之后，该参数是被忽略的，即可以填大于0的任意值。
static int
sp_create() {
	return epoll_create(1024);
}

// 释放 epoll fd
static void
sp_release(int efd) {
	close(efd);
}

/*
 * 为 epoll 添加一个监听的文件描述符，仅监控读事件
 * fd    : sp_create() 返回值
 * sock  : 待监听的文件描述符
 * ud    : 自己使用的指针地址
 *       : 返回0表示添加成功, -1表示失败
 */
static int 
sp_add(int efd, int sock, void *ud) {
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ud;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &ev) == -1) {
		return 1;
	}
	return 0;
}

/*
 * 删除 epoll 中监听的 fd
 * fd    : sp_create()创建的fd
 * sock  : 待删除的fd
 */
static void 
sp_del(int efd, int sock) {
	epoll_ctl(efd, EPOLL_CTL_DEL, sock , NULL);
}

/*
 * 修改 epoll 中已有 fd 的监听事件
 * efd   : epoll fd
 * sock  : 待修改的fd
 * ud    : 用户自定义数据指针
 * enable: true表示开启写监听, false表示还是读监听
 */
static void 
sp_write(int efd, int sock, void *ud, bool enable) {
	struct epoll_event ev;
	ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
	ev.data.ptr = ud;
	epoll_ctl(efd, EPOLL_CTL_MOD, sock, &ev);
}

/*
 * 轮询fd事件
 * efd   : sp_create()创建的fd
 * e     : 一段struct event内存的首地址
 * max   : e内存能够使用的最大值
 *       : 返回监听到事件的fd数量，write与read分别对应写和读事件flag，值为true时表示该事件发生
 */
static int 
sp_wait(int efd, struct event *e, int max) {
	struct epoll_event ev[max];
	int n = epoll_wait(efd , ev, max, -1);
	int i;
	for (i=0;i<n;i++) {
		e[i].s = ev[i].data.ptr;
		unsigned flag = ev[i].events;
		e[i].write = (flag & EPOLLOUT) != 0;
		e[i].read = (flag & (EPOLLIN | EPOLLHUP)) != 0;
		e[i].error = (flag & EPOLLERR) != 0;
	}

	return n;
}

/*
 * 将fd设置为非阻塞
 */
static void
sp_nonblocking(int fd) {
	int flag = fcntl(fd, F_GETFL, 0);
	if ( -1 == flag ) {
		return;
	}

	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

#endif
