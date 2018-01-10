CONVERT
====

* shell instruction

```$xslt
./convert.bin ${origin_file} ${new_base_file} ${origin_query} ${new_query_file} ${operation}
./convert.bin \
data/audio/audio_base.fvecs \
data/e2a/e2a_base.fves \
data/audio/audio_query.fvecs \
data/e2a/e2a_query.fves \
e2m \
m2a \

```  

* parameter 
    * ${origin_file}: the data you want to transform
    * ${new_base_file}:   where you wanna save transformed data
    * ${origin_query}: the query data you want to transform
    * ${new_query_file}: where you wanna save transformed query data
    * ${operation}: 
        * **e2m** euclidToMIP
        * **m2a** mipToAngular
        * **e2m m2a** euclidToMIP, followed with mipToAngular