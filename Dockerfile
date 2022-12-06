FROM ubuntu:20.04

ARG PREFIX=/opt

COPY files/ /

RUN apt update \
	&& apt install -y make gcc \
	&& cd /usr/local/src/asm6-1.6 \
	&& make \
	&& make install \
	&& rm -fr /usr/local/src/asm6-1.6 \
	&& cd /usr/local/src/asm6f-1.6_freem02 \
	&& make \
	&& make install \
	&& rm -fr /usr/local/src/asm6f-1.6_freem02 \
	&& cd /usr/local/src/cc65-2.19 \
	&& make \
	&& make install PREFIX=/usr/local \
	&& rm -fr /usr/local/src/cc65-2.19 \
#	&& cd /usr/local/src/magickit-2.51 \
#	&& make \
#	&& make install \
#	&& make clean \
	&& rm -fr /usr/local/src/magickit-2.51 \
	&& cd /usr/local/src/ppmck/src/nesasm \
	&& make -f Makefile.unx \
	&& install -s /usr/local/src/ppmck/bin/nesasm /usr/local/bin \
	&& rm -fr /usr/local/src/ppmck \
	&& apt purge -y --auto-remove gcc \
	&& apt clean \
	&& rm -fr /var/lib/apt/lists/*
