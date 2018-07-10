export CPLUS_INCLUDE_PATH= /usr/lib/x86_64-linux-gnu/
export LIBRARY_PATH= /usr/lib/x86_64-linux-gnu/

HEADERS=../common/basic.hpp ../common/cmdparser.hpp ../common/oclobject.hpp ../common/yuv_utils.h ../common/utils.h
SOURCES=main.cpp ../common/basic.cpp ../common/cmdparser.cpp ../common/oclobject.cpp ../common/yuv_utils.cpp ../common/utils.cpp

ifeq ($(CONFIG),debug)
	OPT =-O0 -g
else
	OPT =
endif

all: MotionEstimationAdvanced

MotionEstimationAdvanced: $(HEADERS) $(SOURCES) Makefile
	g++ $(SOURCES) -I../common -lOpenCL -oMotionEstimationAdvanced -std=gnu++0x $(OPT) -lrt -lhb-shared -lhrm-shared

clean:
	rm -f MotionEstimationAdvanced

