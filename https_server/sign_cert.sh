#! /bin/bash

# purpose: local CA signs a dummy csr with dynamically generated config to 
# generate a new certificate 

csr=./proxy_key_cert/fd.csr
rootcert=./root_key_cert/myCA.pem
rootkey=./root_key_cert/myCA.key
certconfig=./proxy_key_cert/cert_config.ext
cert=./proxy_key_cert/cert.pem


openssl x509 -req -in $csr -CA $rootcert -CAkey $rootkey -CAcreateserial -out $cert -days 825 -sha256 -extfile $certconfig