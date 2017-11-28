OBJS=main.o key.o vk.o win.o vect.o enemy.o
SHDR=\
	s-const-tex-frag.spv s-const-tex-vert.spv \
	s-phong-frag.spv s-phong-vert.spv

LIBS= -LC:/VulkanSDK/1.0.54.0/Lib -lvulkan-1
FLAGS=-g -m64 -c -g -IC:/VulkanSDK/1.0.54.0/Include -Wno-narrowing -fpermissive -Werror


main.exe	:	obj $(OBJS:%.o=obj/%.o) $(SHDR)
	g++ -m64 -o main.exe $(OBJS:%.o=obj/%.o) $(LIBS)

obj/%.o:%.cpp
	g++  $(FLAGS) $< -o $@

%.spv:%.frag
	glslangValidator.exe $< -V -o $@

%.spv:%.vert
	glslangValidator.exe $< -V -o $@

obj:
	mkdir obj

clean:
	rm -rf obj
	rm *.o *.spv main.exe -f
