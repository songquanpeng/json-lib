#include "json.h"
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#define EXPECT(c, ch)       do { assert(*(c)->json == (ch)); (c)->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char *json;
} json_context;

static void json_parse_whitespace(json_context *c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    c->json = p;
}

/*
 * null = "null"
 * true = "true"
 * false = "false"
 */
static int json_parse_literal(json_context *c, json_value *v, const char *literal, json_type type) {
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 1; literal[i]; i++) {
        if (c->json[i - 1] != literal[i]) {
            return JSON_PARSE_INVALID_VALUE;
        }
    }
    c->json += i - 1;
    v->type = type;
    return JSON_PARSE_OK;
}

/*
 * number = [ "-" ] int [ frac ] [ exp ]
 * int = "0" / digit1-9 *digit
 * frac = "." 1*digit
 * exp = ("e" / "E") ["-" / "+"] 1*digit
 */
static int json_parse_number(json_context *c, json_value *v) {
    const char *p = c->json;
    if (*p == '-') {
        p++;
    }
    if (*p == '0') {
        p++;
    } else {
        if (!ISDIGIT1TO9(*p)) return JSON_PARSE_INVALID_VALUE;
        for (p++;ISDIGIT(*p);p++);
    }
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return JSON_PARSE_INVALID_VALUE;
        for (p++;ISDIGIT(*p);p++);
    }
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '-' || *p == '+') {
            p++;
        }
        if (!ISDIGIT(*p)) return JSON_PARSE_INVALID_VALUE;
        for (p++;ISDIGIT(*p);p++);
    }

    errno = 0;
    v->num = strtod(c->json, NULL);
    if (errno == ERANGE && (v->num == HUGE_VAL || v->num == -HUGE_VAL)) return JSON_PARSE_NUMBER_TOO_BIG;

    c->json = p;
    v->type = JSON_NUMBER;
    return JSON_PARSE_OK;
}

/* value = null / false / true / number */
static int json_parse_value(json_context *c, json_value *v) {
    switch (*c->json) {
        case 'n':
            return json_parse_literal(c, v, "null", JSON_NULL);
        case 't':
            return json_parse_literal(c, v, "true", JSON_TRUE);
        case 'f':
            return json_parse_literal(c, v, "false", JSON_FALSE);
        case '\0':
            return JSON_PARSE_EXPECT_VALUE;
        default:
            return json_parse_number(c, v);
    }
}

/* JSON-text = ws value ws */
int json_parse(json_value *v, const char *json) {
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

json_type json_get_type(const json_value *v) {
    assert(v != NULL);
    return v->type;
}

double json_get_number(const json_value *v) {
    assert(v != NULL && v->type == JSON_NUMBER);
    return v->num;
}
