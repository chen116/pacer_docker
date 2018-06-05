import os
from ctypes import cdll
# import ctypes
#import sysv_ipc
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


# create shm id key

client = docker.from_env()
cs=[]
# remove container with same name
for x in (client.containers.list(all=True)):
	print("rm:",x.name)
	x.remove(force=True)
vicid={"VIC_SHM_ID":str(1024)}
container = client.containers.run('pyopcl',runtime='nvidia',environment=vicid,ipc_mode='host',cpu_period=int(1e5),cpu_quota=int(1e5),name="v1",detach=False)#,tty=True)
#container.logs()
#,detach=True)


# to see stdout... $ docker logs v1(or whatever the name is)

