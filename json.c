#include "json.h"
#include <assert.h>
#include <stdlib.h>

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
} json_context;

static void json_parse_whitespace(json_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    c->json = p;
}

/* null = "null" */
static int json_parse_null(json_context* c, json_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l') {
        return JSON_PARSE_INVALID_VALUE;
    }
    c->json += 3;
    v->type = JSON_NULL;
    return JSON_PARSE_OK;
}

/* true  = "true" */
static int json_parse_true(json_context* c, json_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e') {
        return JSON_PARSE_INVALID_VALUE;
    }
    c->json += 3;
    v->type = JSON_TRUE;
    return JSON_PARSE_OK;
}

/* false  = "false" */
static int json_parse_false(json_context* c, json_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e') {
        return JSON_PARSE_INVALID_VALUE;
    }
    c->json += 4;
    v->type = JSON_FALSE;
    return JSON_PARSE_OK;
}

/* value = null / false / true */
static int json_parse_value(json_context* c, json_value* v) {
    switch (*c->json) {
        case 'n': return json_parse_null(c, v);
        case 't': return json_parse_true(c, v);
        case 'f': return json_parse_false(c, v);
        case '\0': return JSON_PARSE_EXPECT_VALUE;
        default: return JSON_PARSE_INVALID_VALUE;
    }
}

/* JSON-text = ws value ws */
int json_parse(json_value* v, const char* json){
    json_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = JSON_NULL;
    json_parse_whitespace(&c);
    ret = json_parse_value(&c, v);
    json_parse_whitespace(&c);
    if (ret != JSON_PARSE_OK) {
        return ret;
    }
    if (*c.json != '\0') {
        return JSON_PARSE_ROOT_NOT_SINGULAR;
    }
    return JSON_PARSE_OK;
}

json_type json_get_type(const json_value* v) {
    assert(v != NULL);
    return v->type;
}
