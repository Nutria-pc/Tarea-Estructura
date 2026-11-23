#ifndef SIMPLE_JSON_HPP
#define SIMPLE_JSON_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cctype>
#include <iomanip>

namespace sjson {

enum class Type { Null, Bool, Number, String, Array, Object };

class Value {
public:
    using ArrayType = std::vector<Value>;
    using ObjectType = std::map<std::string, Value>;

private:
    Type type_ = Type::Null;
    bool bool_val_ = false;
    double num_val_ = 0.0;
    std::string str_val_;
    ArrayType arr_val_;
    ObjectType obj_val_;

public:
    Value() : type_(Type::Null) {}
    Value(bool b) : type_(Type::Bool), bool_val_(b) {}
    Value(int n) : type_(Type::Number), num_val_(static_cast<double>(n)) {}
    Value(double n) : type_(Type::Number), num_val_(n) {}
    Value(const char* s) : type_(Type::String), str_val_(s) {}
    Value(const std::string& s) : type_(Type::String), str_val_(s) {}
    Value(const ArrayType& a) : type_(Type::Array), arr_val_(a) {}
    Value(const ObjectType& o) : type_(Type::Object), obj_val_(o) {}

    static Value makeArray() { return Value(ArrayType{}); }
    static Value makeObject() { return Value(ObjectType{}); }

    Type type() const { return type_; }
    bool isNull() const { return type_ == Type::Null; }
    bool isBool() const { return type_ == Type::Bool; }
    bool isNumber() const { return type_ == Type::Number; }
    bool isString() const { return type_ == Type::String; }
    bool isArray() const { return type_ == Type::Array; }
    bool isObject() const { return type_ == Type::Object; }

    bool asBool() const { return bool_val_; }
    double asDouble() const { return num_val_; }
    int asInt() const { return static_cast<int>(num_val_); }
    const std::string& asString() const { return str_val_; }
    ArrayType& asArray() { return arr_val_; }
    const ArrayType& asArray() const { return arr_val_; }
    ObjectType& asObject() { return obj_val_; }
    const ObjectType& asObject() const { return obj_val_; }

    void push_back(const Value& v) {
        if (type_ != Type::Array) {
            type_ = Type::Array;
            arr_val_.clear();
        }
        arr_val_.push_back(v);
    }

    Value& operator[](const std::string& key) {
        if (type_ != Type::Object) {
            type_ = Type::Object;
            obj_val_.clear();
        }
        return obj_val_[key];
    }

    const Value& at(const std::string& key) const {
        if (type_ != Type::Object) throw std::runtime_error("Not a JSON object");
        auto it = obj_val_.find(key);
        if (it == obj_val_.end()) throw std::runtime_error("Key not found: " + key);
        return it->second;
    }

    bool hasKey(const std::string& key) const {
        return type_ == Type::Object && obj_val_.find(key) != obj_val_.end();
    }

    static std::string escapeString(const std::string& s) {
        std::ostringstream oss;
        for (unsigned char c : s) {
            switch (c) {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    oss << c;
                    break;
            }
        }
        return oss.str();
    }

    std::string dump(int indent = 2, int current_indent = 0) const {
        std::string pad(current_indent, ' ');
        std::string next_pad(current_indent + indent, ' ');
        std::ostringstream oss;

        switch (type_) {
            case Type::Null:
                oss << "null";
                break;
            case Type::Bool:
                oss << (bool_val_ ? "true" : "false");
                break;
            case Type::Number: {
                if (num_val_ == static_cast<long long>(num_val_)) {
                    oss << static_cast<long long>(num_val_);
                } else {
                    oss << std::fixed << std::setprecision(2) << num_val_;
                }
                break;
            }
            case Type::String:
                oss << "\"" << escapeString(str_val_) << "\"";
                break;
            case Type::Array: {
                if (arr_val_.empty()) {
                    oss << "[]";
                } else {
                    oss << "[\n";
                    for (size_t i = 0; i < arr_val_.size(); ++i) {
                        oss << next_pad << arr_val_[i].dump(indent, current_indent + indent);
                        if (i + 1 < arr_val_.size()) oss << ",";
                        oss << "\n";
                    }
                    oss << pad << "]";
                }
                break;
            }
            case Type::Object: {
                if (obj_val_.empty()) {
                    oss << "{}";
                } else {
                    oss << "{\n";
                    size_t count = 0;
                    for (const auto& kv : obj_val_) {
                        oss << next_pad << "\"" << escapeString(kv.first) << "\": "
                            << kv.second.dump(indent, current_indent + indent);
                        if (++count < obj_val_.size()) oss << ",";
                        oss << "\n";
                    }
                    oss << pad << "}";
                }
                break;
            }
        }
        return oss.str();
    }
};

class Parser {
    const std::string& src_;
    size_t pos_ = 0;

