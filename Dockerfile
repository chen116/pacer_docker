# clinfo - show OpenCL platforms and devices

# sudo docker build -t clinfo .
# sudo docker run --runtime=nvidia -ti --rm clinfo



# nano Dockerfile && sudo docker build -t pyopcl . && sudo docker run --runtime=nvidia -e "VIC_SHM_ID=1024" -ti --rm pyopcl

FROM nvidia/opencl:runtime-ubuntu16.04
RUN apt-get update
RUN apt -y install apt-utils
RUN apt-get -y install build-essential libnuma-dev
RUN apt-get -y install opencl-headers
RUN apt -y install ocl-icd-opencl-dev
RUN apt-get -y install python3-pyopencl
RUN apt -y install wget
RUN apt -y install python3-dev && wget http://semanchuk.com/philip/sysv_ipc/sysv_ipc-1.0.0.tar.gz && tar -xzf sysv_ipc-1.0.0.tar.gz && cd sysv_ipc-1.0.0 && python3 setup.py install

RUN apt -y install git
RUN git clone https://github.com/libheartbeats/heartbeats.git
#COPY heartbeats /heartbeats
RUN cd heartbeats && mkdir hbed
ENV HEARTBEAT_ENABLED_DIR /heartbeats/hbed
RUN cd heartbeats && make clean && make uninstall && make && make install && ldconfig 



COPY heartbeat.py heartbeat.py
COPY heartbeat_python_lib.c heartbeat_python_lib.c
COPY Makefile Makefile
RUN make

COPY test.py test.py

WORKDIR /

CMD ["python3","test.py"]
