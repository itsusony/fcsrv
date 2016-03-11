#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void* thread_listen(void* args){
    int sd;
    struct sockaddr_in listen_addr;
    socklen_t sin_size;
    struct sockaddr_in from_addr;

    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        fprintf(stderr,"socket create error!!");
        exit(1);
    }

    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(udp_port);
    listen_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        fprintf(stderr,"bind error!!");
        exit(1);
    }

    char buf[BUF_SIZE]="";
    while(1){
        memset(buf,0,BUF_SIZE);
        if(recvfrom(sd, buf, BUF_SIZE, 0, (struct sockaddr *)&from_addr, &sin_size) < 0)continue;
        pthread_mutex_lock(&mutex_jobs_lock);
        insert_jobs(buf);
        pthread_mutex_unlock(&mutex_jobs_lock);
    }
    close(sd);
}

void* thread_process(void* args){
    while(1){
        JD** oldjobs = NULL;
        unsigned long long int oldjobs_len=0;
        pthread_mutex_lock(&mutex_jobs_lock);
        if (jobs_len>0){
            oldjobs = jobs;
            oldjobs_len = jobs_len;
            init_jobs();
        }
        pthread_mutex_unlock(&mutex_jobs_lock);
        if(oldjobs_len>0){
            pthread_mutex_lock(&mutex_jobsum_write_lock);
            unsigned long long int i;for(i=0;i<oldjobs_len;i++){
                JD* jd = oldjobs[i];
                insert_jobsum(jd);
                free_job(jd);
            }
            pthread_mutex_unlock(&mutex_jobsum_write_lock);
            free(oldjobs);
        }
        sleep(1);
    }
}

pthread_mutex_t mutex_dump_lock = PTHREAD_MUTEX_INITIALIZER; // dump前にロックが必要です。
void _thread_dump(){
    pthread_mutex_lock(&mutex_dump_lock);
    dump_save();
    pthread_mutex_unlock(&mutex_dump_lock);
}

void* thread_dump(void* args){
    while(1){
        sleep(DUMP_TIMEGAP);
        _thread_dump();
    }
}

pthread_mutex_t mutex_dumpsignal_lock = PTHREAD_MUTEX_INITIALIZER;
int _dumpreq_signal_from_api = 0;
void* thread_dump_from_api(void* args){
    while(1){
        int signal = 0;
        pthread_mutex_lock(&mutex_dumpsignal_lock);
        signal = _dumpreq_signal_from_api;
        _dumpreq_signal_from_api=0;
        pthread_mutex_unlock(&mutex_dumpsignal_lock);
        if(signal)_thread_dump();
        sleep(1);
    }
}
