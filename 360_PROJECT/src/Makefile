KVERS = ${shell uname -r}

obj-m += do_execve.o

build : kernel_modules
kernel_modules : 
	make -C /lib/modules/${KVERS}/build  M=${CURDIR} modules

clean : 
	rm -rf *.o *.ko
	rm -rf modules.order 
	rm -rf Module.symvers
	rm -rf do_execve.mod*
	rm -rf .do_execve*
	rm -rf .tmp_versions
