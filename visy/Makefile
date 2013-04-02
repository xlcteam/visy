C=$(CROSS_COMPILE)gcc -Wall -g
GFLAGS=`pkg-config --libs --cflags gtk+-2.0 gmodule-2.0`
OFLAGS=`pkg-config --libs --cflags opencv`

DEST=/usr/lib
INCDEST=/usr/include

OBJ= img.o visy.o graph.o

TARGET=visy

%.o: %.c
	$(C) -c -o $@ $< $(OFLAGS)

$(TARGET): $(OBJ)
	$(CROSS_COMPILE)ar rcs lib$@.a $(OBJ)

all: $(TARGET)
	@echo "All done"

install: $(TARGET)
	install lib$(TARGET).a $(DEST)/lib$(TARGET).a
	mkdir $(INCDEST)/visy/
	install *.h $(INCDEST)/visy/

uninstall: 
	rm -rf $(DEST)/lib$(TARGET).a

.PHONY: clean

clean:
	rm -f *.o *.a

