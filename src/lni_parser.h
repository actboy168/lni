#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>

namespace lni {
    enum class ctype : uint8_t {
        none = 0,
        digit = 1,
        alpha = 2,
        underscode = 4,
        hexdigit = digit | alpha,
    };

    static ctype ctypemap[256] = {
        /*0x00*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0x10*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0x20*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0x30*/ ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0x40*/ ctype::none, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
        /*0x50*/ ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::none, ctype::none, ctype::none, ctype::none, ctype::underscode,
        /*0x60*/ ctype::none, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
        /*0x70*/ ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0x80*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0x90*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0xA0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0xB0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0xC0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0xD0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0xE0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
        /*0xF0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
    };

    inline void expect(const char*& p, const char c) {
        assert(*p == c);
        p++;
    }

    inline bool is_digit(char c) {
        return ctypemap[(unsigned char)c] == ctype::digit;
    }

    inline bool is_hexdigit(char c) {
        return (uint8_t)ctypemap[(unsigned char)c] & (uint8_t)ctype::digit;
    }

    inline bool equal(const char* p, const char c) {
        return *p == c;
    }

    inline bool equal(const char* p, const char c[]) {
        return *p == c[0] || *p == c[1];
    }

    inline bool consume(const char*& p, const char c) {
        if (equal(p, c)) {
            p++;
            return true;
        }
        return false;
    }

    inline bool consume(const char*& p, const char c[]) {
        if (equal(p, c)) {
            p++;
            return true;
        }
        return false;
    }

    struct lex {
        const char* z;
        int line = 1;

        lex(const char* input) : z(input) {
            if (z[0] == '\xEF' && z[1] == '\xBB' && z[2] == '\xBF') {
                z += 3;
            }
        }

        void incline() {
            char old = *z;
            assert(equal(z, "\n\r"));
            z++;
            if (equal(z, "\n\r") && !equal(z, old))
                z++;
            ++line;
        }

        void parse_whitespace() {
            for (;;) {
                if (!consume(z, " \t")) {
                    if (equal(z, "\n\r")) {
                        incline();
                    }
                    else {
                        return;
                    }
                }
            }
        }

        void parse_whitespace_and_comments() {
            parse_whitespace();
            while (z[0] == '-' && z[1] == '-') {
                for (z += 2; *z != '\r' && *z != '\n' && *z != '\0'; z++);
                parse_whitespace();
            }
        }

        template <class Handler>
        void parse_split(Handler& h) {
            while (z[0] == '`' && z[1] == '`' && z[2] == '`' && z[3] == '`') {
                h.accept_split();
                for (z += 4; *z != '\r' && *z != '\n' && *z != '\0'; z++);
                parse_whitespace();
            }
        }

        template <class Handler, class ... Args>
        bool error(Handler& h, const char* fmt, const Args& ... args) {
            h.accept_error(line, fmt, args...);
            return false;
        }

        template <class Handler>
        bool parse_number(Handler& h)
        {
            const char* p = z;
            const char *expo = "Ee";
            consume(z, '-');
            if (consume(z, '0')) {
                if (consume(z, "xX")) {
                    expo = "Pp";
                }
            }
            for (;;) {
                if (consume(z, expo))
                    consume(z, "-+");
                if (is_hexdigit(*z))
                    z++;
                else if (*z == '.')
                    z++;
                else break;
            }
            if (!h.accept_number(p, z - p)) {
                return error(h, "malformed number");
            }
            return true;
        }

        template <class Handler>
        bool parse_string(Handler& h, char del) {
            std::vector<char> s;
            expect(z, del);
            const char* p = z;
            for (char ch = *p++; ch != del; ch = *p++) {
                switch (ch) {
                case '\\':
                    switch (*p++) {
                    case '"':  s.push_back('"'); break;
                    case '\'': s.push_back('\''); break;
                    case '\\': s.push_back('\\'); break;
                    case 'a':  s.push_back('\a'); break;
                    case 'b':  s.push_back('\b'); break;
                    case 'f':  s.push_back('\f'); break;
                    case 'n':  s.push_back('\n'); break;
                    case 'r':  s.push_back('\r'); break;
                    case 't':  s.push_back('\t'); break;
                    case 'v':  s.push_back('\v'); break;
                        //case 'x':  break;
                        //case 'u':  break;
                        //case 'z':  break;
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9': {
                        p--;
                        int r = 0;
                        for (int i = 0; i < 3 && is_digit(*p); ++i, p++) {
                            r = 10 * r + *p - '0';
                        }
                        if (r > 0xFF)
                            return error(h, "decimal escape too large");
                        s.push_back((char)(unsigned char)r);
                        break;
                    }
                    default:
                        return error(h, "invalid escape sequence near '\\%c'", *(p - 1));
                    }
                    break;
                case '\0':
                case '\n':
                case '\r':
                    return error(h, "unfinished string near <eof>");
                default:
                    s.push_back(ch);
                }
            }
            z = p;
            h.accept_string(s.data(), s.size());
            return true;
        }

        template <class Handler>
        bool parse_long_string(Handler& h) {
            const char* p = z;
            expect(z, '[');
            int sep = 0;
            while (consume(z, '=')) {
                sep++;
            }
            if (!consume(z, '[')) {
                return error(h, "invalid long string delimiter near '%s'", std::string(p, z - p).c_str());
            }
            if (equal(z, "\n\r")) {
                incline();
            }
            p = z;
            for (;;) {
                switch (*z) {
                case '\0':
                    return error(h, "unfinished long string near <eof>");
                case ']': {
                    z++;
                    int endsep = 0;
                    while (consume(z, '=')) {
                        endsep++;
                    }
                    if (sep == endsep && consume(z, ']')) {
                        h.accept_string(p, z - p - 2 - sep);
                        return true;
                    }
                    break;
                }
                case '\n':
                case '\r':
                    incline();
                    break;
                default:
                    z++;
                    break;
                }
            }
        }

        template <class Handler>
        bool parse_table(Handler& h) {
            int ary = 0;
            expect(z, '{');
            h.accept_table(ary);
            for (;;) {
                parse_whitespace_and_comments();
                if (consume(z, '}')) {
                    return true;
                }
                if (!parse_value<Handler>(h)) {
                    return false;
                }
                parse_whitespace_and_comments();
                if (!consume(z, '=')) {
                    h.accept_table_array(ary);
                }
                else {
                    parse_whitespace_and_comments();
                    if (!parse_value<Handler>(h)) {
                        return false;
                    }
                    h.accept_table_hash();
                    parse_whitespace_and_comments();
                }
                if (consume(z, '}')) {
                    return true;
                }
                if (!consume(z, ',')) {
                    return error(h, "',' expected near '%c'", *z);
                }
            }
            return true;
        }

        template <class Handler>
        bool parse_value(Handler& h) {
            switch (*z) {
            case '{':  return parse_table(h);
            case '[':  return parse_long_string(h);
            case '"': case '\'': return parse_string(h, *z);
            case '-': case '.':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': return parse_number(h);
            case '\0':
                return error(h, "unexpected symbol near <eof>");
            default: {
                const char* p = z;
                for (;;) {
                    switch (*z) {
                    case '=':
                    case ',':
                    case '}':
                    case '\n':
                    case '\r':
                    case '\t':
                    case '\0':
                    case ' ':
                        parse_identifier<Handler>(h, p, z - p);
                        return true;
                    default:
                        z++;
                        break;
                    }
                }
            }
            }
        }

        template <class Handler>
        bool parse_value_ext(Handler& h) {
            switch (*z) {
            case '{':  return parse_table(h);
            case '[':  return parse_long_string(h);
            case '"': case '\'': return parse_string(h, *z);
            case '-': case '.':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': return parse_number(h);
            case '\0':
                return error(h, "unexpected symbol near <eof>");
            default: {
                const char* p = z;
                for (;;) {
                    switch (*z) {
                    case '\n':
                    case '\r':
                    case '\0':
                        parse_identifier<Handler>(h, p, z - p);
                        return true;
                    case '-':
                        if (z[1] == '-') {
                            parse_identifier<Handler>(h, p, z - p);
                            return true;
                        }
                    default:
                        z++;
                        break;
                    }
                }
            }
            }
        }

        template <class Handler>
        bool parse_keyword(Handler& h, const char* str, size_t len) {
            switch (len) {
            case 3:
                if (str[0] == 'n' && str[1] == 'i' && str[2] == 'l') {
                    h.accept_nil();
                    return true;
                }
                break;
            case 4:
                if (str[0] == 't' && str[1] == 'r' && str[2] == 'u' && str[3] == 'e') {
                    h.accept_boolean(true);
                    return true;
                }
                break;
            case 5:
                if (str[0] == 'f' && str[1] == 'a' && str[2] == 'l' && str[3] == 's' && str[4] == 'e') {
                    h.accept_boolean(false);
                    return true;
                }
                break;
            }
            return false;
        }

        template <class Handler>
        void parse_identifier(Handler& h, const char* str, size_t len) {
            const char* beg = str;
            const char* end = str + len - 1;
            for (; beg <= end; ++beg) {
                if (!equal(beg, " \t")) {
                    break;
                }
            }
            for (; beg <= end; --end) {
                if (!equal(end, " \t")) {
                    break;
                }
            }
            if (beg < end) {
                if (parse_keyword(h, beg, end - beg + 1))
                    return;
            }
            h.accept_string(beg, end - beg + 1);
        }

        template <class Handler>
        bool parse_key(Handler& h) {
            switch (*z) {
            case '"': case '\'': return parse_string(h, *z);
            case '-':
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9': return parse_number(h);
            case '\0':
                return error(h, "unexpected symbol near <eof>");
            case '=':
                return error(h, "unexpected symbol near '='");
            default:
                break;
            }
            const char* p = z;
            for (;;) {
                switch (*z) {
                case '=':
                case '.':
                case '\n':
                case '\r':
                case '\t':
                case '\0':
                case ' ':
                    h.accept_string(p, z - p);
                    return true;
                default:
                    z++;
                    break;
                }
            }
        }

        template <class Handler>
        bool parse_section_key(Handler& h) {
            switch (*z) {
            case '"': case '\'': return parse_string(h, *z);
            case '\0':
                return error(h, "unexpected symbol near <eof>");
            default:
                break;
            }
            const char* p = z;
            for (;;) {
                switch (*z) {
                case ':':
                case '.':
                case '<':
                case '>':
                case '[':
                case ']':
                case '\n':
                case '\r':
                case '\t':
                case '\0':
                case ' ':
                    h.accept_string(p, z - p);
                    return true;
                default:
                    z++;
                    break;
                }
            }
        }

        template <class Handler>
        bool parse_section(Handler& h) {
            expect(z, '[');
            bool split = consume(z, '[');
            h.accept_split_end();
            parse_whitespace();
            unsigned int level = 1;
            for (;;level++) {
                if (!consume(z, '.')) {
                    break;
                }
            }
            if (!parse_section_key(h)) {
                return false;
            }
            unsigned int last;
            if (!h.accept_section(level, last)) {
                return error(h, "The previous section is Lv.%d, but current is Lv.%d.", last, level);
            }
            parse_whitespace();
            bool inherited = false;
            if (consume(z, ':')) {
                parse_whitespace();
                if (!parse_section_key(h)) {
                    return false;
                }
                h.accept_section_inherited();
                for (;;) {
                    if (consume(z, '.')) {
                        if (!parse_section_key(h)) {
                            return false;
                        }
                        h.accept_section_child();
                        continue;
                    }
                    break;
                }
                inherited = true;
            }
            h.accept_section_end(inherited, level==1);

            parse_whitespace();
            if (!consume(z, ']') || (split && !consume(z, ']'))) {
                return error(h, "']' expected near '%c'", *z);
            }
            if (split) {
                h.accept_split();
            }
            return true;
        }

        template <class Handler>
        bool parse_internal_section(Handler& h) {
            expect(z, '<');
            parse_whitespace();
            if (!parse_section_key(h)) {
                return false;
            }
            if (!h.accept_internal_section()) {
                return error(h, "unknown internal object");
            }
            parse_whitespace();
            if (!consume(z, '>')) {
                return error(h, "'>' expected near '%c'", *z);
            }
            return true;
        }

        template <class Handler>
        bool parse_set(Handler& h) {
            parse_split(h);
            if (!parse_key(h)) {
                return false;
            }
            parse_whitespace_and_comments();
            if (!consume(z, '=')) {
                return error(h, "'=' expected near '%c'", *z);
            }
            parse_whitespace_and_comments();
            if (!parse_value_ext<Handler>(h)) {
                return false;
            }
            h.accept_set();
            return true;
        }

        template <class Handler>
        bool parse_root_object(Handler& h) {
            if (equal(z, "[<")) {
                return true;
            }
            h.accept_split_end();
            h.accept_section_sub();
            while (*z != '<' && *z != '[' && *z != '\0') {
                if (!parse_set(h)) {
                    return false;
                }
                parse_whitespace_and_comments();
            }
            h.accept_object();
            return true;
        }

        template <class Handler>
        bool parse_object(Handler& h) {
            if (equal(z, '[')) {
                if (!parse_section(h)) {
                    return false;
                }
            }
            else if (equal(z, '<')) {
                if (!parse_internal_section(h)) {
                    return false;
                }
            }
            else {
                return error(h, "'[' expected near '%c'", *z);
            }
            parse_whitespace_and_comments();
            while (*z != '<' && *z != '[' && *z != '\0') {
                if (!parse_set(h)) {
                    return false;
                }
                parse_whitespace_and_comments();
            }
            h.accept_object();
            return true;
        }

        template <class Handler>
        bool parse(Handler& h) {
            h.accept_root();
            parse_whitespace_and_comments();
            if (!equal(z, '\0')) {
                if (!parse_root_object(h)) {
                    return false;
                }
            }
            while (!equal(z, '\0')) {
                if (!parse_object(h)) {
                    return false;
                }
            }
            h.accept_root_end();
            return true;
        }
    };
}
