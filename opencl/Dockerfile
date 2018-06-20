# clinfo - show OpenCL platforms and devices

# sudo docker build -t clinfo .
# sudo docker run --runtime=nvidia -ti --rm clinfo



# nano Dockerfile && sudo docker build -t pyopcl . && sudo docker run --runtime=nvidia -e "VIC_SHM_ID=1024" -ti --rm pyopcl

# to have a bash in container, rmb to change CMD below
# nano Dockerfile && sudo docker build -t pyopcl . && sudo docker run --runtime=nvidia -e "VIC_SHM_ID=1024" -ti --rm pyopcl /bin/bash



# sudo docker build -t pyopcl .  && sudo docker run --runtime=nvidia -v /home/eeb205/pacer_docker/ipc/:/foo -e "VIC_SHM_ID=1024" -ti --rm pyopcl /bin/bash

FROM nvidia/opencl:runtime-ubuntu16.04
RUN apt-get update
RUN apt -y install apt-utils
RUN apt-get -y install build-essential libnuma-dev
RUN apt-get -y install opencl-headers
RUN apt -y install ocl-icd-opencl-dev
RUN apt-get -y install python3-pyopencl
RUN apt -y install wget
RUN apt -y install python3-dev && wget http://semanchuk.com/philip/sysv_ipc/sysv_ipc-1.0.0.tar.gz && tar -xzf sysv_ipc-1.0.0.tar.gz && cd sysv_ipc-1.0.0 && python3 setup.py install
RUN apt-get update
RUN apt -y install git


# HEARTBEATS
RUN git clone https://github.com/libheartbeats/heartbeats.git
#COPY heartbeats /heartbeats
RUN cd heartbeats && mkdir hbed
ENV HEARTBEAT_ENABLED_DIR /heartbeats/hbed
RUN cd heartbeats && make clean && make uninstall && make && make install && ldconfig 

RUN apt -y install cmake-gui





RUN apt -y install nano

RUN git clone https://github.com/smistad/OpenCL-Getting-Started.git && cd OpenCL-Getting-Started && make 


VOLUME /foo



COPY shmlib.c shmlib.c
COPY heartbeat.py heartbeat.py
COPY heartbeat_python_lib.c heartbeat_python_lib.c
COPY Makefile Makefile
RUN make

COPY test.py test.py
#COPY openclcuda /openclcuda


#OPENCL-INTERPCEPT

RUN ls
RUN git clone https://github.com/chen116/opencl-intercept-layer.git && cd opencl-intercept-layer && mkdir build && cd build && cmake .. && make 

RUN cp /opencl-intercept-layer/vic/clintercept.conf ~/
RUN mv /usr/lib/x86_64-linux-gnu/libOpenCL.so.1.0.0 /usr/lib/x86_64-linux-gnu/real_libOpenCL.so.1.0.0 && ln -sf /opencl-intercept-layer/build/libOpenCL.so.1 /usr/lib/x86_64-linux-gnu/libOpenCL.so.1 && ln -sf /opencl-intercept-layer/build/libOpenCL.so /usr/lib/x86_64-linux-gnu/libOpenCL.so


WORKDIR /

CMD ["python3","test.py"]
