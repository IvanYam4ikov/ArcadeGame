.PHONY: all run clean

all:
	$(MAKE) -C GUI_files/cpp_files
	$(MAKE) -C source

run: all
	cd source && ./arcadeSystem

clean:
	$(MAKE) -C GUI_files/cpp_files clean
	$(MAKE) -C source clean
