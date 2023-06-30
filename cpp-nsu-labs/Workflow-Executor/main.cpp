#include  <iostream>
#include "config//Config.h"

int main(){
    try{
        Config config("conf.txt");
        auto blockObjects = config.ParseConfig();
        auto order = config.ParseConveyor();


        std::string last_result;
        for (int i = 0; i < order.size(); ++i) {
            auto res = blockObjects[i].get()->operation(last_result);
            last_result = res;
        }

        std::cout << last_result;
    }catch (const std::exception &e) {
        std::cout << e.what();
    }
    return 0;
}
