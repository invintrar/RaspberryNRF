#This target will compile all the file
main:
	gcc -o output main.c nrf24l01.c -lwiringPi
	./output
	rm output

nrf:
	gcc -o output main1.c nrf24l01.c -lwiringPi
	./output
	rm output
send:
	gcc -o output send.c nrf24l01.c -lwiringPi
	./output
	rm output
uart:
	gcc -o output uart.c -lwiringPi
	./output
	rm output

maingps:
	gcc -o output maingps.c gps.c -lwiringPi -lm
	./output
	rm output
