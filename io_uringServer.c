#include <stdio.h>
#include <liburing.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
 
#define EVENT_ACCEPT 0
#define EVENT_READ 1
#define EVENT_WRITE 2
 
//结构体用于保存与 I/O 操作相关的连接信息，
//其中 fd 保存文件描述符（即套接字），event 保存当前操作的类型（如读、写、接受连接等）。
struct conn_info
{
    int fd;
    int event;
};

int init_server(unsigned short port)
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    if(bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        return -1;
    }
    listen(sockfd,10);

    return sockfd;
}

//ENTRIES_LENGTH 定义了提交队列和完成队列的大小，表示可以同时处理的最大 I/O 操作数量。
#define ENTRIES_LENGTH 1024
#define BUFFER_LENGTH 1024

//该函数准备一个接收事件（EVENT_READ），将该事件与传入的套接字和缓冲区相关联，并将其提交到 io_uring。
int set_event_recv(struct io_uring *ring,int sockfd,void *buf,size_t len,int flags)
{
    //ring 是一个指向已经初始化的 io_uring 结构体的指针。
    //io_uring_sqe 结构体表示一个提交队列元素
    //io_uring_get_sqe 返回的是一个指向 io_uring_sqe 结构体的指针，它代表一个可以填充的提交队列元素。
        //调用此函数后，你可以通过设置该指针指向的 SQE 结构体的字段来指定具体的 I/O 操作（比如读、写操作、文件描述符、内存缓冲区等）。
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    struct conn_info accept_info = 
    {
        .fd = sockfd,
        .event = EVENT_READ,
    };

    io_uring_prep_recv(sqe,sockfd,buf,len,flags);
    memcpy(&sqe->user_data,&accept_info,sizeof(struct conn_info));
}

//该函数准备一个发送事件（EVENT_WRITE），用于将数据发送到客户端。
int set_event_send(struct io_uring *ring, int sockfd,void *buf, size_t len, int flags)
{
 
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
 
	struct conn_info accept_info = {
		.fd = sockfd,
		.event = EVENT_WRITE,
	};
 
	io_uring_prep_send(sqe, sockfd, buf, len, flags);
	memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}

//该函数准备一个接收连接事件（EVENT_ACCEPT），将其与套接字相关联，表示等待新的客户端连接。
int set_event_accept(struct io_uring *ring, int sockfd, struct sockaddr *addr,socklen_t *addrlen, int flags)
{
 
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
 
	struct conn_info accept_info = {
		.fd = sockfd,
		.event = EVENT_ACCEPT,
	};
 
	io_uring_prep_accept(sqe, sockfd, (struct sockaddr *)addr, addrlen, flags);
	memcpy(&sqe->user_data, &accept_info, sizeof(struct conn_info));
}

int main(int argc,char *argv[])
{
    unsigned short port = 9999;
    int sockfd = init_server(port);

    //io_uring环境初始化
    struct io_uring_params params;
    memset(&params,0,sizeof(params));
    //初始化了一个 io_uring 实例，这个实例将用于管理所有的异步 I/O 操作。
    struct io_uring ring;
	io_uring_queue_init_params(ENTRIES_LENGTH, &ring, &params);

#if 0
	struct sockaddr_in clientaddr;	
	socklen_t len = sizeof(clientaddr);
	accept(sockfd, (struct sockaddr*)&clientaddr, &len);
#else
    struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
    //set_event_accept 函数将一个 accept 操作添加到 io_uring 的提交队列中。这个操作用于接受客户端连接请求。
    //这一步是服务器启动时的初始操作，它告诉 io_uring 开始监听并处理客户端连接。
	set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);
