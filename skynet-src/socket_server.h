#ifndef skynet_socket_server_h
#define skynet_socket_server_h

#include <stdint.h>

// 宏定义socket_server_poll()返回的socket消息类型
#define SOCKET_DATA 0			//数据data到来
#define SOCKET_CLOSE 1			//关闭连接
#define SOCKET_OPEN 2			//多处用到，参见代码
#define SOCKET_ACCEPT 3			//被动连接建立
#define SOCKET_ERR 4			//错误
#define SOCKET_EXIT 5			//退出socket
#define SOCKET_UDP 6			//udp通信
#define SOCKET_WARNING 7

struct socket_server;

struct socket_message {
	int id;
	uintptr_t opaque;		// 在skynet中对应一个Actor实体的handle句柄
	int ud;	// for accept, ud is new connection id ; for data, ud is size of data 
	char * data;
};

//创建一个socket_server
struct socket_server * socket_server_create();
//释放一个socket_server的资源占用
void socket_server_release(struct socket_server *);
/*
* 封装了的epoll或kqueue，用来获取socket的网络事件或消息
* （通常放在循环体中持续监听网络消息）
* socket_server : socket_server_create() 返回的socket_server实例
* result        : 结果数据存放的地址指针
*               : 返回消息类型，对应于宏定义中的SOCKET_DATA的类型
*/
int socket_server_poll(struct socket_server *, struct socket_message *result, int *more);

//退出socket_server
void socket_server_exit(struct socket_server *);

/*  
* 关闭socket_server
* socket_server : socket_server_create() 返回的socket_server实例
* opaque        : skynet中服务handle的句柄
* id            : socket_server_listen() 返回的id
*/
void socket_server_close(struct socket_server *, uintptr_t opaque, int id);

/*  
* 停止socket
* socket_server : socket_server_create() 返回的socket_server实例
* opaque        : skynet中服务handle的句柄
* id            : socket句柄
*/
void socket_server_shutdown(struct socket_server *, uintptr_t opaque, int id);

/*  
* 将该socket放入epoll中监听（启动之前要先通过socket_server_listen()开启TCP的socket(),bind(),listen()步骤）
* 或将服务器 report_accept() 后的socket放入epoll中监听。总之，对于socket的fd，想要收发数据，都得先调用 socket_server_start()
* socket_server : socket_server_create() 返回的socket_server实例
* opaque        : skynet中服务handle的句柄
* id            : socket_server_listen() 返回的id
*/
void socket_server_start(struct socket_server *, uintptr_t opaque, int id);

/*
* 发送数据
* socket_server : socket_server_create() 返回的socket_server实例
* buffer        : 要发送的数据
* sz            : 数据的大小
* id            : socket_server_listen() 返回的id
*               : 假如返回-1表示error
*/
// return -1 when error
int socket_server_send(struct socket_server *, int id, const void * buffer, int sz);
int socket_server_send_lowpriority(struct socket_server *, int id, const void * buffer, int sz);

/*  
* 开启TCP监听，执行了socket(),bind(),listen() 步骤 
* socket_server : socket_server_create() 返回的socket_server实例
* opaque        : skynet中服务handle的句柄
* addr          : ip地址
* port          : 端口号
*               : 返回一个id作为操作此端口监听的句柄        
*/
// ctrl command below returns id
int socket_server_listen(struct socket_server *, uintptr_t opaque, const char * addr, int port, int backlog);
/*  
* 以非阻塞的方式连接服务器
* socket_server : socket_server_create() 返回的socket_server实例
* opaque        : skynet中服务handle的句柄
* addr          : ip地址
* port          : 端口号
*               : 返回一个id作为操作此端口监听的句柄        
*/
int socket_server_connect(struct socket_server *, uintptr_t opaque, const char * addr, int port);
/*  
* 并不对应bind函数，而是将stdin、stout这类IO加入到epoll中管理
* socket_server : socket_server_create() 返回的socket_server实例
* opaque        : skynet中服务handle的句柄
* fd            : socket的文本描述       
*/
int socket_server_bind(struct socket_server *, uintptr_t opaque, int fd);

// for tcp
void socket_server_nodelay(struct socket_server *, int id);

/*
* 创建一个udp socket监听，并绑定skynet服务的handle，udp不需要像tcp那样要调用socket_server_start后才能接收消息
* 如果port != 0, 绑定socket，如果addr == NULL, 绑定 ipv4 0.0.0.0。如果想要使用ipv6，地址使用“::”，端口中port设为0
*/
struct socket_udp_address;

// create an udp socket handle, attach opaque with it . udp socket don't need call socket_server_start to recv message
// if port != 0, bind the socket . if addr == NULL, bind ipv4 0.0.0.0 . If you want to use ipv6, addr can be "::" and port 0.
int socket_server_udp(struct socket_server *, uintptr_t opaque, const char * addr, int port);
// 设置默认的端口地址，返回0表示成功
// set default dest address, return 0 when success
int socket_server_udp_connect(struct socket_server *, int id, const char * addr, int port);


/*
* 假如 socket_udp_address 是空的, 使用最后最后调用 socket_server_udp_connect 时传入的address代替
* 也可以使用 socket_server_send 来发送udp数据
*/
// If the socket_udp_address is NULL, use last call socket_server_udp_connect address instead
// You can also use socket_server_send 
int socket_server_udp_send(struct socket_server *, int id, const struct socket_udp_address *, const void *buffer, int sz);

// 获取传入消息的IP地址 address, 传入的 socket_message * 必须是SOCKET_UDP类型
// extract the address of the message, struct socket_message * should be SOCKET_UDP
const struct socket_udp_address * socket_server_udp_address(struct socket_server *, struct socket_message *, int *addrsz);

struct socket_object_interface {
	void * (*buffer)(void *);
	int (*size)(void *);
	void (*free)(void *);
};

// if you send package sz == -1, use soi.
void socket_server_userobject(struct socket_server *, struct socket_object_interface *soi);

#endif
