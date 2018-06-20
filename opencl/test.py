# import the required modules
import pyopencl as cl
import numpy as np
import time
import os


import heartbeat
shm_id = int(os.environ['VIC_SHM_ID'])
window_size_hr=5
print('		creating hb')
ts=time.time()
hb = heartbeat.Heartbeat(shm_id,window_size_hr,100,"vic.log",10,100)
print('		time took to create hb',time.time()-ts)
#hb.heartbeat_beat()
#print(hb.get_instant_heartrate())


#this line would create a context
cntxt = cl.create_some_context(answers=['0'],interactive=False)
#now create a command queue in the context
queue = cl.CommandQueue(cntxt)
num_ele=1001
# create some data array to give as input to Kernel and get output
num1 = np.array(range(num_ele), dtype=np.int32)
num2 = np.array(range(num_ele), dtype=np.int32)
out = np.empty(num1.shape, dtype=np.int32)
print("		correct answer",num1[-1]**3+num2[-1]**3)
ts=time.time()
ans=[]
hb.heartbeat_beat()
print('		hb init hr:',hb.get_instant_heartrate())
for i in range(len(num1)):
	ans.append(num1[i]**3+num2[i]**3)
print('		cpu exec time:',time.time()-ts)
hb.heartbeat_beat()
print('		hb: cpu exec time:',1/hb.get_instant_heartrate())
print('		start opencl:')
# create the buffers to hold the values of the input
num1_buf = cl.Buffer(cntxt, cl.mem_flags.READ_ONLY | cl.mem_flags.COPY_HOST_PTR,hostbuf=num1)
num2_buf = cl.Buffer(cntxt, cl.mem_flags.READ_ONLY | cl.mem_flags.COPY_HOST_PTR,hostbuf=num2)
# create output buffer
out_buf = cl.Buffer(cntxt, cl.mem_flags.WRITE_ONLY, out.nbytes)

# Kernel Program
code = """
__kernel void frst_prog(__global int* num1, __global int* num2,__global int* out) 
{
    int i = get_global_id(0);
    out[i] = num1[i]*num1[i]*num1[i]+num2[i]* num2[i]*num2[i];
}
"""
# build the Kernel
bld = cl.Program(cntxt, code).build()
ts=time.time()
hb.heartbeat_beat()
# Kernel is now launched
launch = bld.frst_prog(queue, num1.shape, None, num1_buf,num2_buf,out_buf)
# wait till the process completes
launch.wait()
cl.enqueue_read_buffer(queue, out_buf, out).wait()
hb.heartbeat_beat()
tts=time.time()
print('		hb: gpu exec time:',1/hb.get_instant_heartrate())
print('		gpu exec time',tts-ts)
# print the output
print("		Number1:", num1[-1])
print("		Number2:", num2[-1])
print("		Output :", out[-1])

hb.heartbeat_finish()
