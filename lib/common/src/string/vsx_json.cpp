/* Copyright (c) 2013 Dropbox, Inc.
 * Copyright (c) 2016 Vovoid Media Technologies AB
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string/vsx_json.h>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <limits>

namespace vsx
{

static const int max_depth = 200;

/* * * * * * * * * * * * * * * * * * * *
 * Serialization
 */

static void dump(std::nullptr_t, std::string &out) {
    out += "null";
}

static void dump(double value, std::string &out) {
    if (std::isfinite(value)) {
        char buf[32];
        snprintf(buf, sizeof buf, "%.17g", value);
        out += buf;
    } else {
        out += "null";
    }
}

static void dump(int value, std::string &out) {
    char buf[32];
    snprintf(buf, sizeof buf, "%d", value);
    out += buf;
}

static void dump(bool value, std::string &out) {
    out += value ? "true" : "false";
}

static void dump(const std::string &value, std::string &out) {
    out += '"';
    for (size_t i = 0; i < value.length(); i++) {
        const char ch = value[i];
        if (ch == '\\') {
            out += "\\\\";
        } else if (ch == '"') {
            out += "\\\"";
        } else if (ch == '\b') {
            out += "\\b";
        } else if (ch == '\f') {
            out += "\\f";
        } else if (ch == '\n') {
            out += "\\n";
        } else if (ch == '\r') {
            out += "\\r";
        } else if (ch == '\t') {
            out += "\\t";
        } else if (static_cast<uint8_t>(ch) <= 0x1f) {
            char buf[8];
            snprintf(buf, sizeof buf, "\\u%04x", ch);
            out += buf;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa8) {
            out += "\\u2028";
            i += 2;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa9) {
            out += "\\u2029";
            i += 2;
        } else {
            out += ch;
        }
    }
    out += '"';
}

static void dump(const json::array &values, std::string &out) {
    bool first = true;
    out += "[";
    for (const auto &value : values) {
        if (!first)
            out += ", ";
        value.dump(out);
        first = false;
    }
    out += "]";
}

static void dump(const json::object &values, std::string &out) {
    bool first = true;
    out += "{";
    for (const auto &kv : values) {
        if (!first)
            out += ", ";
        dump(kv.first, out);
        out += ": ";
        kv.second.dump(out);
        first = false;
    }
    out += "}";
}

void json::dump(std::string &out) const {
    m_ptr->dump(out);
}

/* * * * * * * * * * * * * * * * * * * *
 * Value wrappers
 */

template <json::type_t tag, typename T>
class value : public json_value {
protected:

    // Constructors
    explicit value(const T &value_) : m_value(value_) {}
    explicit value(T &&value_)      : m_value(std::move(value_)) {}

    // Get type tag
    json::type_t type() const override {
        return tag;
    }

    // Comparisons
    bool equals(const json_value * other) const override {
      return m_value == static_cast<const value<tag, T> *>(other)->m_value;
    }
    bool less(const json_value * other) const override {
        return m_value < static_cast<const value<tag, T> *>(other)->m_value;
    }

    const T m_value;
    void dump(std::string &out) const override { vsx::dump(m_value, out); }
};

class json_double final : public value<json::NUMBER, double> {
    double number_value() const override { return m_value; }
    int int_value() const override { return static_cast<int>(m_value); }
    bool equals(const json_value * other) const override { return fabs(m_value - other->number_value()) < std::numeric_limits<double>::epsilon(); }
    bool less(const json_value * other)   const override { return m_value <  other->number_value(); }
public:
    explicit json_double(double value_) : value(value_) {}
};

class json_int final : public value<json::NUMBER, int> {
    double number_value() const override { return m_value; }
    int int_value() const override { return m_value; }
    bool equals(const json_value * other) const override { return fabs( (double)m_value - other->number_value() ) < std::numeric_limits<double>::epsilon(); }
    bool less(const json_value * other)   const override { return m_value <  other->number_value(); }
public:
    explicit json_int(int value_) : value(value_) {}
};

class json_boolean final : public value<json::BOOL, bool> {
    bool bool_value() const override { return m_value; }
public:
    explicit json_boolean(bool value_) : value(value_) {}
};

