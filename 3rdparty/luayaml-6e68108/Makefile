VERSION = 0.3.1
PREFIX  = /usr/local
LIBDIR  = $(PREFIX)/lib/lua/5.1
CC      = gcc
CFLAGS  = -O2 -Wall -fPIC $(XCFLAGS)
LDFLAGS = -shared
LDLIBS  = -lyaml

yaml.so: lyaml.o b64.o
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

install: yaml.so
	install -Dpm0755 $< $(DESTDIR)$(LIBDIR)/$<

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/yaml.so

dist: lua-yaml-$(VERSION).tar.gz
rock: yaml-$(VERSION)-1.src.rock
rockspec: yaml-$(VERSION)-1.rockspec

%.tar.gz: lyaml.c b64.c b64.h Makefile README.md test.lua rockspec.in | clean
	mkdir $*
	cp -r $^ $*
	tar -czf $@ $*
	rm -rf $*

%.src.rock: %.rockspec
	luarocks pack $<

yaml-%-1.rockspec: rockspec.in
	sed 's/%VERSION%/$*/g' $< > $@

test: yaml.so test.lua
	@lua test.lua

clean:
	rm -f yaml.so *.o *.tar.gz *.rock *.rockspec


.PHONY: install uninstall dist rock rockspec test clean