#endif
    char buffer[BUFFER_LENGTH] = {0};
    //主循环：提交操作和处理完成事件
    //在 while(1) 循环中，服务器会不断处理 I/O 操作，首先通过 set_event_accept() 等待客户端连接。
    while(1)
    {
        /*
        1/获取 SQE：首先，调用 io_uring_get_sqe(ring) 从提交队列中获取一个空的 SQE。
        2/填充 SQE：接下来，你可以填充这个 SQE，指定具体的 I/O 操作。例如，如果你要发起一个异步读操作，你会设置相应的字段，比如文件描述符、缓冲区地址、读取的字节数等。
        3/提交 SQE：最后，通过调用 io_uring_submit(ring) 来将这个已填充的 SQE 提交到内核，启动 I/O 操作。
        */
        //将之前添加到提交队列中的所有操作提交给内核，由内核异步执行这些操作。
        io_uring_submit(&ring);
        struct io_uring_cqe *cqe;
        //等待至少一个操作完成，这是一个阻塞调用。
        io_uring_wait_cqe(&ring,&cqe);

        // 批量获取已经完成的操作结果，nready 表示完成的操作数量
        struct io_uring_cqe *cqes[128];
        int nready = io_uring_peek_batch_cqe(&ring, cqes, 128);

        //处理完成的事件
        for(int i = 0; i < nready; i++)
        {
            //从完成队列（Completion Queue, CQ）中获取某个已完成的 I/O 操作的完成队列元素
            struct io_uring_cqe *entries = cqes[i];
            struct conn_info result;
            memcpy(&result,&entries->user_data,sizeof(struct conn_info));
            //处理 accept 事件。当一个新的客户端连接到来时，io_uring 完成队列会返回 EVENT_ACCEPT 事件，表示一个新的连接已经建立。
            //此时，服务器会：
            //重新设置 accept 事件，继续监听新的客户端连接。
            //获取新连接的文件描述符 connfd，并设置一个 recv 事件来准备接收数据。
            if (result.event == EVENT_ACCEPT)
            {
                set_event_accept(&ring, sockfd, (struct sockaddr *)&clientaddr, &len, 0);
                printf("set_event_accept\n");
                int connfd = entries->res;
                set_event_recv(&ring, connfd, buffer, BUFFER_LENGTH, 0);
            }
            //EVENT_READ：处理 recv 事件。当从客户端接收到数据时，io_uring 返回 EVENT_READ 事件。如果接收到的数据长度大于0，
            //则会设置一个 send 事件来将数据发送回客户端。如果 ret == 0，说明客户端关闭了连接，则关闭文件描述符。
            else if (result.event == EVENT_READ)
            {
                int ret = entries->res;
                 printf("set_event_recv ret: %d, %s\n", ret, buffer);
                if (ret == 0)
                {
                    close(result.fd);
                }
                else if (ret > 0)
                {
                    set_event_send(&ring, result.fd, buffer, ret, 0);
                }
            }
            //EVENT_WRITE：处理 send 事件。当数据成功发送给客户端后，io_uring 返回 EVENT_WRITE 事件。
            //此时，服务器会再次设置一个 recv 事件，准备接收更多数据。
            else if (result.event == EVENT_WRITE)
            {
                int ret = entries->res;
                printf("set_event_send ret: %d, %s\n", ret, buffer);
                set_event_recv(&ring, result.fd, buffer, BUFFER_LENGTH, 0);
            }
        }
        //完成队列的推进
        //这个函数通知 io_uring，你已经处理完了 nready 个完成队列条目（CQE）。io_uring 可以释放这些 CQE 供后续操作使用。
        io_uring_cq_advance(&ring, nready);
    }
}

/*
io_uring 的作用：在这个示例中，io_uring 被用来高效地处理网络 I/O 操作。通过异步提交和处理 accept、recv、send 操作，服务器能够高效处理多个并发连接，而无需阻塞等待每个I/O操作完成。
异步模型：io_uring 提供了一种低延迟、高并发的异步 I/O 处理方式。操作在提交后由内核异步执行，完成后再由应用程序查询并处理结果。这种方式大大减少了系统调用的开销，提高了程序的并发处理能力。

提交操作：使用 io_uring_prep_* 函数准备操作，并提交给内核处理。
等待完成：使用 io_uring_wait_cqe 等方法等待操作完成，并获取结果。
处理结果：根据完成队列中的事件类型（如 EVENT_ACCEPT、EVENT_READ、EVENT_WRITE）进行相应的处理和后续操作。
*/