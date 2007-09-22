INCLUDES = $(all_includes)
METASOURCES = AUTO
SUBDIRS = common libcli xml_to_data devices piklab-hex coff progs piklab-prog tools data libgui piklab

messages:
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	LIST=`find . -name \*.h -o -name \*.cpp -o -name \*.c`; \
	if test -n "$$LIST"; then \
	  $(XGETTEXT) $$LIST --from-code=UTF-8 -o $(podir)/piklab.pot; \
	fi
