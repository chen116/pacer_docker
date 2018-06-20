import os
from ctypes import cdll
import ctypes
import sysv_ipc
import sys
import docker
import time
import _thread
# import heartbeat
# shm_id = int(os.environ['VIC_SHM_ID'])
# window_size_hr=5
# hb = heartbeat.Heartbeat(shm_id,window_size_hr,100,"vic.log",10,100)
# hb.heartbeat_beat()
# pinrt(hb.get_instant_heartrate())
# hb.heartbeat_finish()
# bodytrack /benches/parsec-vic/sequenceB_261/ 4 261 3000 8 2 4 1



client = docker.from_env()
cs=[]
# remove container with same name
for x in (client.containers.list(all=True)):
	print("rm:",x.name)
	x.remove(force=True)
vicid={"VIC_SHM_ID":str(1024)}

mnt = docker.types.Mount('/foo', '/home/eeb205/pacer_docker/ipc/',type='bind')
# https://docker-py.readthedocs.io/en/stable/api.html#docker.types.Mount




# container = client.containers.run('pyopcl',runtime='nvidia',environment=vicid,ipc_mode='host',cpu_period=int(1e5),cpu_quota=int(1e5),name="v1",detach=False)#,tty=True)
container = client.containers.run('pyopcl',mounts=[mnt],runtime='nvidia',environment=vicid,ipc_mode='host',cpu_period=int(1e5),cpu_quota=int(1e5),name="v1",detach=True)#,tty=True)
# https://docker-py.readthedocs.io/en/stable/containers.html

#container.logs()
#,detach=True)

#docker run --runtime=nvidia -v /home/eeb205/pacer_docker/ipc/:/foo -e "VIC_SHM_ID=1024" -ti --rm pyopcl /bin/bash



# to see stdout... $ docker logs v1(or whatever the name is)




shmlib = cdll.LoadLibrary('./shmlib.so')
ids = [1024]#, 4000, 5000]
lenn = len(ids)

logids=[]
gloids=[]
for x in ids:
	logids.append(x*2)
	gloids.append(x*2+1)

logmem = []
glomem = []
result = None
while result is None:
	try:
		for i in range(lenn):
			logmem.append(sysv_ipc.SharedMemory(logids[i]))
			glomem.append(sysv_ipc.SharedMemory(gloids[i]))
			result=1
	except:
		pass

indexs = [0 for x in range(lenn)]
for i in range(lenn):
	g=glomem[i].read()
	tmp_index = shmlib.get_index(g)
	if tmp_index!=indexs[i]:
		indexs[i]=tmp_index
		# print("v",i,indexs[i])
		if indexs[i]>=30:
			keep_going[i]=0
		if i==1 and indexs[i]>=30:
			keep_going[i]=0
			for con in client.containers.list():
				if con.name=='v'+str(i):
					con.remove(force=True)
		p=logmem[i].read()
		hr = shmlib.get_hr(p,tmp_index)/1e6
		shmlib.get_ts.restype = ctypes.c_int64
		ts = shmlib.get_ts(p,tmp_index)
		print("meow",ts,1/hr,tmp_index)


