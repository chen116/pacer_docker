# clinfo - show OpenCL platforms and devices

# sudo docker build -t clinfo .
# sudo docker run --runtime=nvidia -ti --rm clinfo



# nano Dockerfile && sudo docker build -t pyopcl . && sudo docker run --runtime=nvidia -e "VIC_SHM_ID=1024" -ti --rm pyopcl

# to have a bash in container, rmb to change CMD below
# nano Dockerfile && sudo docker build -t pyobjdet . && sudo docker run --runtime=nvidia -e "VIC_SHM_ID=1024" -ti --rm pyopcl /bin/bash


# To run standalone :



# now at vncing:
# at the vnc window of root@: run $ xhost + local:root
# sudo docker build  --no-cache -t pyobjdet .  
# sudo docker run --runtime=nvidia -v /tmp/.X11-unix/:/tmp/.X11-unix:ro -v /root/pacer_docker/ipc/soc/tcp:/foo -e DISPLAY=$DISPLAY --ipc="host" -e "VIC_SHM_ID=1024" -ti --rm pyobjdet /bin/bash
# https://groups.google.com/forum/#!topic/etetoolkit/4SrZk5fRtCM
# https://github.com/osrf/docker_images/issues/21


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



RUN apt -y install cmake-gui





RUN apt -y install nano

# RUN git clone https://github.com/smistad/OpenCL-Getting-Started.git && cd OpenCL-Getting-Started && make 


VOLUME /foo



# COPY shmlib.c shmlib.c
# COPY heartbeat.py heartbeat.py
# COPY heartbeat_python_lib.c heartbeat_python_lib.c
# COPY Makefile Makefile
# RUN make




# COPY test.py test.py



#https://github.com/hughperkins/tf-coriander/blob/master/docker/Dockerfile
RUN apt-get -y install python3-pip python3-dev

RUN apt-get update
RUN apt-get install -y cmake
# RUN apt-get install -y qt5-default libvtk6-dev
RUN apt-get install -y zlib1g-dev libjpeg-dev libwebp-dev libpng-dev libtiff5-dev libjasper-dev libopenexr-dev libgdal-dev
RUN apt-get install -y libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev yasm libopencore-amrnb-dev libopencore-amrwb-dev libv4l-dev libxine2-dev
RUN apt-get install -y libtbb-dev libeigen3-dev
RUN apt-get install -y python-dev python-tk python-numpy python3-dev python3-tk python3-numpy
RUN apt-get install -y ant default-jdk
RUN apt-get install -y doxygen

RUN apt-get install -y cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
RUN apt-get install -y python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev



RUN git clone https://github.com/opencv/opencv_contrib.git

RUN git clone https://github.com/opencv/opencv.git && mv opencv OpenCV && cd OpenCV && mkdir build && cd build && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules -DWITH_OPENCL=ON .. && \
	 	make -j7 && \
	 		make install && \
	 			ldconfig	
	 			
RUN apt-get install -y xorg-dev libglu1-mesa-dev
# ARG OPENCV_VERSION=3.4.1
# RUN apt-get install -y unzip wget && wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip && unzip $OPENCV_VERSION.zip && rm $OPENCV_VERSION.zip && mv opencv-$OPENCV_VERSION OpenCV



# RUN cd OpenCV && mkdir build && cd build && \
# 		cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules -DWITH_OPENCL=ON -DWITH_QT=ON -DWITH_OPENGL=ON -DFORCE_VTK=ON -DWITH_TBB=ON -DWITH_GDAL=ON -DWITH_XINE=ON -DBUILD_EXAMPLES=ON -DENABLE_PRECOMPILED_HEADERS=OFF .. && \
# 		 	make -j4 && \
# 		 		make install && \
# 		 			ldconfig

#cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules -DWITH_OPENCL=ON -DWITH_QT=ON -DWITH_OPENGL=ON -DFORCE_VTK=ON -DWITH_TBB=ON -DWITH_GDAL=ON -DWITH_XINE=ON -DBUILD_EXAMPLES=ON ..


# install openGL
RUN apt -y install freeglut3-dev


RUN git clone https://github.com/chen116/pacer_docker.git
# RUN apt -y install python3-pip && pip3 install --upgrade pip && pip install numpy && pip install opencv-python && apt -y install libsm6 libxrender1 libfontconfig1 && apt -y install python3-tk && apt -y install x11-xkb-utils && pip install imutils
RUN pip3 install --upgrade pip  && pip install imutils 
#RUN cd /pacer_docker/app/objdet/ && sh install-opencv.sh

# makes heartbeat python module
RUN cd /pacer_docker/opencl && make && cp *.so /pacer_docker/app/objdet/


#OPENCL-INTERPCEPT
RUN ls /dev/shm
RUN git clone https://github.com/chen116/opencl-intercept-layer.git && cd opencl-intercept-layer && mkdir build && cd build && cmake .. && make 
RUN cp /opencl-intercept-layer/vic/clintercept.conf ~/
RUN mv /usr/lib/x86_64-linux-gnu/libOpenCL.so.1.0.0 /usr/lib/x86_64-linux-gnu/real_libOpenCL.so.1.0.0 && ln -sf /opencl-intercept-layer/build/libOpenCL.so.1 /usr/lib/x86_64-linux-gnu/libOpenCL.so.1 && ln -sf /opencl-intercept-layer/build/libOpenCL.so /usr/lib/x86_64-linux-gnu/libOpenCL.so


# HEARTBEATS
RUN git clone https://github.com/chen116/heartbeats.git
#COPY heartbeats /heartbeats
RUN cd heartbeats && mkdir hbed
ENV HEARTBEAT_ENABLED_DIR /heartbeats/hbed
RUN cd heartbeats && make clean && make uninstall && make && make install && ldconfig 

# boost
RUN apt install -y libboost-all-dev

WORKDIR /

# CMD ["python3","test.py"]
