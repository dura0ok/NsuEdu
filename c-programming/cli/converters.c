#include "converters.h"
#include "stdio.h"
#include "string.h"

const char main_system_name[] = "metre";

double meter_to_feet(const double meters){
    return meters * 3.28084;
}

double metre_to_metre(const double meters){
    return meters;
}

double feet_to_metre(const double feet){
    return feet * 0.3048;
}

double metre_to_mile(const double meters){
    return meters * 1/1609.34;
}
double mile_to_metre(const double miles){
    return miles * 1609.34;
}

static struct Converter converters[] = {
        {.from="metre", .to="feet", .func=&meter_to_feet},
        {.from="metre", .to="mile", .func=&metre_to_mile},
        {.from="metre", .to="metre", .func=&metre_to_metre},
        {.from="feet", .to="metre", .func=&feet_to_metre},
        {.from="mile", .to="metre", .func=&mile_to_metre},
};


struct Converter* find_to_main_converter(char from[BUFSIZ]){
    size_t converters_size = sizeof(converters) / sizeof(struct Converter);
    for (size_t i = 0; i < converters_size; ++i) {
        if ((strcmp(from, converters[i].from)) == 0 && strcmp(converters[i].to, main_system_name) == 0) return &converters[i];
    }
    return NULL;
}

struct Converter* find_from_main_converter(char to[BUFSIZ]){
    size_t converters_size = sizeof(converters) / sizeof(struct Converter);
    for (size_t i = 0; i < converters_size; ++i) {
        if ((strcmp(to, converters[i].to)) == 0 && strcmp(converters[i].from, main_system_name) == 0) return &converters[i];
    }
    return NULL;
}