MAKE := make
all:
	$(MAKE) -C ./lib
	$(MAKE) -C ./1-helloworld

clean:
	$(MAKE) clean -C ./lib
	$(MAKE) clean -C ./1-helloworld
