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
	gcc -o output uartP.c -lwiringPi
	./output
	rm output
