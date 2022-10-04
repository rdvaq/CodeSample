#!/bin/bash

curl -x comp112-01:$1 $2 > res
curl $2 > ref
diff res ref

# curl -x comp112-01:$1 http://www.cs.cmu.edu/~prs/bio.html > res
# curl http://www.cs.cmu.edu/~prs/bio.html > ref
# diff res ref

# curl -x comp112-01:$1 http://www.cs.cmu.edu/~dga/dga-headshot.jpg > res
# curl http://www.cs.cmu.edu/~dga/dga-headshot.jpg > ref
# diff res ref