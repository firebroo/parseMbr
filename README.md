#parse mast boot record, which is an old partition format.

## Install
1. git clone https://github.com/firebroo/parseMbr.git
2. cd parseMbr
3. make && make install

## Use
1. dd if=/dev/sdx of=mbr.bin bs=512 count=1
2. parseMbr -f mbr.bin 

/dev/sdx is your bootLoader'device

