# Https Sever

by Matt & Chami

## Baseline Features: 

- Support for basic GET method and caching (similar to assignment 1)
- Handle the CONNECT method of HTTP (this will allow your proxy to support HTTPS as well).
- Ability to handle multiple clients at the same time. 
- Error handling
- Basic Performance Testing and Optimizations: Your proxy’s performance should 
  be comparable to the baseline (without using a proxy). 


## Additional Functionality:

- content filtering with url
- Trusted Proxy/SSL interception. Once trusted by clients, the proxy can handle SSL 
connections while being able to look at the content. The proxy can decrypt/encrypt 
content that it is getting from the server.


## Modules

`cache`: Module for cache. Currently 10 element RAM cache implemented as singly linked list.

`char_vector`: Module for expanding character array (no removal - just insertion + access). 

`client_record`: Module for representing the record of a client in our proxy.

`debugging`: Module related to debugging.

`errors`: Module related to reporting errors and closing.

`http`: Module related to HTTP protocol.

`more_file`: Module with some file I/O wrappers.

`more_math`: Module with more math functions that C doesn't provide.

`more_memory`: Module with more memory allocation functions that C doesn't provide.

`more_string`: Module with more string functions that C doesn't provide.

`more_time`: Module with more time functions that C doesn't provide.

`proxy`: Module with main proxy. 

`set`: Module providing hash set data structure

`socket_record`: Module for providing table for unused server sockets

`SSL`: Module for SSL related operations (used for implementing HTTPS)

`table`: Module providing hash map data structure

`tcp`: Module related to TCP.

`times_queue`: Module for sliding window rate limiting enforcement algorithm.

## HTTPS files

`proxy_key_cert/`: contains the private key and certificate used by the proxy

`root_key_cert/`: contains the CAkey and CApem used to sign certs

`fd.csr`: dummy certificate signing request used for dynamic cert generation


## Starting Proxy

Run `./startproxy`.

## Submitting Project

Baseline:

```
cd .. (out of git repo)
tar --exclude='.*' --exclude='*.o' --exclude='*.out' -cvf cs112-finalproject.tar.gz cs112-finalproject
provide comp112 CheckPoint1 cs112-finalproject.tar.gz
```

Checkpoint 2: 

```
cd .. (out of git repo)
tar --exclude='.*' --exclude='*.o' --exclude='*.out' -cvf cs112-finalproject_2.tar.gz cs112-finalproject
provide comp112 CheckPoint2 cs112-finalproject_2.tar.gz
```

## Testing Proxy

### Unit tests

For all components that could be tested independently (string, time, http operations, data structures), run `./run_unit_test`.

### Simple tests

Run `./test01` followed by one of `text1`, `text2`, or `bin` to test the proxy on the 3 provided
files from A1. `./test04-08` are additional basic functionality tests carried over from A1 (see
their comments)

### Additional tests 

* Run `./test02` to check for 2 clients who interleave their requests, meaning client1 sends 10 bytes
of request, then client2 sends 10, etc. Used to test if `select` is properly swapping between
them and their corresponding servers.
* Run `./test03` to check for client that disconnects early (note that curl seems to do this when -x is specified)
* Run `./run_test` followed by a resource to test the proxy on hitting a particular resource. This
includes some timing (used to perform experiments).
* Run `./conn_test_01` to check that a CURL request through our proxy to a HTTPS site gets the correct content
* Run `./conn_test_02` to check that a CONNECT + GET request from a client through the proxy to a HTTP site gets the correct content
* Run `./startcheckedproxy` then `./check_proxy` or `./check_proxy2` (simpler) or `./check_proxy3`. Make sure proxy has been set to have debugging mode on. 
* Run `./misbehaving_client` to make sure that both GET and CONNECT of banned hosts are forbidden as well as a bad request
* Run `./get_stream_client` and `./rate_limit_client` to see clients with multiple requests / CONNECTs on same client as well as the rate limit being enforced
* Run `./get_slow_stream_client` to see a client sending a lot of requests to a rather slow server
* `./test09` is a better version of `./test08` and caught bug with priority queue backed by cache. `./test10` is for unused server sockets. `./test11` is a variant of `./test09`

