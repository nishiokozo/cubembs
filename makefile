main.exe:	main.o
	clang++ -m64 -std=c++11 -o main.exe main.o -LC:/VulkanSDK/1.0.54.0/Lib -lvulkan-1

main.o:	main.cpp
	clang++ -m64 -std=c++11 -c -g -IC:/VulkanSDK/1.0.54.0/Include -o main.o main.cpp -Wno-narrowing -fpermissive -Werror

clean:
	rm *.o -f
	rm *.exe -f	