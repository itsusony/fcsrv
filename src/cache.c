JD** jobs; //raw datas
unsigned long long int jobs_len=0;
pthread_mutex_t mutex_jobs_lock = PTHREAD_MUTEX_INITIALIZER; // 同時書き込み＆読み取りどちらもロックが必要です。
void init_jobs(){jobs = malloc(OKU*sizeof(JD*));jobs_len=0;}
void insert_jobs(const char* idfa){
    JD *jd = malloc(sizeof(JD));
    jd->id = strdup(idfa);
    jd->time = (long long int)time(NULL);
    jobs[jobs_len++]=jd;
}
void free_job(JD* jd){free(jd->id);free(jd);}



HJDS* jobsum=NULL;
pthread_mutex_t mutex_jobsum_write_lock = PTHREAD_MUTEX_INITIALIZER; // 同時書き込みはロック必要ですが、同時readはおk

void insert_jobsum(JD* jd){
    HJDS* exist_jds = NULL;
    HASH_FIND_STR(jobsum,jd->id,exist_jds);

    struct tm *tst = localtime(&(jd->time));
    HJDATE *jdt = malloc(sizeof(HJDATE));
    jdt->ymd = malloc(8*sizeof(char));
    sprintf(jdt->ymd,"%04d%02d%02d",tst->tm_year+1900,tst->tm_mon+1,tst->tm_mday);
    if(!exist_jds){
        HJDS *newjds = malloc(sizeof(HJDS));
        newjds->id = strdup(jd->id);
        newjds->times = NULL;
        HASH_ADD_STR(newjds->times,ymd,jdt);
        HASH_ADD_STR(jobsum,id,newjds);
    }else{
        HJDATE *find_jdt=NULL;
        HASH_FIND_STR(exist_jds->times,jdt->ymd,find_jdt);
        if(!find_jdt){
            // todo check MAX_DAYS
            HASH_ADD_STR(exist_jds->times,ymd,jdt);
        }else{
            free(jdt->ymd);
            free(jdt);
        }
    }
}
