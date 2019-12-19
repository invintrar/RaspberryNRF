#This target will compile all the file
main:
	gcc -o output main.c nrf24l01.c -lwiringPi
	./output
	rm output

main1:
	gcc -o output main1.c nrf24l01.c -lwiringPi
	./output
	rm output
