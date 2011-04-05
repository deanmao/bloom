makelibs: bloom clean

bloom:
	$(MAKE) -C ./src
	cp ./src/build/default/bloomfilter.node .
	
clean:
	rm -rf ./src/build ./src/.lock-wscript
	
