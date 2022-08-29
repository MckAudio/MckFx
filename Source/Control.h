/**
 * @file Control.h
 * @author MckAudio (henri.meissner@gmx.de)
 * @brief 
 * @version 0.1
 * @date 2022-08-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <string>


namespace Data {
    class Control {
        public:
            std::string name { "Control" };
            double value { 0.0 };
            double minVal { 0.0 };
            double maxVal { 127.0 };
            double stepVal { 1.0 };
            std::string unit { "" };
    };
}