#! /bin/bash

# script to generate a new csr from a config file
openssl req -new -config fd.cnf -key key.pem -out fd.csr