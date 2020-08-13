CC=arm-linux-gnueabihf-gcc
SRC=*.c

all:
	$(CC) -I./ $(SRC)  -march=armv7-a -mfpu=neon -mfloat-abi=hard -o webspi

clean:
	rm webspi
