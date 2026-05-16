include ../Makefile.inc

release : utfencoder

utfencoder : utf_tool.r.o utf_codec.r.o
	$(LD) -o $@ $^

debug : utf_demo.d.o utf_codec.d.o
	$(LD) -o $@ $^

utest : utf_codec_test.d.o utf_codec.d.o
	$(LD) -o $@ $^ $(GTEST_LIBS)

run_utest : utest
	./$^

run_itest : release
	echo "Integration tests..."
	bash ./integration_test.sh

all: depends run_utest run_itest

clean :
	-rm -f *.o *.hdrdep.bak
	-rm -f utfencoder utest debug
	-rm -rf .itest
	-rm -f test.utf8 test.utf8.out test.utf16 test.utf16.py

cleanall : clean
	@echo > Makefile.hdrdep

-include Makefile.hdrdep
