// 我们的创建的服务，需要通过消息来驱动，而一个服务要获取消息，是从消息队列里取的。skynet包含两级消息队列，
// 一个global_mq，他包含一个head和tail指针，分别指向次级消息队列的头部和尾部，另外还有一个次级消息队列，这个一个单向链表。
// 消息的派发机制是，工作线程，会从global_mq里pop一个次级消息队列来，然后从次级消息队列中，pop出一个消息，并传给context的callback函数，
// 在完成驱动以后，再将次级消息队列push回global_mq中

#ifndef SKYNET_MESSAGE_QUEUE_H
#define SKYNET_MESSAGE_QUEUE_H

#include <stdlib.h>
#include <stdint.h>

struct skynet_message {
	uint32_t source;			// 消息发送方的服务地址
	// 如果这是一个回应消息，那么要通过session找回对应的一次请求，在lua层，我们每次调用call的时候，都会往对  
    // 方的消息队列中，push一个消息，并且生成一个session，然后将本地的协程挂起，挂起时，会以session为key，协程句  
    // 柄为值，放入一个table中，当回应消息送达时，通过session找到对应的协程，并将其唤醒。
	int session;
	void * data;				// 消息地址
	size_t sz;					// 消息大小
};

// type is encoding in skynet_message.sz high 8bit
#define MESSAGE_TYPE_MASK (SIZE_MAX >> 8)
#define MESSAGE_TYPE_SHIFT ((sizeof(size_t)-1) * 8)

struct message_queue;

void skynet_globalmq_push(struct message_queue * queue);
struct message_queue * skynet_globalmq_pop(void);

struct message_queue * skynet_mq_create(uint32_t handle);
void skynet_mq_mark_release(struct message_queue *q);

typedef void (*message_drop)(struct skynet_message *, void *);

void skynet_mq_release(struct message_queue *q, message_drop drop_func, void *ud);
uint32_t skynet_mq_handle(struct message_queue *);

// 0 for success
int skynet_mq_pop(struct message_queue *q, struct skynet_message *message);
void skynet_mq_push(struct message_queue *q, struct skynet_message *message);

// return the length of message queue, for debug
int skynet_mq_length(struct message_queue *q);
int skynet_mq_overload(struct message_queue *q);

void skynet_mq_init();

#endif
