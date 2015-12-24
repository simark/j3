.PHONY: all

all:
	make -C master/fw
	make -C slave/fw

clean:
	make -C master/fw clean
	make -C slave/fw clean