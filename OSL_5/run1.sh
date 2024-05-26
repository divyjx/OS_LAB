cp -f part1/schedule.c /usr/src/minix/servers/sched/schedule.c
cp -f part1/forkexit.c /usr/src/minix/servers/pm/forkexit.c
cp -f part1/system.c /usr/src/minix/kernel/system.c
cd /usr/src && make build MKUPDATE=yes