#ifndef TBSYS_CONFIG_H
#define TBSYS_CONFIG_H

#include <string>
#include <ext/hash_map>
#include "stringutil.h"

namespace tbsys {

    struct str_hash {
        size_t operator()(const std::string& str) const {
            return __gnu_cxx::__stl_hash_string(str.c_str());
        }
    };

    struct char_equal {
        bool operator()(const char* s1, const char* s2) const {
            return strcmp(s1, s2) == 0;
        }
    };
    typedef __gnu_cxx::hash_map<std::string, std::string, str_hash> STR_STR_MAP;
    typedef STR_STR_MAP::iterator STR_STR_MAP_ITER;
    typedef __gnu_cxx::hash_map<std::string, STR_STR_MAP*, str_hash> STR_MAP;
    typedef STR_MAP::iterator STR_MAP_ITER;
    
    #define TBSYS_CONFIG tbsys::CConfig::getCConfig()
    
    /** 
     * @brief 解析配置文件,并将配置项以key-value的形式存储到内存中
     */
    class  CConfig {
        public:
            CConfig();
            ~CConfig();
            
          
            int load(const char *filename);
          
            const char *getString(const char *section, const std::string& key, const char *d = NULL);
           
            std::vector<const char*> getStringList(const char *section, const std::string& key);
           
            int getInt(char const *section, const std::string& key, int d = 0);
           
            std::vector<int> getIntList(const char *section, const std::string& key);
         
            int getSectionKey(const char *section, std::vector<std::string> &keys);
           
            int getSectionName(std::vector<std::string> &sections);
            
            std::string toString();
           
            static CConfig& getCConfig();
            
        private:
          
            STR_MAP m_configMap;
            
        private:
           
            int parseValue(char *str, char *key, char *val);
            char *isSectionName(char *str);     
    };
}

#endif