class json_string final : public value<json::STRING, std::string> {
    const std::string &string_value() const override { return m_value; }
public:
    explicit json_string(const std::string &value_) : value(value_) {}
    explicit json_string(std::string &&value_)      : value(move(value_)) {}
};

class json_array final : public value<json::ARRAY, json::array> {
    const json::array &array_items() const override { return m_value; }
    const json & operator[](size_t i) const override;
public:
    explicit json_array(const json::array &value_) : value(value_) {}
    explicit json_array(json::array &&value_)      : value(move(value_)) {}
};

class json_object final : public value<json::OBJECT, json::object> {
    const json::object &object_items() const override { return m_value; }
    const json & operator[](const std::string &key) const override;
public:
    explicit json_object(const json::object &value_) : value(value_) {}
    explicit json_object(json::object &&value_)      : value(move(value_)) {}
};

class json_null final : public value<json::NUL, std::nullptr_t> {
public:
    json_null() : value(nullptr) {}
};

/* * * * * * * * * * * * * * * * * * * *
 * Static globals - static-init-safe
 */
struct statics_t {
    const std::shared_ptr<json_value> null = std::make_shared<json_null>();
    const std::shared_ptr<json_value> t = std::make_shared<json_boolean>(true);
    const std::shared_ptr<json_value> f = std::make_shared<json_boolean>(false);
    const std::string empty_string;
    const std::vector<json> empty_vector;
    const std::map<std::string, json> empty_map;
    statics_t() {}
};

static const statics_t & statics() {
    static const statics_t s {};
    return s;
}

static const json & static_null() {
    // This has to be separate, not in Statics, because Json() accesses statics().null.
    static const json json_null;
    return json_null;
}

/* * * * * * * * * * * * * * * * * * * *
 * Constructors
 */

json::json() noexcept                  : m_ptr(statics().null) {}
json::json(std::nullptr_t) noexcept    : m_ptr(statics().null) {}
json::json(double value)               : m_ptr(std::make_shared<json_double>(value)) {}
json::json(int value)                  : m_ptr(std::make_shared<json_int>(value)) {}
json::json(bool value)                 : m_ptr(value ? statics().t : statics().f) {}
json::json(const std::string &value)        : m_ptr(std::make_shared<json_string>(value)) {}
json::json(std::string &&value)             : m_ptr(std::make_shared<json_string>(move(value))) {}
json::json(const char * value)         : m_ptr(std::make_shared<json_string>(value)) {}
json::json(const json::array &values)  : m_ptr(std::make_shared<json_array>(values)) {}
json::json(json::array &&values)       : m_ptr(std::make_shared<json_array>(move(values))) {}
json::json(const json::object &values) : m_ptr(std::make_shared<json_object>(values)) {}
json::json(json::object &&values)      : m_ptr(std::make_shared<json_object>(move(values))) {}

/* * * * * * * * * * * * * * * * * * * *
 * Accessors
 */

json::type_t json::type()                           const { return m_ptr->type();         }
double json::number_value()                       const { return m_ptr->number_value(); }
int json::int_value()                             const { return m_ptr->int_value();    }
bool json::bool_value()                           const { return m_ptr->bool_value();   }
const std::string & json::string_value()               const { return m_ptr->string_value(); }
const std::vector<json> & json::array_items()          const { return m_ptr->array_items();  }
const std::map<std::string, json> & json::object_items()    const { return m_ptr->object_items(); }
const json & json::operator[] (size_t i)          const { return (*m_ptr)[i];           }
const json & json::operator[] (const std::string &key) const { return (*m_ptr)[key];         }

double                    json_value::number_value()              const { return 0; }
int                       json_value::int_value()                 const { return 0; }
bool                      json_value::bool_value()                const { return false; }
const std::string &            json_value::string_value()              const { return statics().empty_string; }
const std::vector<json> &      json_value::array_items()               const { return statics().empty_vector; }
const std::map<std::string, json> & json_value::object_items()              const { return statics().empty_map; }
const json &              json_value::operator[] (size_t)         const { return static_null(); }
const json &              json_value::operator[] (const std::string &) const { return static_null(); }

const json & json_object::operator[] (const std::string &key) const {
    auto iter = m_value.find(key);
    return (iter == m_value.end()) ? static_null() : iter->second;
}
const json & json_array::operator[] (size_t i) const {
    if (i >= m_value.size()) return static_null();
    else return m_value[i];
}

