
FROM cudasp5
LABEL maintainer "NVIDIA CORPORATION <cudatools@nvidia.com>"
# before running it, make sure cudasp5 image is built already by doing: sudo docker build -t cudasp5 .  under ../runtime/
# nano Dockerfile && sudo docker build -t cudasp5dev . && sudo docker run --runtime=nvidia -ti --rm cudasp5dev /bin/bash

#RUN apt-get update && apt-get install -y git && git clone https://github.com/welcheb/CUDA_examples.git && git clone https://github.com/zchee/cuda-sample.git
#RUN apt-get install nano


RUN apt-get update && apt-get install -y --no-install-recommends \
        cuda-core-$CUDA_PKG_VERSION \
        cuda-misc-headers-$CUDA_PKG_VERSION \
        cuda-command-line-tools-$CUDA_PKG_VERSION \
        cuda-license-$CUDA_PKG_VERSION \
        cuda-cublas-dev-$CUDA_PKG_VERSION \
        cuda-cufft-dev-$CUDA_PKG_VERSION \
        cuda-curand-dev-$CUDA_PKG_VERSION \
        cuda-cusparse-dev-$CUDA_PKG_VERSION \
        cuda-npp-dev-$CUDA_PKG_VERSION \
        cuda-cudart-dev-$CUDA_PKG_VERSION \
        cuda-driver-dev-$CUDA_PKG_VERSION && \
    cd /tmp && apt-get download gpu-deployment-kit && \
    rm -rf /var/lib/apt/lists/*
RUN apt update && apt install -y freeglut3-dev && apt -y install libxmu-dev libxi-dev && apt-get -y install python3-pip && pip3 install pycuda && pip3 install numba && apt -y install nano



RUN mkdir /tmp/gpu-deployment-kit && cd /tmp/gpu-deployment-kit && \
    dpkg -x /tmp/gpu-deployment-kit_*.deb . && \
    mv usr/include/nvidia/gdk/* /usr/local/cuda/include && \
    mv usr/src/gdk/nvml/lib/* /usr/local/cuda/lib64/stubs && \
    rm -rf /tmp/gpu-deployment-kit*
#COPY openclcuda /openclcuda
COPY cuda6p5_samples /cuda6p5_samples
ENV LIBRARY_PATH /usr/local/cuda/lib64/stubs:${LIBRARY_PATH}

CMD ["ls","usr/local/cuda"]
