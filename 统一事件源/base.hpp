#ifndef _BASE_H
#define _BASE_H

#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <netdb.h>
#include <math.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>

class CallFailed
{
  public:
    explicit CallFailed(const std::string &s, const int &line) : ErrString(s), LineNo(line)
    {
        std::cout << ErrString << LineNo << std::endl;
        std::cout << "errno == " << errno << std::endl;
        perror("The reason is :");
        exit(0);
    }
    std::string ErrString;
    int LineNo;
};

class other_error : public std::logic_error
{
    /*等待填充*/
};
#endif