/* * * * * * * * * * * * * * * * * * * *
 * Comparison
 */

bool json::operator== (const json &other) const {
    if (m_ptr->type() != other.m_ptr->type())
        return false;

    return m_ptr->equals(other.m_ptr.get());
}

bool json::operator< (const json &other) const {
    if (m_ptr->type() != other.m_ptr->type())
        return m_ptr->type() < other.m_ptr->type();

    return m_ptr->less(other.m_ptr.get());
}

/* * * * * * * * * * * * * * * * * * * *
 * Parsing
 */

/* esc(c)
 *
 * Format char c suitable for printing in an error message.
 */
static inline std::string esc(char c) {
    char buf[12];
    if (static_cast<uint8_t>(c) >= 0x20 && static_cast<uint8_t>(c) <= 0x7f) {
        snprintf(buf, sizeof buf, "'%c' (%d)", c, c);
    } else {
        snprintf(buf, sizeof buf, "(%d)", c);
    }
    return std::string(buf);
}

static inline bool in_range(long x, long lower, long upper) {
    return (x >= lower && x <= upper);
}

namespace {
/* Json parser
 *
 * Object that tracks all state of an in-progress parse.
 */
struct json_parser final {

    /* State
     */
    const std::string &str;
    size_t i;
    std::string &err;
    bool failed;
    const json_parse strategy;

    /* fail(msg, err_ret = Json())
     *
     * Mark this parse as failed.
     */
    json fail(std::string &&msg) {
        return fail(move(msg), json());
    }

    template <typename T>
    T fail(std::string &&msg, const T err_ret) {
        if (!failed)
            err = std::move(msg);
        failed = true;
        return err_ret;
    }

    /* consume_whitespace()
     *
     * Advance until the current character is non-whitespace.
     */
    void consume_whitespace() {
        while (str[i] == ' ' || str[i] == '\r' || str[i] == '\n' || str[i] == '\t')
            i++;
    }

    /* consume_comment()
     *
     * Advance comments (c-style inline and multiline).
     */
    bool consume_comment() {
      bool comment_found = false;
      if (str[i] == '/') {
        i++;
        if (i == str.size())
          return fail("unexpected end of input inside comment", false);
        if (str[i] == '/') { // inline comment
          i++;
          if (i == str.size())
            return fail("unexpected end of input inside inline comment", false);
          // advance until next line
          while (str[i] != '\n') {
            i++;
            if (i == str.size())
              return fail("unexpected end of input inside inline comment", false);
          }
          comment_found = true;
        }
        else if (str[i] == '*') { // multiline comment
          i++;
          if (i > str.size()-2)
            return fail("unexpected end of input inside multi-line comment", false);
          // advance until closing tokens
          while (!(str[i] == '*' && str[i+1] == '/')) {
            i++;
            if (i > str.size()-2)
              return fail(
                "unexpected end of input inside multi-line comment", false);
          }
          i += 2;
          if (i == str.size())
            return fail(
              "unexpected end of input inside multi-line comment", false);
          comment_found = true;
        }
        else
          return fail("malformed comment", false);
      }
      return comment_found;
    }

    /* consume_garbage()
     *
     * Advance until the current character is non-whitespace and non-comment.
     */
    void consume_garbage() {
      consume_whitespace();
      if(strategy == json_parse::COMMENTS) {
        bool comment_found = false;
        do {
          comment_found = consume_comment();
          consume_whitespace();
        }
        while(comment_found);
      }
    }

    /* get_next_token()
     *
     * Return the next non-whitespace character. If the end of the input is reached,
     * flag an error and return 0.
     */
    char get_next_token() {
        consume_garbage();
        if (i == str.size())
            return fail("unexpected end of input", (char)0);

        return str[i++];
    }

