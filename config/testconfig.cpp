
#include "config.h"
#include <stdint.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace tbsys;
using namespace std;

int main(int argc, char *argv[])
{
    if( argc < 2 ) 
	{
        fprintf(stderr, "%s filename\n", argv[0]);
        return 1;
    }
   
    if( TBSYS_CONFIG.load(argv[1]) ) 
	{
        fprintf(stderr, "load config failure: %s\n", argv[1]);
    }
   
    fprintf(stderr, "string=%s\n", TBSYS_CONFIG.getString("test_section", "string"));
    fprintf(stderr, "int=%d\n", TBSYS_CONFIG.getInt("test_section", "int"));
    
    vector<const char*> strList = TBSYS_CONFIG.getStringList("test_section", "stringlist");
    for(int i=0; i<(int)strList.size(); i++) 
	{
        fprintf(stderr, "string%d=%s\n", i+1, strList[i]);
    }
    
    vector<int> intList = TBSYS_CONFIG.getIntList("test_section", "intlist");
    for(int i=0; i<(int)intList.size(); i++) 
	{
        fprintf(stderr, "int%d=%d\n", i+1, intList[i]);
    }
    
    vector<string> keys;
    TBSYS_CONFIG.getSectionKey("test_section", keys);
    for(uint32_t i=0; i<keys.size(); i++) 
	{
        fprintf(stderr, "key_%d=%s\n", i+1, keys[i].c_str());
    }
    fprintf(stderr,"---------------------------------");
    fprintf(stderr, "\n%s\n", TBSYS_CONFIG.toString().c_str());
                
    return 0;
}
