# Install Piklab XML files

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(datadir)/usbpicprog
	for xmlfile in *.xml; do \
		if test -f $$xmlfile; then \
			$(INSTALL_DATA) $$xmlfile $(DESTDIR)$(datadir)/usbpicprog; \
		fi \
	done
