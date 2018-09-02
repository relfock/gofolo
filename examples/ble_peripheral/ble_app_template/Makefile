
all: 
	make -C pca10040/s132/armgcc/
	../../../nRF5x-Command-Line-Tools/mergehex/mergehex -m ../../../components/softdevice/s132/hex/s132_nrf52_3.0.0_softdevice.hex pca10040/s132/armgcc/_build/nrf52832_xxaa.hex -o gofolo.hex

flash: 
	make -C pca10040/s132/armgcc/ $@

flash_softdevice: 
	make -C pca10040/s132/armgcc/ $@

erase: 
	make -C pca10040/s132/armgcc/ $@

clean: 
	make -C pca10040/s132/armgcc/ $@
