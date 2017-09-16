CC := g++
TARGET := testconfig
INCLUDE := -I/root/lib/include/tbnet/ -I/root/lib/include/tbsys/ -I/usr/local/storage/tair_bin/include/
LIBS := -lpthread -lz -lrt
Tair_lib := /usr/local/storage/tair_bin/lib/libtairclientapi.a /root/lib/lib/libtbnet.a /root/lib/lib/libtbsys.a

# C++语言编译参数
CXXFLAGS := -g -Wall -D_REENTRANT 

# C预处理参数
# CPPFLAGS := 

OBJECTS := testconfig.o config.o stringutil.o 

$(TARGET): $(OBJECTS) 
	$(CC) -o $(TARGET) $(OBJECTS)

# $@表示所有目标集
%.o:%.cpp 
	$(CC) -c $(CXXFLAGS) $< -o $@ 

.PHONY : clean
clean: 
	-rm -f $(OBJECTS) $(TARGET)
 
