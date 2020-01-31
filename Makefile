#This target will compile all the file
nrf:
	gcc -o output main.c nrf24l01.c -lwiringPi
	./output
	rm output
uart:
	gcc -o output uart.c -lwiringPi
	./output
	rm output
