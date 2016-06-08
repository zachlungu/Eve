#include <core.h>

char *hex_digits="0123456789abcdef";

#define MAX(a, b) ((a)>(b)?(a):(b))

void format_number(string s, iu64 x, int base, int pad)
{
    if ((x > 0) || (pad > 0)) {
        format_number(s, x/base, base, pad - 1);
        string_insert(s, hex_digits[x%base]);
    } 
}

extern void print_value();

void vbprintf(string s, string fmt, va_list ap)
{
    character i;
    int state = 0;
    int base = 0;
    int pad;
    int count = 0;

    string_foreach(i, fmt) {
        switch (state){
        case 2:
            for (int j = 0; j < count; j++)
                string_insert(s, i);
            state = 0;
            break;

        case 0:
            base = 10;
            pad = 0;
            if (i == '%') state = 3;
            else string_insert(s, i);
            break;

        case 1:
            if ((i >= '0') && (i <= '9')) {
                pad = pad * 10 + digit_of(i);
                break;
            } 

        case 3:
            switch (i) {
            case '0':
                state = 1;
                break;

            case '%':
                string_insert(s, '\%');
                break;
                
            case 'b':
                string_concat(s, (va_arg(ap, string)));
                break;

            case 'n':
                count = va_arg(ap, unsigned int);
                state = 2;
                break;
                
            case 'c':
                string_insert(s, va_arg(ap, int));
                break;

            case 's':
                {
                    char *c = va_arg(ap, char *);
                    if (!c) c = (char *)"(null)";
                    int len = cstring_length(c);
                    for (; *c; c++)
                        string_insert(s, *c);
                }
                break;
                
            // there is plenty wrong here
            case 'p':
                pad = 16;
                unsigned int x = va_arg(ap, unsigned int);
                format_number(s, x, 16, pad?pad:1);
                break;
                
            case 'x':
                base=16;
            case 'o':
                if (base == 10) base=8;
            case 'u':
                {
                    unsigned int x = va_arg(ap, unsigned int);
                    format_number(s, x, base, pad?pad:1);
                    break;
                }

             // layer violation..meh
            case 'v':
                print_value(s, va_arg(ap, void *));
                break;
                
            case 'X':
                // xxx - utf8 will break this
                 {
                  buffer xx = va_arg(ap, buffer);
                  string_foreach(i, xx){
                     print_byte(s, i);
                  }
                 }
                break;
                
            case 'd': case 'i':
                {
                    int x = va_arg(ap, int);
                    if (x <0){
                        string_insert(s, '-');
                        x = -x;
                    }
                    format_number(s, (unsigned int)x, base, pad?pad:1);
                    break;
                }
            default:
                break;
            }
            // badness
            if (state == 3) 
                state = 0;
            break;
        }
    }
}


string aprintf(heap h, char *fmt, ...)
{
    string b = allocate_string(h);
    va_list ap;
    string f = string_from_cstring(h, fmt);
    va_start(ap, fmt);
    vbprintf(b, f, ap);
    va_end(ap);
    deallocate(h, f);
    return(b);
}

void bbprintf(string b, string fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vbprintf(b, fmt, ap);
    va_end(ap);
}



