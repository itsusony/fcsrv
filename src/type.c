typedef struct jobdata {
    char* id; //idfa
    time_t time; // time_t
} JD;
typedef struct jobdate {
    char* ymd; //20121231, key
    UT_hash_handle hh;
} HJDATE; // hash
typedef struct jobsummary {
    char* id; //idfa, key
    HJDATE *times;
    UT_hash_handle hh;
} HJDS; // hash
