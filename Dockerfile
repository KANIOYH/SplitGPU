FROM nvidia/cuda:12.2.2-base-ubuntu20.04
FROM python:3.9
#--ipc=host

WORKDIR /app 

ADD ./app/sp_watchdog.py /usr/bin

RUN pip install ./requirements.txt -i http://mirrors.aliyun.com/pypi/simple/ \
 && mkdir /hlib \
 && /usr/bin/sp_watchdog.py 

ADD ./build/libcudart.so /usr/hlib

ENV LD_PRELOAD=/usr/hlib/libcudart.so