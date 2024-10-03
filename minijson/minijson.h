#pragma once
#include <stdbool.h>
#include <stdint.h>

const char* minijson_version();

enum JsonObjType {
    JObj,
    JArray,
    JNum,
    JBool,
    JStr,
    JNull,
};

struct JsonBaseObj {
    enum JsonObjType type;
    void* ptr;
};

struct JsonBool {
    bool data;
};

struct JsonNum {
    bool isInt;
    int64_t Int64;
    double Double;
};

struct JsonArray {
    int length;
    struct JsonBaseObj* list;
};

struct JsonObj {
    int length;
    char** keyList;
    struct JsonBaseObj* valueList;
};

struct JsonObj* minijson_parse_str();
void minijson_to_str();
