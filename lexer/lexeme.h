#ifndef COMPILER_LEXEME_HEADER
#define COMPILER_LEXEME_HEADER

#include <string>
#include <variant>
#include <vector>

enum LexemeType {
    eof,
    Identifier,
    Integer,
    Double,
    Keyword,
    Operator,
    Separator,
    String
};

enum AllKeywords {
    AND,
    ARRAY,
    ASM,
    BEGIN,
    BREAK,
    CASE,
    CONST,
    CONSTRUCTOR,
    CONTINUE,
    DESTRUCTOR,
    DIV,
    DO,
    DOWNTO,
    ELSE,
    END,
    FALSE,
    // FILE,
    FOR,
    FUNCTION,
    GOTO,
    IF,
    IMPLEMENTATION,
    IN,
    INLINE,
    INTERFACE,
    LABEL,
    MOD,
    NIL,
    NOT,
    OBJECT,
    OF,
    ON,
    OPERATOR,
    OR,
    PACKED,
    PROCEDURE,
    PROGRAM,
    RECORD,
    REPEAT,
    SET,
    SHL,
    SHR,
    STRING,
    THEN,
    TO,
    TRUE,
    TYPE,
    UNIT,
    UNTIL,
    USES,
    VAR,
    WHILE,
    WITH,
    XOR,
    AS,
    CLASS,
    CONSTREF,
    DISPOSE,
    EXCEPT,
    EXIT,
    EXPORTS,
    FINALIZATION,
    FINALLY,
    INHERITED,
    INITIALIZATION,
    IS,
    LIBRARY,
    NEW,
    OUT,
    PROPERTY,
    RAISE,
    SELF,
    THREADVAR,
    TRY,
    ABSOLUTE,
    ABSTRACT,
    ALIAS,
    ASSEMBLER,
    CDECL,
    CPPDECL,
    DEFAULT,
    EXPORT,
    EXTERNAL,
    FORWARD,
    GENERIC,
    INDEX,
    LOCAL,
    NAME,
    NOSTACKFRAME,
    OLDFPCCALL,
    OVERRIDE,
    PASCAL,
    PRIVATE,
    PROTECTED,
    PUBLIC,
    PUBLISHED,
    READ,
    READLN,
    REGISTER,
    REINTRODUCE,
    SAFECALL,
    SOFTFLOAT,
    SPECIALIZE,
    STDCALL,
    VIRTUAL,
    WRITE,
    WRITELN
};

enum Operators {
    EQUAL,
    UNEQUAL,
    LESSEQUAL,
    LESS,
    GREATEREQUAL,
    SYMDIFF,
    GREATER,
    ADDASSIGN,
    ADD,
    SUBSTRACTASSIGN,
    SUBSTRACT,
    MULTIPLYASSIGN,
    MULTIPLY,
    DIVISIONASSIGN,
    DIVISION,
    AT,
    CIRCUMFLEX,
    ASSIGN
};

enum Separators {
    DOUBLEPERIOD,
    PERIOD,
    COMMA,
    SEMICOLON,
    COLON,
    LPARENTHESIS,
    RPARENTHESIS,
    RSBRACKET,
    LSBRACKET
};

typedef std::variant<int, double, std::string, AllKeywords, Operators,
        Separators>
        LexemeValue;

class Position {
    int line;
    int column;

public:
    Position(int line, int column);

    [[nodiscard]] int GetLine() const;

    [[nodiscard]] int GetColumn() const;

    void Set(int line, int column);

    void Set(const Position &pos);
};

class Lexeme {
    Position position;
    LexemeType type;
    LexemeValue value;
    std::string rawLexeme;

public:
    Lexeme(const Position &position, LexemeType type, LexemeValue &value,
           const std::string &rawLexeme);

    LexemeType GetType();

    friend std::ostream &operator<<(std::ostream &os, const Lexeme &lexeme);

    std::string String();

    template<typename T>
    T GetValue() { return std::get<T>(value); }

    Position GetPos() { return position; }

    std::string GetRaw() { return rawLexeme; }

    friend bool operator==(Lexeme &lex, LexemeType type);

    friend bool operator==(Lexeme &lex, Operators op);

    friend bool operator==(Lexeme &lex, Separators sep);

    friend bool operator==(Lexeme &lex, AllKeywords keyword);
};

#endif