cp -f mfs/read.c /usr/src/minix/fs/mfs/read.c
cp -f link.c /usr/src/minix/servers/vfs/link.c
cp -f open.c /usr/src/minix/servers/vfs/open.c
cp -f read.c /usr/src/minix/servers/vfs/read.c
cp -f mfs/write.c /usr/src/minix/fs/mfs/write.c
cp -f const.h /usr/src/minix/include/minix/const.h
cp -f fslib.c /usr/src/minix/lib/libc/gen/fslib.c
cd /usr/src && make build MKUPDATE=yes