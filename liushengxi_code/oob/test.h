/*************************************************************************
	> File Name: test.h
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月18日 星期二 11时44分27秒
 ************************************************************************/

#ifndef _TEST_H
#define _TEST_H

void heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg);
void heartbeat_cli(int servfd_arg, int nsec_arg, int maxnprobes_arg);

#endif
