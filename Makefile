# Topmost todo

SUBDIRS = src
SUBDIRS_ALL = ${SUBDIRS} test

.PHONY: src test clean run-test help

all: ${SUBDIRS_ALL}

help:
	@echo "make run-test to run tests"

src:
	@echo "building src"; cd src && $(MAKE) && cd ..

test: src
	@echo "building test"; cd test && $(MAKE) && cd ..
	
run-test: test
	@echo "running test"; cd test && $(MAKE) run-test && cd ..
	
clean:
	@for dir in ${SUBDIRS_ALL}; do echo "cleaning $$dir"; cd $$dir && $(MAKE) clean && cd ..; done
