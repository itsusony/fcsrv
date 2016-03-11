#include <errno.h>
#include <event.h>
#include <evhttp.h>

void api_callback(struct evhttp_request* req, void *arg){
    struct evbuffer *buf;
    buf = evbuffer_new();
    if(buf == NULL)return;

    evhttp_add_header(req->output_headers,"Content-Type","text/plain; charset=utf-8");
    evhttp_add_header(req->output_headers,"Pragma","no-cache");
    evhttp_add_header(req->output_headers,"Cache-Control","no-cache");
    evhttp_add_header(req->output_headers,"Expires","Thu, 01 Dec, 1994 16:00:00 GMT");
    evhttp_add_header(req->output_headers,"Connection","close");

    struct evkeyvalq keys;
    evhttp_parse_query(req->uri,&keys);
    const char *cmd = evhttp_find_header(&keys,"c");
    const char *id = evhttp_find_header(&keys,"id");

    if(cmd == NULL){
        evbuffer_add_printf(buf,"ng");
    }else if(strncmp(cmd,"ping",4)==0){
        evbuffer_add_printf(buf,"pong");
    }else if(strncmp(cmd,"get",3)==0){
        if(!id || strlen(id)==0)goto invalidaccess;
        
        HJDS *job=NULL;
        HJDATE *cur,*tmp;
        char *output=NULL;

        HASH_FIND_STR(jobsum, id, job);
        if(job){
            int cnt = HASH_COUNT(job->times);
            if(cnt>0){
                output = calloc(cnt,sizeof(char)*10); // len(YYYYMMDD,)+1 * cnt
                HASH_ITER(hh,job->times,cur,tmp){
                    strcat(output,cur->ymd);
                    strcat(output,",");
                }
            }
        }

        evbuffer_add_printf(buf,output ? output : "");
        free(output);
    }else if(strncmp(cmd,"sys_dump",8)==0){
        pthread_mutex_lock(&mutex_dumpsignal_lock);
        _dumpreq_signal_from_api = 1;
        pthread_mutex_unlock(&mutex_dumpsignal_lock);
        evbuffer_add_printf(buf,"ok");
    }else{
        invalidaccess:
        evhttp_send_error(req,501,"invalid request");
        evhttp_clear_headers(&keys);
        evbuffer_free(buf);
        return;
    }
    evhttp_send_reply(req,HTTP_OK,"",buf);
    evhttp_clear_headers(&keys);
    evbuffer_free(buf);
}

void create_api_server(){
    event_init();
    struct event_base *evbase = NULL;
    struct evhttp *evh = evhttp_new(evbase);
    if(evhttp_bind_socket(evh,"0.0.0.0",12345)!=0){
        printf("Error! Can't listen to server");
        exit(1);
    }else{
        evhttp_set_gencb(evh,api_callback,NULL);
        event_dispatch();
    }
}
