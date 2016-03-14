void dump_save(){
    FILE *fp;
    if ((fp=fopen(DUMPFILE,"w"))==NULL){
        openlog(APPNAME, LOG_CONS | LOG_PID, LOG_USER);
        syslog(LOG_CRIT, "dump_save file write open error!");
        closelog();
        return;
    }
    if(HASH_COUNT(jobsum)>0){
        HJDS *cur,*tmp;
        HJDATE *cur2,*tmp2;
        HASH_ITER(hh,jobsum,cur,tmp){
            fprintf(fp,"<%s>",cur->id);
            HASH_ITER(hh,cur->times,cur2,tmp2){
                fprintf(fp,"%s,",cur2->ymd);
            }
            fprintf(fp,"\n");
        }
    }
    fclose(fp);
}
void dump_load(){
    FILE *fp;
    if ((fp=fopen(DUMPFILE,"r"))==NULL){
        openlog(APPNAME, LOG_CONS | LOG_PID, LOG_USER);
        syslog(LOG_NOTICE, "dump_save file read open error!");
        closelog();
        return;
    }
    int linesize = 1024*256; 
    char *line = malloc(linesize * sizeof(char)); //100mb for one line
    char idfa[1024]="",vals[102400]="";
    char *saveptr=NULL;
    long logcnt = 0;
    pthread_mutex_lock(&mutex_jobsum_write_lock);
    while(fgets(line, linesize, fp) != NULL) {
        sscanf(line,"<%[^<>]>%[^\n]",idfa,vals);
        if (strlen(idfa) == 0 || strlen(vals) == 0)continue;
        HJDS* job = malloc(sizeof(HJDS));
        job->id = strdup(idfa);
        job->times = NULL;
        char *ymd = strtok_r(vals,",", &saveptr);
        while(ymd){
            HJDATE* dt = malloc(sizeof(HJDATE));
            dt->ymd = strdup(ymd);
            HASH_ADD_STR(job->times,ymd,dt);
            ymd = strtok_r(NULL,",",&saveptr);
        }
        HASH_ADD_STR(jobsum,id,job);
        logcnt++;
    }
    pthread_mutex_unlock(&mutex_jobsum_write_lock);
    fclose(fp);
    free(line);
    openlog(APPNAME, LOG_CONS | LOG_PID, LOG_USER);
    syslog(LOG_NOTICE, "dump_load finished. total count is %ld",logcnt);
    closelog();
}