    void skipWhitespace() {
        while (pos_ < src_.size() && std::isspace(static_cast<unsigned char>(src_[pos_]))) {
            ++pos_;
        }
    }

    char peek() {
        skipWhitespace();
        if (pos_ >= src_.size()) throw std::runtime_error("Unexpected end of JSON");
        return src_[pos_];
    }

    char get() {
        skipWhitespace();
        if (pos_ >= src_.size()) throw std::runtime_error("Unexpected end of JSON");
        return src_[pos_++];
    }

    std::string parseStringRaw() {
        if (get() != '"') throw std::runtime_error("Expected '\"'");
        std::string res;
        while (pos_ < src_.size()) {
            char c = src_[pos_++];
            if (c == '"') return res;
            if (c == '\\') {
                if (pos_ >= src_.size()) throw std::runtime_error("Invalid escape");
                char e = src_[pos_++];
                switch (e) {
                    case '"': res.push_back('"'); break;
                    case '\\': res.push_back('\\'); break;
                    case '/': res.push_back('/'); break;
                    case 'b': res.push_back('\b'); break;
                    case 'f': res.push_back('\f'); break;
                    case 'n': res.push_back('\n'); break;
                    case 'r': res.push_back('\r'); break;
                    case 't': res.push_back('\t'); break;
                    default: res.push_back(e); break;
                }
            } else {
                res.push_back(c);
            }
        }
        throw std::runtime_error("Unterminated string in JSON");
    }

    Value parseNumber() {
        skipWhitespace();
        size_t start = pos_;
        if (src_[pos_] == '-') ++pos_;
        while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) ++pos_;
        if (pos_ < src_.size() && src_[pos_] == '.') {
            ++pos_;
            while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) ++pos_;
        }
        if (pos_ < src_.size() && (src_[pos_] == 'e' || src_[pos_] == 'E')) {
            ++pos_;
            if (pos_ < src_.size() && (src_[pos_] == '+' || src_[pos_] == '-')) ++pos_;
            while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) ++pos_;
        }
        double val = std::stod(src_.substr(start, pos_ - start));
        return Value(val);
    }

    Value parseArray() {
        get(); // '['
        Value arr = Value::makeArray();
        if (peek() == ']') {
            get();
            return arr;
        }
        while (true) {
            arr.push_back(parseValue());
            char c = get();
            if (c == ']') break;
            if (c != ',') throw std::runtime_error("Expected ',' or ']' in JSON array");
        }
        return arr;
    }

    Value parseObject() {
        get(); // '{'
        Value obj = Value::makeObject();
        if (peek() == '}') {
            get();
            return obj;
        }
        while (true) {
            std::string key = parseStringRaw();
            if (get() != ':') throw std::runtime_error("Expected ':' in JSON object");
            obj[key] = parseValue();
            char c = get();
            if (c == '}') break;
            if (c != ',') throw std::runtime_error("Expected ',' or '}' in JSON object");
        }
        return obj;
    }

public:
    explicit Parser(const std::string& src) : src_(src) {}

    Value parseValue() {
        char c = peek();
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return Value(parseStringRaw());
        if (c == 't') {
            if (src_.substr(pos_, 4) == "true") { pos_ += 4; return Value(true); }
            throw std::runtime_error("Invalid literal");
        }
        if (c == 'f') {
            if (src_.substr(pos_, 5) == "false") { pos_ += 5; return Value(false); }
            throw std::runtime_error("Invalid literal");
        }
        if (c == 'n') {
            if (src_.substr(pos_, 4) == "null") { pos_ += 4; return Value(); }
            throw std::runtime_error("Invalid literal");
        }
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
            return parseNumber();
        }
        throw std::runtime_error(std::string("Unexpected character in JSON: ") + c);
    }
};

inline Value parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + filepath);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    // Remove UTF-8 BOM if present
    if (content.size() >= 3 &&
        static_cast<unsigned char>(content[0]) == 0xEF &&
        static_cast<unsigned char>(content[1]) == 0xBB &&
        static_cast<unsigned char>(content[2]) == 0xBF) {
        content = content.substr(3);
    }
    Parser p(content);
    return p.parseValue();
}

inline void saveFile(const std::string& filepath, const Value& val) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo escribir en el archivo: " + filepath);
    }
    file << val.dump(2) << "\n";
}

} // namespace sjson

#endif // SIMPLE_JSON_HPP
