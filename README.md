#parse mast boot record, which is an old partition format.

## Install
1. git clone https://github.com/firebroo/parseMbr.git
2. cd parseMbr
3. make && make install

## Use
/dev/sdx is your bootLoader'device

1. parseMbr -f /dev/sdx


