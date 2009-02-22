SUBDIRS=system built-in programs plug-in remote other

.PHONY: all execute clean


all:
	for subdir in $(SUBDIRS); do \
  make -C $$subdir; \
done


execute:
	for subdir in $(SUBDIRS); do \
  make -C $$subdir execute; \
done


clean:
	for subdir in $(SUBDIRS); do \
  make -C $$subdir clean; \
done
