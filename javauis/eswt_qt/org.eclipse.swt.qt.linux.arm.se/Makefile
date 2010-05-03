# Scratchbox paths. Not the same as Linux host paths!
LIB_PATH:=/home/pmotresc/org.eclipse.swt/Eclipse_SWT_PI/qt/library
LIB_PATH_GFX:=/home/pmotresc/org.eclipse.swt/Eclipse_SWT_PI/qt/graphics_library

debug:
	cd $(LIB_PATH) && qmake -spec linux-g++ -unix CONFIG+=debug DESTDIR=debug OBJECTS_DIR=debug/linux-g++/build -o Makefile.maemo.debug swt.pro
	cd $(LIB_PATH) && make -f Makefile.maemo.debug
	cd $(LIB_PATH_GFX) && qmake -spec linux-g++ -unix CONFIG+=debug DESTDIR=debug OBJECTS_DIR=debug/linux-g++/build -o Makefile.maemo.debug graphics.pro
	cd $(LIB_PATH_GFX) && make -f Makefile.maemo.debug
	echo Done building debug

release:
	cd $(LIB_PATH) && qmake -spec linux-g++ -unix CONFIG+=release DESTDIR=release OBJECTS_DIR=release/linux-g++/build -o Makefile.maemo.release swt.pro
	cd $(LIB_PATH) && make -f Makefile.maemo.release
	cd $(LIB_PATH_GFX) && qmake -spec linux-g++ -unix CONFIG+=release DESTDIR=release OBJECTS_DIR=release/linux-g++/build -o Makefile.maemo.release graphics.pro
	cd $(LIB_PATH_GFX) && make -f Makefile.maemo.release
	echo Done building release

clean:
	-cd $(LIB_PATH)
	-make -f Makefile.maemo.debug clean
	-rm -f $(LIB_PATH)/Makefile.maemo.debug
	-rm -f -r $(LIB_PATH)/debug
	-cd $(LIB_PATH_GFX)
	-make -f Makefile.maemo.debug clean
	-rm -f $(LIB_PATH_GFX)/Makefile.maemo.debug
	-rm -f -r $(LIB_PATH_GFX)/debug
	-@echo Done cleaning debug
	-cd $(LIB_PATH)
	-make -f Makefile.maemo.release clean
	-rm -f $(LIB_PATH)/Makefile.maemo.release
	-rm -f -r $(LIB_PATH)/release
	-cd $(LIB_PATH_GFX)
	-make -f Makefile.maemo.release clean
	-rm -f $(LIB_PATH_GFX)/Makefile.maemo.release
	-rm -f -r $(LIB_PATH_GFX)/release
	-@echo Done cleaning release

all: debug, release