## Progress Log

### Stage 0

- [x] Reorganize Chami's A1 to be more modular and easier to update. Check that it still works. 
More modular solution has separate modules for things like string operations, HTTP parsing,
TCP operations, etc.
- [x] Add HTTP format error handling.

### Stage 1

- [x] Test the individual modules from stage 0. Will allow Matt to become familiar with existing
code. 
- [x] Allow for the proxy to handle multiple clients with nonpersistent connections (i.e. connections with clients closed after servicing 1 GET request).  
- [x] Handle possibility of clients leaving early. 
- [x] Test with program that has clients interleave their GET requests.

### Stage 2

- [x] Implement CONNECT handling. 
- [x] Need to create server that will return files of varying sizes enough to fill cache.
- [x] Test overflowing of cache with local server. 
- [x] Test overflowing of cache with list of https in `proxy_testing/http_urls`.
- [x] Test other aspects of proxy functionality (age updates, things going stale)

### Baseline Demo 

* `./run_unit_test` - Unit testing of various modules such as invalid requests (inc. rate limiting start) 
* `./test01 <filename>` - Basic test for 1 of 3 A1 files
* `./test02` - Test interleaved (multiple) clients
* `./test03` - Test clients that leave early
* `./test04` - Check Age is included in proxy response
* `./test05` - Check that oldest item is evicted if cache overflows 
* `./test06` - Check stale item is evicted even if its not oldest if cache overflows
* `./test07` - Check that GET resets stale timer
* `./test08` - Check that different ports are considered different resources
* `./conn_test_01` - Test CURL->PROXY->HTTPS Site
* `./conn_test_02` - Test CONNECT+GET->PROXY->HTTP Site
* `./experiments` - Runs set of Experiments2 (timing, stress testing proxy)

## Baseline Experiments

See [results](https://docs.google.com/spreadsheets/d/1v_NDJWZCp_v7gJO0qMq5uH_vLfK0ituqHQ7WTzwDf5g/edit?usp=sharing) and [notes](https://docs.google.com/document/d/1k2QGEjFneP5fCvSpwILMvX-DcxVw6IreEd4Lujd8QB0/edit?usp=sharing).

### Stage 3

- [x] Have persistent connections (clients can make multiple GET requests over same connection).
- [x] Reuse server connections by keeping a finite number of unused server sockets associated with hosts
- [x] Convert cache from linked list to hash set
- [x] Implement simple filtering (hash table lookup based on host name)
- [x] Byte rate limiting (have implemented sliding window algorithm, waiting on persistent connections)
- [x] Highlighting links in HTML files to resources that are in cache
- [x] HTTPS

## CheckPoint2 Demo

### HTTP Component 

* `./startproxy` then `./get_slow_stream_client` then `./startslowserver`
* `./startproxy` then `./non_closing_server`
* `./startproxy` then `./closing_server` then `./startserver`
* `./startlimitedproxy` then `./rate_limit_client` 
* Open browser and go to `http://www.cs.tufts.edu/comp/112/index.html`. Add `www.cs.tufts.edu` to `banned_hosts.txt`, clear cache and do it again. 
* Open browser and go to `http://www.testingmcafeesites.com/index.html`. Then click on links to ac, al or a couple others. Then go back to index, may need to refresh.

### Checkpoint2 HTTP Experiments

See [here](https://docs.google.com/spreadsheets/d/1kehswulAW-EccWh7-nfP8N0ZZDJYGMoPaXgtPelmEDs/edit?usp=sharing) and `experiments/plan.txt`

### HTTPS Component

* Matt

### Stage 4

- [ ] Fix bugs with HTTPS and reusing server sockets
- [ ] Rate limiting for banned hosts
- [ ] Implement multilayer caching (n RAM cache blocks, 10n disk cache blocks).

## References

* [http URLs for testing](http://www.testingmcafeesites.com/)
    * http://www.testingmcafeesites.com/porn/index.html was removed
