cp -f old_code/forkexit.c /usr/src/minix/servers/pm/forkexit.c
cp -f old_code/system.c /usr/src/minix/kernel/system.c
cp -f part2/sched_time/schedule.c /usr/src/minix/servers/sched/schedule.c
cd /usr/src && make build MKUPDATE=yes