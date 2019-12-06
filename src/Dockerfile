FROM ubuntu:19.04

RUN apt update && apt install -y qemu
RUN apt install -y qemu-system-x86
RUN apt install -y apt-utils xinetd

RUN useradd -m ctf
WORKDIR /home/ctf

COPY ./ctf.xinetd /etc/xinetd.d/ctf
COPY ./start.sh /start.sh
RUN echo "Blocked by ctf_xinetd" > /etc/banner_fail

COPY photon.iso /home/ctf/photon.iso

RUN chmod +x /start.sh
RUN chown -R root:ctf /home/ctf && \
    chmod -R 750 /home/ctf
    
CMD ["/start.sh"]
EXPOSE 2000
