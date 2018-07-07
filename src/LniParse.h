#pragma once

#include <vector>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>	  
#include <lua.hpp>

namespace lni {

	enum class ctype {
		none = 0,
		digit = 1,
		alpha = 2,
		underscode = 4,
	};

	static ctype ctypemap[256] = {
		/*0x00*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x10*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x20*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x30*/ ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x40*/ ctype::none, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
		/*0x50*/ ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::none, ctype::none, ctype::none, ctype::none, ctype::underscode,
		/*0x60*/ ctype::none, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
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

	inline void expect(const char*& p, const char c)
	{
		assert(*p == c);
		p++;
	}

	inline bool is_digit(char c)
	{
		return ctypemap[c] == ctype::digit;
	}

	inline bool is_alpha_or_underscode(char c)
	{
		return !!((int)ctypemap[c] & ((int)ctype::alpha | (int)ctype::underscode));
	}

	inline bool is_alnum_or_underscode(char c)
	{
		return !!((int)ctypemap[c] & ((int)ctype::alpha | (int)ctype::digit | (int)ctype::underscode));
	}

	inline bool equal(const char* p, const char c)
	{
		return *p == c;
	}

	inline bool equal(const char* p, const char c[])
	{
		return *p == c[0] || *p == c[1];
	}

	inline bool consume(const char*& p, const char c)
	{
		if (equal(p, c)) {
			p++;
			return true;
		}
		return false;
	}

	inline bool consume(const char*& p, const char c[])
	{
		if (equal(p, c)) {
			p++;
			return true;
		}
		return false;
	}

	template <bool Convert>
	struct lex {
		const char* z;
		int line = 1;

		lex(const char* input)
			: z(input)
		{
			if (z[0] == '\xEF' && z[1] == '\xBB' && z[2] == '\xBF') {
				z += 3;
			}
		}

		void incline()
		{
			char old = *z;
			assert(equal(z, "\n\r"));
			z++;
			if (equal(z, "\n\r") && !equal(z, old))
				z++;
			++line;
		}

		void parse_whitespace()
		{
			for (;;)
			{
				if (!consume(z, " \t"))
				{
					if (equal(z, "\n\r"))
					{
						incline();
					}
					else
					{
						return;
					}
				}
			}
		}

		template <class Handler>
		void parse_whitespace_and_comments_or_execute(Handler& h)
		{
			parse_whitespace();
			for (; z[0] == '-' && z[1] == '-';) {
				if (z[2] == '!') {
					z += 3;
					const char* start = z;
					for (; !equal(z, "\n\r") && !equal(z, '\0'); z++);
					h.accept_execute(start, z);
				}
				else {
					for (z += 2; !equal(z, "\n\r") && !equal(z, '\0'); z++);
				}
				parse_whitespace();
			}
		}

		void parse_whitespace_and_comments()
		{
			parse_whitespace();
			for (; z[0] == '-' && z[1] == '-';) {
				for (z += 2; !equal(z, "\n\r") && !equal(z, '\0'); z++);
				parse_whitespace();
			}
		}

		template <class Handler, class ... Args>
		bool error(Handler& h, const char* fmt, const Args& ... args)
		{
			h.accept_error(line, fmt, args...);
			return false;
		}

		template <class Handler>
		bool parse_number(Handler& h)
		{
			const char* p = z;
			bool isint = true;
			int64_t i64 = 0;
			int exp = 0;
			bool minus = consume(p, '-');
			if (consume(p, '0'))
			{
				if (consume(p, "xX"))
				{
					for (;; p++)
					{
						if (*p >= '0' && *p <= '9') {
							if (i64 >= 0x0800000000000000) {
								return error(h, "integer too big near '%s'", std::string(z, p - z).c_str());
							}
							i64 <<= 4;
							i64 += *p - '0';
						}
						else if (*p >= 'A' && *p <= 'F') {
							if (i64 >= 0x0800000000000000) {
								return error(h, "integer too big near '%s'", std::string(z, p - z).c_str());
							}
							i64 <<= 4;
							i64 += *p - 'A' + 10;
						}
						else if (*p >= 'a' && *p <= 'f') {
							if (i64 >= 0x0800000000000000) {
								return error(h, "integer too big near '%s'", std::string(z, p - z).c_str());
							}
							i64 <<= 4;
							i64 += *p - 'a' + 10;
						}
						else {
							break;
						}
					}
					z = p;
					h.accept_integer(minus ? -i64 : i64);
					return true;
				}
			}
			else
			{
				if (!is_digit(*p))
					return error(h, "invalid number near '%s'", std::string(z, p - z + 1).c_str());
				i64 *= 10;
				i64 += *p - '0';
				for (p++; is_digit(*p); p++)
				{
					// 2^63 = 9223372036854775808
					if (i64 > 0x0CCCCCCCCCCCCCCC || (i64 == 0x0CCCCCCCCCCCCCCC && *p == '9')) {
						return error(h, "integer too big near '%s'", std::string(z, p - z).c_str());
					}
					i64 *= 10;
					i64 += *p - '0';
				}
			}
			if (consume(p, '.')) {
				isint = false;
				if (!is_digit(*p))
					return error(h, "invalid number near '%s'", std::string(z, p - z).c_str());
				for (p++; is_digit(*p); p++);
			}
			if (consume(p, "eE")) {
				isint = false;
				if (equal(p, "+-")) p++;
				if (!is_digit(*p))
					return error(h, "invalid number near '%s'", std::string(z, p - z).c_str());
				for (p++; is_digit(*p); p++);
			}
			if (isint)  {
				z = p;
				h.accept_integer(minus ? -i64 : i64);
				return true;
			}
			errno = 0;
			double d = strtod(z, NULL);
			if (errno == ERANGE && (d == HUGE_VAL || d == -HUGE_VAL))
				return error(h, "double too big near '%s'", std::string(z, p - z).c_str());
			z = p;
			h.accept_double(d);
			return true;
		}

		template <class Handler>
		bool parse_string(Handler& h, char del)
		{
			std::vector<char> s;
			expect(z, del);
			const char* p = z;
			for (char ch = *p++; ch != del; ch = *p++)
			{
				switch (ch)
				{
				case '\\':
					switch (*p++)
					{
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
		bool parse_long_string(Handler& h)
		{
			const char* p = z;
			expect(z, '[');
			int sep = 0;
			while (consume(z, '=')) {
				sep++;
			}
			if (!consume(z, '[')) {
				return error(h, "invalid long string delimiter near '%s'", std::string(p, z - p).c_str());
			}
			if (equal(z, "\n\r"))
			{
				incline();
			}
			p = z;
			for (;;)
			{
				switch (*z)
				{
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
				}
						  break;
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
		bool parse_table(Handler& h)
		{
			int ary = 0;
			expect(z, '{');
			h.accept_table(ary);
			for (;;)
			{
				parse_whitespace_and_comments();
				if (consume(z, '}')) {
					return true;
				}
				if (!parse_value<Handler, Convert>(h)) {
					return false;
				}
				parse_whitespace_and_comments();
				if (!consume(z, '=')) {
					h.accept_table_array(ary);
				}
				else {
					parse_whitespace_and_comments();
					if (!parse_value<Handler, Convert>(h)) {
						return false;
					}
					h.accept_table_hash();
				}
				parse_whitespace_and_comments();
				if (consume(z, '}')) {
					return true;
				}
				if (!consume(z, ',')) {
					return error(h, "',' expected near '%c'", *z);
				}
			}
			return true;
		}

		template <class Handler, bool Convert>
		bool parse_value(Handler& h, typename std::enable_if<Convert>::type* = 0)
		{
			switch (*z) {
			case '{':  return parse_table(h);
			case '[':  return parse_long_string(h);
			case '"': case '\'': return parse_string(h, *z);
			case '-':
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': return parse_number(h);
			case '\0':
				return error(h, "unexpected symbol near <eof>");
			default:  {
				const char* p = z;
				for (;;) {
					switch (*z) {
					case '=':
					case ':':
					case ',':
					case ']':
					case '\n':
					case '\r':
					case '\t':
					case '\0':
					case ' ':
						parse_identifier<Handler, Convert>(h, p, z - p);
						return true;
					default:
						z++;
						break;
					}
				}
			}
			}
		}

		template <class Handler, bool Convert>
		bool parse_value(Handler& h, typename std::enable_if<!Convert>::type* = 0)
		{
			switch (*z) {
			case '{':  return parse_table(h);
			case '[':  return parse_long_string(h);
			case '"': case '\'': return parse_string(h, *z);
			case '\0':
				return error(h, "unexpected symbol near <eof>");
			default: {
				const char* p = z;
				for (;;) {
					switch (*z) {
					case '=':
					case ':':
					case ',':
					case ']':
					case '\n':
					case '\r':
					case '\t':
					case '\0':
					case ' ':
						parse_identifier<Handler, Convert>(h, p, z - p);
						return true;
					default:
						z++;
						break;
					}
				}
			}
			}
		}

		template <class Handler, bool Convert>
		bool parse_value_ext(Handler& h, typename std::enable_if<Convert>::type* = 0)
		{
			switch (*z) {
			case '{':  return parse_table(h);
			case '[':  return parse_long_string(h);
			case '"': case '\'': return parse_string(h, *z);
			case '-':
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': return parse_number(h);
			case '\0':
				return error(h, "unexpected symbol near <eof>");
			default:  {
				const char* p = z;
				for (;;) {
					switch (*z) {
					case '\n':
					case '\r':
					case '\0':
						parse_identifier<Handler, Convert>(h, p, z - p);
						return true;
					case '-':
						if (z[1] == '-') {
							parse_identifier<Handler, Convert>(h, p, z - p);
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

		template <class Handler, bool Convert>
		bool parse_value_ext(Handler& h, typename std::enable_if<!Convert>::type* = 0)
		{
			switch (*z) {
			case '{':  return parse_table(h);
			case '[':  return parse_long_string(h);
			case '"': case '\'': return parse_string(h, *z);
			case '\0':
				return error(h, "unexpected symbol near <eof>");
			default: {
				const char* p = z;
				for (;;) {
					switch (*z) {
					case '\n':
					case '\r':
					case '\0':
						parse_identifier<Handler, Convert>(h, p, z - p);
						return true;
					case '-':
						if (z[1] == '-') {
							parse_identifier<Handler, Convert>(h, p, z - p);
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
		bool parse_keyword(Handler& h, const char* str, size_t len)
		{
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

		template <class Handler, bool Convert>
		void parse_identifier(Handler& h, const char* str, size_t len, typename std::enable_if<Convert>::type* = 0)
		{
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
				if (h.accept_identifier(beg, end - beg + 1))
					return;
			}
			h.accept_string(beg, end - beg + 1);
		}

		template <class Handler, bool Convert>
		void parse_identifier(Handler& h, const char* str, size_t len, typename std::enable_if<!Convert>::type* = 0)
		{
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
				if (h.accept_identifier(beg, end - beg + 1))
					return;
			}
			h.accept_string(beg, end - beg + 1);
		}

		template <class Handler>
		bool parse_key(Handler& h)
		{
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
				switch (*z)	 {
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
		bool parse_section_key(Handler& h)
		{
			switch (*z) {
			case '"': case '\'': return parse_string(h, *z);
			case '\0':
				return error(h, "unexpected symbol near <eof>");
			default:
				break;
			}
			const char* p = z;
			for (;;) {
				switch (*z)	 {
				case ':':
				case '.':
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
		bool parse_section(Handler& h)
		{
			int mode = 0;
			expect(z, '[');
			if (consume(z, '[')) {
				mode = 1;
			}
			parse_whitespace();
			bool has_root = false;
			if (!equal(z, '.')) {
				if (!parse_section_key(h)) {
					return false;
				}
				has_root = true;
			}
			if (h.accept_section(has_root)) {
				bool top = true;
				for (;;) {
					if (consume(z, '.')) {
						if (!parse_section_key(h)) {
							return false;
						}
						h.accept_section_child();
						top = false;
						continue;
					}
					if (consume(z, '[')) {
						if (!consume(z, ']')) {
							return error(h, "']' expected near '%c'", *z);
						}
						h.accept_section_array();
						top = false;
						continue;
					}
					break;
				}
				if (mode == 1) {
					h.accept_section_newarray();
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
						if (consume(z, '[')) {
							if (!consume(z, ']')) {
								return error(h, "']' expected near '%c'", *z);
							}
							h.accept_section_array();
							continue;
						}
						break;
					}
					inherited = true;
				}
				h.accept_section_end(inherited, top && mode == 0);
			}
			parse_whitespace();
			if (!consume(z, ']') || ((mode == 1) && (!consume(z, ']')))) {
				return error(h, "']' expected near '%c'", *z);
			}
			return true;
		}

		template <class Handler>
		bool parse_set(Handler& h)
		{
			if (!parse_key(h)) {
				return false;
			}
			parse_whitespace_and_comments();
			if (!consume(z, '=')) {
				return error(h, "'=' expected near '%c'", *z);
			}
			parse_whitespace_and_comments();
			if (!parse_value_ext<Handler, Convert>(h)) {
				return false;
			}
			h.accept_set();
			return true;
		}

		template <class Handler>
		bool parse_object(Handler& h)
		{
			if (!equal(z, '[')) {
				return error(h, "'[' expected near '%c'", *z);
			}
			if (!parse_section(h)) {
				return false;
			}
			parse_whitespace_and_comments_or_execute(h);
			while (!equal(z, "[\0")) {
				if (!parse_set(h)) {
					return false;
				}
				parse_whitespace_and_comments_or_execute(h);
			}
			h.accept_object();
			return true;
		}

		template <class Handler>
		bool parse(Handler& h)
		{
			h.accept_root();
			parse_whitespace_and_comments_or_execute(h);
			while (!equal(z, '\0')) {
				if (!parse_object(h)) {
					return false;
				}
				parse_whitespace_and_comments_or_execute(h);
			}
			h.accept_root_end();
			return true;
		}
	};

	inline void lua_copytable(lua_State* L, int src, int dst)
	{
		src = lua_absindex(L, src);
		dst = lua_absindex(L, dst);
		lua_pushnil(L);
		while (lua_next(L, src)) {
			lua_pushvalue(L, -2);
			lua_insert(L, -2);
			lua_rawset(L, dst);
		}
	}

	inline void lua_cleartable(lua_State* L, int t)
	{
		t = lua_absindex(L, t);
		lua_pushnil(L);
		while (lua_next(L, t)) {
			lua_pop(L, 1);
			lua_pushvalue(L, -1);
			lua_pushnil(L);
			lua_rawset(L, t);
		}
	}

	struct simple_handler
	{
		lua_State* L;

		simple_handler(lua_State* L)
			: L(L)
		{}

		void accept_boolean(bool b) {
			lua_pushboolean(L, b ? 1 : 0);
		}
		void accept_nil() {
			lua_pushnil(L);
		}
		void accept_integer(int64_t i) {
			lua_pushinteger(L, i);
		}
		void accept_double(double d) {
			lua_pushnumber(L, d);
		}
		void accept_string(const char* str, size_t len) {
			lua_pushlstring(L, str, len);
		}
		void accept_table(int& ary) {
			lua_newtable(L);
			ary = 1;
		}
		void accept_table_hash() {
			lua_rawset(L, -3);
		}
		void accept_table_array(int& ary) {
			lua_rawseti(L, -2, ary++);
		}
		void accept_set() {
			lua_settable(L, -3);
		}
		void accept_object() {
			lua_pop(L, 1);
		}
		void accept_root() {
			lua_newtable(L);
			lua_pushvalue(L, -1);
		}
		void accept_root_end() {
			lua_pop(L, 1);
		}
		bool accept_identifier(const char* str, size_t len) {
			return false;
		}
		template <class ... Args>
		void accept_error(int line, const char* fmt, const Args& ... args) {
			lua_pushinteger(L, line);
			lua_pushfstring(L, fmt, args...);
		}
	};

	struct handler
		: public simple_handler
	{
		int t_main = 0;
		int t_default = 0;
		int t_enum = 0;
		int t_env = 0;

		handler(lua_State* L)
			: simple_handler(L)
		{}
		void accept_root() {
			// env
			// main
			// default
			// enum
			t_enum = lua_gettop(L);
			t_default = t_enum - 1;
			t_main = t_default - 1;
			t_env = t_main - 1;
			lua_pushvalue(L, t_main);
		}
		bool accept_section(bool has_root) {
			if (!has_root) {
				lua_pushvalue(L, -1);
				return true;
			}
			lua_remove(L, -2);
			const char* name = luaL_checkstring(L, -1);
			if (0 == strcmp(name, "default")) {
				lua_pop(L, 1);
				lua_pushvalue(L, t_default);
				lua_pushvalue(L, -1);
				return false;
			}
			if (0 == strcmp(name, "enum")) {
				lua_pop(L, 1);
				lua_pushvalue(L, t_enum);
				lua_pushvalue(L, -1);
				return false;
			}
			if (0 == strcmp(name, "root")) {
				lua_pop(L, 1);
				lua_pushvalue(L, t_main);
				lua_pushvalue(L, -1);
				return false;
			}
			lua_pushvalue(L, -1);
			if (lua_gettable(L, t_main) != LUA_TTABLE) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_insert(L, -3);
				lua_settable(L, t_main);
			}
			else {
				lua_remove(L, -2);
			}
			lua_pushvalue(L, -1);
			return true;
		}
		void accept_section_array() {
			lua_Integer n = luaL_len(L, -3);
			if (n == 0) n = 1;
			if (LUA_TTABLE != lua_geti(L, -1, n)) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_seti(L, -3, n);
			}
			lua_remove(L, -2);
		}
		void accept_section_newarray() {
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_seti(L, -3, luaL_len(L, -3) + 1);
			lua_remove(L, -2);
		}
		void accept_section_child()
		{
			lua_pushvalue(L, -1);
			if (lua_gettable(L, -3) != LUA_TTABLE) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_insert(L, -3);
				lua_settable(L, -4);
			}
			else {
				lua_remove(L, -2);
			}
			lua_remove(L, -2);
		}
		void accept_section_inherited() {
			lua_pushvalue(L, -1);
			if (LUA_TTABLE != lua_rawget(L, t_main)) {
				lua_pop(L, 1);
				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_insert(L, -3);
				lua_settable(L, -4);
			}
			lua_remove(L, -2);
		}
		void accept_section_end(bool inherited, bool top) {
			if (inherited) {
				lua_cleartable(L, -2);
				lua_copytable(L, -1, -2);
				lua_pop(L, 1);
				return;
			}
			else {
				lua_cleartable(L, -1);
				if (top) {
					lua_copytable(L, t_default, -1);
				}
			}
		}
		bool accept_identifier(const char* str, size_t len) {
			lua_pushlstring(L, str, len);
			if (LUA_TNIL != lua_rawget(L, t_enum)) {
				return true;
			}
			lua_pop(L, 1);
			return false;
		}
		void accept_execute(const char* start, const char* end) {
			if (luaL_loadbuffer(L, start, end - start, "=(lni-exec)")) {
				lua_error(L);
				return;
			}
			lua_pushvalue(L, t_env);
			if (!lua_setupvalue(L, -2, 1)) {
				lua_pop(L, 1);
			}
			if (lua_pcall(L, 0, 0, 0)) {
				lua_error(L);
				return;
			}
		}
	};
}
