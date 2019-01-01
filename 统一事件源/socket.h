#ifndef _SERVSOCKET_H
#define _SERVSOCKET_H

#include "base.hpp"

class BaseSocket
{
  public:
    BaseSocket() //默认构造函数
    {
        base_socket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (base_socket_ < 0)
            throw CallFailed(" socket.h 文件: socket create failed !!! at line  ", __LINE__);
    }
    BaseSocket(const int fd) { base_socket_ = fd; }
    //拷贝构造函数
    BaseSocket(const BaseSocket &rfs) { base_socket_ = rfs.base_socket_; }

    BaseSocket(BaseSocket &&) = delete;                 //移动构造函数
    BaseSocket &operator=(BaseSocket &&) = delete;      //移动赋值操作符
    BaseSocket &operator=(const BaseSocket &) = delete; //赋值操作符
    ~BaseSocket()
    {
        close(base_socket_);
    }
    inline void SetBaseSocket(const int &fd) { base_socket_ = fd; }
    inline int GetBaseSocket() { return base_socket_; }

    inline void Close()
    {
        if (close(base_socket_) == -1)
            throw CallFailed(" socket.h 文件: close function failed !!! at line  ", __LINE__);
    }
    inline int Send(const void *buf, size_t len, int flags = SOCK_NONBLOCK)
    {
    }

    inline int Recv(void *buffer, size_t length, int &index, int flags = SOCK_NONBLOCK)
    {
    }

  protected:
    int base_socket_ = -1;
};

class ServSocket : public BaseSocket
{
    /*继承了 base_socket_ */
  public:
    ServSocket() = delete;
    ServSocket(const ServSocket &) = delete;
    ServSocket &operator=(const ServSocket &) = delete;
    ServSocket(ServSocket &&) = delete;            //移动构造函数
    ServSocket &operator=(ServSocket &&) = delete; //移动赋值操作符

    explicit ServSocket(const char *ip, const int port)
    {
        bzero(&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &serv_addr.sin_addr);
        serv_addr.sin_port = htons(port);
    }
    inline void SetReuse()
    {
        int optval = 1;
        if (setsockopt(base_socket_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0)
            throw CallFailed(" socket.h 文件: setsockopt function failed !!! at line  ", __LINE__);
    }
    inline int Bind()
    {
        /* Bind(listenfd, (struct sockaddr *)&address, sizeof(address));**/
        if (bind(base_socket_, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0)
            throw CallFailed(" socket.h 文件: bind function failed !!! at line  ", __LINE__);
        return (base_socket_);
    }
    inline int Listen()
    {
        int backlog = 1024;
        char *ptr = NULL;
        if ((ptr = getenv("LISTENQ")) != NULL)
            backlog = atoi(ptr);

        if (listen(base_socket_, backlog) < 0)
            throw CallFailed(" socket.h 文件: listen function failed !!! at line  ", __LINE__);
        return (base_socket_);
    }
    inline int Accept(struct sockaddr *sa, socklen_t *salenptr)
    {
        int n;
    again:
        if ((n = accept(base_socket_, sa, salenptr)) < 0)
        {
            /*连接在listen后建立，在accept前夭折，此时accept可能会返回错误，对此必须处理,*/
#ifdef EPROTO
            if (errno == EPROTO || errno == ECONNABORTED || errno == EWOULDBLOCK)
#else
            if (errno == ECONNABORTED)
#endif
                goto again;
            else
                throw CallFailed(" socket.h 文件: accept function failed !!! at line  ", __LINE__);
        }
        return (n);
    }

  private:
    struct sockaddr_in serv_addr;
};
#endif