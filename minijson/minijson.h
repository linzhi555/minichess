#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    JNull,
    JBool,
    JNum,
    JStr,
    JArray,
    JMap,
} JsonObjType;

typedef struct {
    bool data;
} JsonBool;

typedef struct {
    bool isInt;
    int64_t Int64;
    double Double;
} JsonNum;

typedef struct {
    int len;
    int cap;
    char* data;
} JsonStr;

typedef struct {
    int length;
    struct JsonBaseObj* list;
} JsonArray;

typedef struct {
    int len;
    int cap;
    struct JsonStr* keyList;
    struct JsonBaseObj* valueList;
} JsonMap;

typedef struct {
    JsonObjType type;
    union {
        JsonBool jsonBool;
        JsonNum jsonNum;
        JsonStr jsonStr;
        JsonArray jsonArray;
        JsonMap jsonMap;
    };
} JsonBaseObj;

const char* minijson_version();
JsonMap* minijson_parse_str();
void minijson_to_str();
