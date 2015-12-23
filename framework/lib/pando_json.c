#include "pando_json.h"
#include "../platform/include/pando_types.h"

static char *json_buf;
static int json_buf_len;
static int pos;

static int FUNCTION_ATTRIBUTE
json_putchar(int c)
{
    if (json_buf != NULL && pos < json_buf_len) 
    {
        json_buf[pos++] = c;
        return c;
    }

    return 0;
}

int FUNCTION_ATTRIBUTE
pando_json_print(struct jsontree_value * json_value, char * dst, int len)
{
    if( dst == NULL) 
    {
        return -1;
    }
    json_buf = dst;
    json_buf_len = len;
    pos = 0;

    struct jsontree_context js_ctx;
    js_ctx.values[0] = json_value;
    jsontree_reset(&js_ctx);
    js_ctx.putchar = json_putchar;

    while (jsontree_print_next(&js_ctx));

    json_buf[pos] = 0;
    
    return pos;
}
