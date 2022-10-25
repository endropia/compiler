#ifndef COMPILER_LEXEME_HEADER
#define COMPILER_LEXEME_HEADER

#include <string>
#include <vector>

enum LexemeType {
    eof,
    Invalid,
    Identifier,
    Integer,
    Real,
    Keyword,
    Operator,
    Separator,
    String
};

std::string to_string(LexemeType type);

[[maybe_unused]] const std::vector<std::string> allKeywords = {
        "string",
        "char",
        "integer",
        "real",
        "and",
        "array",
        "asm",
        "begin",
        "break",
        "case",
        "const",
        "constructor",
        "continue",
        "destructor",
        "div",
        "do",
        "downto",
        "else",
        "end",
        "false",
        "file",
        "for",
        "function",
        "goto",
        "if",
        "implementation",
        "in",
        "inline",
        "interface",
        "label",
        "mod",
        "nil",
        "not",
        "object",
        "of",
        "on",
        "operator",
        "or",
        "packed",
        "procedure",
        "program",
        "record",
        "repeat",
        "set",
        "shl",
        "shr",
        "string",
        "then",
        "to",
        "true",
        "type",
        "unit",
        "until",
        "uses",
        "var",
        "while",
        "with",
        "xor",
        "as",
        "class",
        "constref",
        "dispose",
        "except",
        "exit",
        "exports",
        "finalization",
        "finally",
        "inherited",
        "initialization",
        "is",
        "library",
        "new",
        "on",
        "out",
        "property",
        "raise",
        "self",
        "threadvar",
        "try",
        "absolute",
        "abstract",
        "alias",
        "assembler",
        "cdecl",
        "Cppdecl",
        "default",
        "export",
        "external",
        "forward",
        "generic",
        "index",
        "local",
        "name",
        "nostackframe",
        "oldfpccall",
        "override",
        "pascal",
        "private",
        "protected",
        "public",
        "published",
        "read",
        "readln",
        "register",
        "reintroduce",
        "safecall",
        "softfloat",
        "specialize",
        "stdcall",
        "virtual",
        "write",
        "writeln"};

class Lexeme {
    int line;
    int column;
    LexemeType type;
    std::string value;
    std::string rawLexeme;
public:
    Lexeme(int line, int column, LexemeType type,
           const std::string &value, const std::string &rawLexeme);

    LexemeType GetType();

    friend std::ostream &operator<<(std::ostream &os, const Lexeme &lexeme);
};

#endif