#include "tblog.h"

using namespace tbsys;

int main(int argc, char *argv[])
{
    //TBSYS_LOG(INFO, "xxx: %s:%d", "xxxx", 1);
    //TBSYS_LOG(ERROR, "xxx: %s:%d", "xxxx", 1);
    TBSYS_LOGGER.setFileName("/tmp/test.txt");
    TBSYS_LOGGER.setLogLevel("WARN");
	TBSYS_LOG(ERROR,"%d", 4+5 ); 
    for(int i=0; i<50; i++) {
        TBSYS_LOG(ERROR, "xxx: %s:%d", "xxxx", i);
    }

    return 0;
}

