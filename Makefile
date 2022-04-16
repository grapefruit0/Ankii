VPATH = ./src

bin_name = ankii

objects = ${bin_name}.o extract_content.o shell.o func_wrapper.o error.o

${bin_name} : ${objects}
	gcc -g -O0 -o ${bin_name} ${objects}
	rm ${objects}

${bin_name}.o : ankii.h
extract_content.o : ankii.h
shell.o : ankii.h
func_wrapper.o : ankii.h
error.o : ankii.h

.PHONY : clean
clean : 
	rm ${bin_name}
