C=$(CROSS_COMPILE)gcc -Wall -g -lm
GFLAGS=`pkg-config --libs --cflags gtk+-2.0 gmodule-2.0`
OFLAGS=`pkg-config --libs --cflags opencv`

TARGET=ViSyLite

ViSyGTK: ViSyGTK.c
	$(C) $(GFLAGS) $(OFLAGS) -o ViSyGTK ViSyGTK.c

ViSyLite: ViSyLite.c
	$(C) ViSyLite.c -I../ -lvisy -L../visy/ -o ViSyLite $(OFLAGS)  

ViSyTest: ViSyTest.c
	$(C) ViSyTest.c -I../ -lvisy -L../visy/ -o ViSyTest $(OFLAGS)  

ViSyAnalyze: ViSyAnalyze.c
	$(C) ViSyAnalyze.c -I../ -lvisy -L../visy/ -o ViSyAnalyze $(OFLAGS)  


all: $(TARGET)
	@echo "All done"



