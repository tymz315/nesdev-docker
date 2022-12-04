FROM ubuntu:20.04

ARG PREFIX=/opt

COPY files/ /

RUN apt update \
	&& apt install -y make gcc \
	&& cd /usr/local/src/asm6-1.6 \
	&& make \
	&& make install \
	&& make clean \
	&& cd /usr/local/src/asm6f-1.6_freem02 \
	&& make \
	&& make install \
	&& make clean \
	&& cd /usr/local/src/cc65-2.19 \
	&& make \
	&& make install PREFIX=/usr/local \
	&& make clean \
	&& cd /usr/local/src/magickit-2.51 \
	&& make \
	&& make install \
	&& make clean \
	&& apt purge -y --auto-remove gcc \
	&& apt clean \
	&& rm -fr /var/lib/apt/lists/*