    /* encode_utf8(pt, out)
     *
     * Encode pt as UTF-8 and add it to out.
     */
    void encode_utf8(long pt, std::string & out) {
        if (pt < 0)
            return;

        if (pt < 0x80) {
            out += static_cast<char>(pt);
        } else if (pt < 0x800) {
            out += static_cast<char>((pt >> 6) | 0xC0);
            out += static_cast<char>((pt & 0x3F) | 0x80);
        } else if (pt < 0x10000) {
            out += static_cast<char>((pt >> 12) | 0xE0);
            out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
            out += static_cast<char>((pt & 0x3F) | 0x80);
        } else {
            out += static_cast<char>((pt >> 18) | 0xF0);
            out += static_cast<char>(((pt >> 12) & 0x3F) | 0x80);
            out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
            out += static_cast<char>((pt & 0x3F) | 0x80);
        }
    }

    /* parse_string()
     *
     * Parse a string, starting at the current position.
     */
    std::string parse_string() {
        std::string out;
        long last_escaped_codepoint = -1;
        while (true) {
            if (i == str.size())
                return fail("unexpected end of input in string", "");

            char ch = str[i++];

            if (ch == '"') {
                encode_utf8(last_escaped_codepoint, out);
                return out;
            }

            if (in_range(ch, 0, 0x1f))
                return fail("unescaped " + esc(ch) + " in string", "");

            // The usual case: non-escaped characters
            if (ch != '\\') {
                encode_utf8(last_escaped_codepoint, out);
                last_escaped_codepoint = -1;
                out += ch;
                continue;
            }

            // Handle escapes
            if (i == str.size())
                return fail("unexpected end of input in string", "");

            ch = str[i++];

            if (ch == 'u') {
                // Extract 4-byte escape sequence
                std::string esc = str.substr(i, 4);
                // Explicitly check length of the substring. The following loop
                // relies on std::string returning the terminating NUL when
                // accessing str[length]. Checking here reduces brittleness.
                if (esc.length() < 4) {
                    return fail("bad \\u escape: " + esc, "");
                }
                for (int j = 0; j < 4; j++) {
                    if (!in_range(esc[j], 'a', 'f') && !in_range(esc[j], 'A', 'F')
                            && !in_range(esc[j], '0', '9'))
                        return fail("bad \\u escape: " + esc, "");
                }

                long codepoint = strtol(esc.data(), nullptr, 16);

                // JSON specifies that characters outside the BMP shall be encoded as a pair
                // of 4-hex-digit \u escapes encoding their surrogate pair components. Check
                // whether we're in the middle of such a beast: the previous codepoint was an
                // escaped lead (high) surrogate, and this is a trail (low) surrogate.
                if (in_range(last_escaped_codepoint, 0xD800, 0xDBFF)
                        && in_range(codepoint, 0xDC00, 0xDFFF)) {
                    // Reassemble the two surrogate pairs into one astral-plane character, per
                    // the UTF-16 algorithm.
                    encode_utf8((((last_escaped_codepoint - 0xD800) << 10)
                                 | (codepoint - 0xDC00)) + 0x10000, out);
                    last_escaped_codepoint = -1;
                } else {
                    encode_utf8(last_escaped_codepoint, out);
                    last_escaped_codepoint = codepoint;
                }

                i += 4;
                continue;
            }

            encode_utf8(last_escaped_codepoint, out);
            last_escaped_codepoint = -1;

            if (ch == 'b') {
                out += '\b';
            } else if (ch == 'f') {
                out += '\f';
            } else if (ch == 'n') {
                out += '\n';
            } else if (ch == 'r') {
                out += '\r';
            } else if (ch == 't') {
                out += '\t';
            } else if (ch == '"' || ch == '\\' || ch == '/') {
                out += ch;
            } else {
                return fail("invalid escape character " + esc(ch), "");
            }
        }
    }

    /* parse_number()
     *
     * Parse a double.
     */
    json parse_number() {
        size_t start_pos = i;

        if (str[i] == '-')
            i++;

        // Integer part
        if (str[i] == '0') {
            i++;
            if (in_range(str[i], '0', '9'))
                return fail("leading 0s not permitted in numbers");
        } else if (in_range(str[i], '1', '9')) {
            i++;
            while (in_range(str[i], '0', '9'))
                i++;
        } else {
            return fail("invalid " + esc(str[i]) + " in number");
        }

        if (str[i] != '.' && str[i] != 'e' && str[i] != 'E'
                && (i - start_pos) <= static_cast<size_t>(std::numeric_limits<int>::digits10)) {
            return std::atoi(str.c_str() + start_pos);
        }

        // Decimal part
        if (str[i] == '.') {
            i++;
            if (!in_range(str[i], '0', '9'))
                return fail("at least one digit required in fractional part");

            while (in_range(str[i], '0', '9'))
                i++;
        }

        // Exponent part
        if (str[i] == 'e' || str[i] == 'E') {
            i++;

            if (str[i] == '+' || str[i] == '-')
                i++;

            if (!in_range(str[i], '0', '9'))
                return fail("at least one digit required in exponent");

            while (in_range(str[i], '0', '9'))
                i++;
        }

        return std::strtod(str.c_str() + start_pos, nullptr);
    }

