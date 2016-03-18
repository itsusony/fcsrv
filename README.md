# fcsrv
Frequency capping control server for mobile application or anything has a unique ID. like IDFA or just a site id.

# what's it?
You can use this fcsrv to count an ID's daily access. The fcsrv receives a commit in a day from an ID, and just count 1 per day.
You can use this to tracking an ID's frequency information.
It is very fast (use udp protocol at 100,000 rps or tcp/ip http tunnel about 20,000 rps), and just use small amount of memory (1 million ids, per id with 7 days count, at 1GB memory)

# how to compile
`make`

binary files will be compiled in bin/fcsrv and bin/client(for test)

# how to use it?

* commit an ID into fcsrv
 - (commit data with udp protocol to port 3903 contains an ID string), recommend to use udp.
 - use tcp/ip api commit: `curl "http://127.0.0.1:3903/?c=commit&id=f81d4fae-7dec-11d0-a765-00a0c91e6bf6"`

* get an ID's frequency date list
 - use tcp/ip api to get: `curl "http://127.0.0.1:3903/?c=get&id=f81d4fae-7dec-11d0-a765-00a0c91e6bf6"`
 - you will get a response like: `20151225,20160317,20160318,`. you can use these in other application's logic
 
* dump memory data to a file in HD for resume.
 - It will dump at background, will not lock your application during dumping. `curl "http://127.0.0.1:12345/?c=sys_dump"`
 - when you restart your fcsrv, it will read the dump file into memory again. you don't need worry about your data after restart server.
 
# TODO

- master/slave support
- unix domain socket support
- daily's count number support. (at now, fcsrv just save date's ymd and ignore access in same day.)
