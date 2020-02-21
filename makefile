VK=C:/VulkanSDK/1.2.131.2

INCS= -I$(VK)/Include
LIBS= -L$(VK)/Lib -lvulkan-1

OBJS=main.o key.o vk.o win.o vect.o enemy.o vector.o matrix.o core.o unit12.o mouse.o fighter.o camera.o
SHDR=\
	s-const-tex-frag.spv s-const-tex-vert.spv \
	s-phong-frag.spv s-phong-vert.spv

#LIBS= -LC:/VulkanSDK/1.0.54.0/Lib -lvulkan-1

FLAGS=-g -m64 -c -g $(INCS) -Wno-narrowing -fpermissive -Werror

main.exe	:	obj $(OBJS:%.o=obj/%.o) $(SHDR)
	clang++ -m64 -o main.exe $(OBJS:%.o=obj/%.o) $(LIBS)

obj/%.o:%.cpp
	clang++  $(FLAGS)  $< -o $@

%.spv:%.frag
	$(VK)/bin/glslangValidator.exe $< -V -o $@

%.spv:%.vert
	$(VK)/bin/glslangValidator.exe $< -V -o $@

obj:
	mkdir obj

clean:
	rm -rf obj
	rm *.o *.spv main.exe -f
