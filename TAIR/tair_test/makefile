CC := g++
TARGET := tair_test
INCLUDE := -I/root/lib/include/tbnet/ -I/root/lib/include/tbsys/ -I/usr/local/storage/tair_bin/include/
LIBS := -lpthread -lz -lrt
Tair_lib := /usr/local/storage/tair_bin/lib/libtairclientapi.a /root/lib/lib/libtbnet.a /root/lib/lib/libtbsys.a

# C++语言编译参数
CXXFLAGS := -g -Wall -D_REENTRANT 

# C预处理参数
# CPPFLAGS := 

OBJECTS := tair_test.o Opjob.o Thread_work.o 

$(TARGET): $(OBJECTS) 
	$(CC) -o $(TARGET) $(OBJECTS) $(Tair_lib) $(LIBS)

# $@表示所有目标集
%.o:%.cpp 
	$(CC) -c $(CXXFLAGS) $(INCLUDE) $< -o $@ 

.PHONY : clean
clean: 
	-rm -f $(OBJECTS) $(TARGET)
 