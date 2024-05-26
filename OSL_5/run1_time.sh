cp -f old_code/schedule.c /usr/src/minix/servers/sched/schedule.c
cp -f old_code/forkexit.c /usr/src/minix/servers/pm/forkexit.c
cp -f old_code/system.c /usr/src/minix/kernel/system.c
cd /usr/src && make build MKUPDATE=yes 