    /* expect(str, res)
     *
     * Expect that 'str' starts at the character that was just read. If it does, advance
     * the input and return res. If not, flag an error.
     */
    json expect(const std::string &expected, json res) {
        assert(i != 0);
        i--;
        if (str.compare(i, expected.length(), expected) == 0) {
            i += expected.length();
            return res;
        } else {
            return fail("parse error: expected " + expected + ", got " + str.substr(i, expected.length()));
        }
    }

    /* parse_json()
     *
     * Parse a JSON object.
     */
    json parse_json(int depth) {
        if (depth > max_depth) {
            return fail("exceeded maximum nesting depth");
        }

        char ch = get_next_token();
        if (failed)
            return json();

        if (ch == '-' || (ch >= '0' && ch <= '9')) {
            i--;
            return parse_number();
        }

        if (ch == 't')
            return expect("true", true);

        if (ch == 'f')
            return expect("false", false);

        if (ch == 'n')
            return expect("null", json());

        if (ch == '"')
            return parse_string();

        if (ch == '{') {
            std::map<std::string, json> data;
            ch = get_next_token();
            if (ch == '}')
                return data;

            while (1) {
                if (ch != '"')
                    return fail("expected '\"' in object, got " + esc(ch));

                std::string key = parse_string();
                if (failed)
                    return json();

                ch = get_next_token();
                if (ch != ':')
                    return fail("expected ':' in object, got " + esc(ch));

                data[std::move(key)] = parse_json(depth + 1);
                if (failed)
                    return json();

                ch = get_next_token();
                if (ch == '}')
                    break;
                if (ch != ',')
                    return fail("expected ',' in object, got " + esc(ch));

                ch = get_next_token();
            }
            return data;
        }

        if (ch == '[') {
            std::vector<json> data;
            ch = get_next_token();
            if (ch == ']')
                return data;

            while (1) {
                i--;
                data.push_back(parse_json(depth + 1));
                if (failed)
                    return json();

                ch = get_next_token();
                if (ch == ']')
                    break;
                if (ch != ',')
                    return fail("expected ',' in list, got " + esc(ch));

                ch = get_next_token();
                (void)ch;
            }
            return data;
        }

        return fail("expected value, got " + esc(ch));
    }
};
}//namespace {

json json::parse(const std::string &in, std::string &err, json_parse strategy) {
    json_parser parser { in, 0, err, false, strategy };
    json result = parser.parse_json(0);

    // Check for any trailing garbage
    parser.consume_garbage();
    if (parser.i != in.size())
        return parser.fail("unexpected trailing " + esc(in[parser.i]));

    return result;
}

// Documented in vsx_json.hpp
std::vector<json> json::parse_multi(const std::string &in,
                               std::string::size_type &parser_stop_pos,
                               std::string &err,
                               json_parse strategy) {
    json_parser parser { in, 0, err, false, strategy };
    parser_stop_pos = 0;
    std::vector<json> json_vec;
    while (parser.i != in.size() && !parser.failed) {
        json_vec.push_back(parser.parse_json(0));
        // Check for another object
        parser.consume_garbage();
        if (!parser.failed)
            parser_stop_pos = parser.i;
    }
    return json_vec;
}

/* * * * * * * * * * * * * * * * * * * *
 * Shape-checking
 */

bool json::has_shape(const shape & types, std::string & err) const {
    if (!is_object()) {
        err = "expected JSON object, got " + dump();
        return false;
    }

    for (auto & item : types) {
        if ((*this)[item.first].type() != item.second) {
            err = "bad type for " + item.first + " in " + dump();
            return false;
        }
    }

    return true;
}

} // namespace vsx
