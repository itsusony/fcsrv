#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <syslog.h>

#include "../lib/uthash.h"

int udp_port = 3903;

#include "define.c"
#include "type.c"
#include "cache.c"

#include "dump.c"
#include "threadworker.c"
#include "api.c"

void read_args(int argc, char** argv){
    int result;
    while((result=getopt(argc,argv,"su:"))!=-1){
        switch(result){
            case 'u':
                udp_port = atoi(optarg);
                if (udp_port <= 0){
                    fprintf(stderr,"正しいポート番号を指定してください。");
                    exit(1);
                }
                break;
            case ':':
                fprintf(stderr,"%c の値を指定してください。\n",result);
                break;
            case '?':
                fprintf(stderr,"不明なオプションです。\n");
                break;
            break;
        }
    }
}

int main(int argc, char** argv){
    read_args(argc,argv);
    init_jobs();
    dump_load();
    pthread_t th1,th2,th3,th4;
    pthread_create(&th1,NULL,thread_listen,(void*)NULL);
    pthread_create(&th2,NULL,thread_process,(void*)NULL);
    pthread_create(&th3,NULL,thread_dump,(void*)NULL);
    pthread_create(&th4,NULL,thread_dump_from_api,(void*)NULL);
    create_api_server();
    return 0;
}
