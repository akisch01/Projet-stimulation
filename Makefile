all:
	$(MAKE) -C chauffage
	$(MAKE) -C thermometre
	$(MAKE) -C systeme_central
	$(MAKE) -C console

clean:
	$(MAKE) -C chauffage clean
	$(MAKE) -C thermometre clean
	$(MAKE) -C systeme_central clean
	$(MAKE) -C console clean