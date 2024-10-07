#include "minijson.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_JSTR_LEN 100

int jnum_from_cstr(JsonNum* num, const char* cs, int n) {
    int64_t intRes;
    double floatRes;

    if (strlen(cs) <= n) {
        return 0;
    }

    char* temp = malloc(sizeof(char) * (n + 1));
    strcpy(temp, cs);

    int res = sscanf(cs, "%ld", &intRes);
    if (res != 0) {
        num->isInt = true;
        num->Double = intRes;
        return n;
    }

    res = sscanf(cs, "%lf", &floatRes);
    if (res != 0) {
        num->isInt = false;
        num->Double = floatRes;
        return n;
    }

    free(temp);
    return 0;
}

void init_jstr(JsonStr* str) {
    str->len = 0;
    str->cap = 100;
    str->data = calloc(sizeof(char), INIT_JSTR_LEN);
}

int _jstr_adjust_cap(JsonStr* str, int newCap) {
    if (str->len + 1 <= newCap) {
        str->data = realloc(str->data, newCap);
        return newCap;
    }

    return 0;
}

int jstr_cpy_cstr(JsonStr* str, char* cs) {
    int len = strlen(cs);
    if (str->cap <= len) {
        str->cap = len * 2;
        str->data = realloc(str->data, str->cap);
    }

    strcpy(str->data, cs);
    str->len = len;

    return len;
}

const char* jstr_cstr(const JsonStr* str) {
    return str->data;
}

int jstr_to_jnum(const JsonStr* str, JsonNum* num) {
    return jnum_from_cstr(num, jstr_cstr(str), str->len);
}

int jstr_append(JsonStr* str, char newch) {
    if (str->cap <= str->len - 1) {
        _jstr_adjust_cap(str, str->cap * 2);
    }

    str->data[str->len] = newch;
    str->len++;
    str->data[str->len] = '\0';
    return str->len;
}

typedef enum {
    TNum,
    TStr,
    TTrue,
    TFalse,
    TNUll,
    TLBrace,
    TRBrace,
    TComma,
    TColon,
    TLBracket,
    TRBracket,
} TokenType;

typedef struct {
    TokenType type;
    JsonStr content;
} Token;

typedef struct {
    const char* src;
    int cursor;
    bool isFinished;
    Token curToken;
    int curLen;
} Lexer;

int init_lexer(Lexer* l, const char* src) {
    l->src = src;
    l->cursor = 0;
    l->isFinished = false;
    l->curLen = 0;
    init_jstr(&l->curToken.content);
    return 0;
}

const Token* lexer_peek(const Lexer* l) {
    return &l->curToken;
}

bool lexer_peek_expect(const Lexer* l, TokenType expect) {
    const Token* tk = lexer_peek(l);
    if (tk == NULL) return false;
    return tk->type == expect;
}

int lexer_next(Lexer* l) {
    l->cursor += l->curLen;
    lexer_peek(l);
    return l->curLen;
}

static int parse_base_obj(JsonBaseObj* obj, Lexer* l);

static int parse_null(Lexer* l) {
    int old = l->cursor;
    if (!lexer_peek_expect(l, TNUll)) return 0;
    lexer_next(l);
    return l->cursor - old;
}

static int parse_num(JsonNum* num, Lexer* l) {
    int old = l->cursor;
    if (!lexer_peek_expect(l, TNum)) return 0;
    const Token* tk = lexer_peek(l);

    return l->cursor - old;
}

static int parse_bool(JsonBool* num, Lexer* l) {
    int old = l->cursor;

    if (!lexer_peek_expect(l, TTrue) && !lexer_peek_expect(l, TFalse)) return 0;
    Token tkbool;
    lexer_next(l);
    return l->cursor - old;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_str(JsonStr* num, Lexer* l) {
    int old = l->cursor;

fail:
    l->cursor = old;
    return 0;
}

static int parse_array(JsonArray* resObj, Lexer* l) {
    int old = l->cursor;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_obj_field(JsonMap* resObj, Lexer* l) {
    int old = l->cursor;
    JsonStr strObj;
    JsonBaseObj fieldObj;

    if (!lexer_peek_expect(l, TStr)) goto fail;
    Token temp;
    lexer_next(l);
    strObj = temp.content;

    if (!lexer_peek_expect(l, TColon)) goto fail;
    lexer_next(l);

    if (parse_base_obj(&fieldObj, l) == 0) goto fail;

    return l->cursor - old;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static JsonMap newJsonMap() {
    JsonMap res = { .cap = 0, .len = 0, .keyList = NULL, .valueList = NULL };
    return res;
}

static int parse_map(JsonMap* resObj, Lexer* l) {
    int old = l->cursor;
    JsonMap obj = newJsonMap();

    if (!lexer_peek_expect(l, TLBrace)) goto fail;

    lexer_next(l);
    while (parse_obj_field(&obj, l) != 0)
        ;

    if (!lexer_peek_expect(l, TRBrace)) goto fail;

    lexer_next(l);
    return l->cursor - old;

fail:
    l->cursor = old;
    lexer_peek(l);
    return 0;
}

static int parse_base_obj(JsonBaseObj* obj, Lexer* l) {
    int offset = 0;

    offset = parse_map(&obj->jsonMap, l);
    if (offset != 0) {
        obj->type = JMap;
        return offset;
    }

    offset = parse_array(&obj->jsonArray, l);
    if (offset != 0) {
        obj->type = JArray;
        return offset;
    }

    offset = parse_str(&obj->jsonStr, l);
    if (offset != 0) {
        obj->type = JStr;
        return offset;
    }

    offset = parse_num(&obj->jsonNum, l);
    if (offset != 0) {
        obj->type = JNum;
        return offset;
    }

    offset = parse_bool(&obj->jsonBool, l);
    if (offset != 0) {
        obj->type = JBool;
        return offset;
    }

    offset = parse_null(l);
    if (offset != 0) {
        obj->type = JNull;
        return offset;
    }

    return 0;
}

const char* minijson_version() {
    return "0.0.1";
}
JsonMap* minijson_parse_str();
void minijson_to_str();
