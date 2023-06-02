enum TokenType : uint32 {
    tok_lcb,
    tok_rcb,
    tok_lsb,
    tok_rsb,
    tok_colon,
    tok_comma,
    tok_string,
    tok_number,
    tok_bool,
    tok_null,
    tok_error,
    tok_eof
};

struct Token {
    TokenType type;
    uint32 flags;   // 1-escaping 2-float
    String str;
};

struct Lexer {
    byte* pos;
    int64 cap;
};

inline uint32 GetCodepoint(Lexer* lexer, int32* length) {
    uint32 codepoint;
    *length = utf8_decode(lexer->pos, lexer->cap, &codepoint);
    lexer->pos += *length;
    lexer->cap -= *length;
    return codepoint;
}

inline bool IsWhiteSpace(uint32 cp) {
    return cp == (uint32)' ' || cp == (uint32)'\t' || cp == (uint32)'\n' || cp == (uint32)'\r';
}

inline bool IsDigit(uint32 cp) {
    return (uint32)'0' <= cp && cp <= (uint32)'9';
}

inline bool IsAlpha(uint32 cp) {
    return ((uint32)'a' <= cp && cp <= (uint32)'z');
}

Token GetToken(Lexer* lexer) {
    Token token = {};
    uint32 c;
    
    // Skip any whitespace.
    int32 length;
    do {
        c = GetCodepoint(lexer, &length);
    } while (IsWhiteSpace(c));

    token.str.data = lexer->pos - 1;
    token.str.length = 1;

    if (c == (uint32)':') {
        token.type = tok_colon;
        return token;
    }
    if (c == (uint32)',') {
        token.type = tok_comma;
        return token;
    }
    if (c == (uint32)'{') {
        token.type = tok_lcb;
        return token;
    }
    if (c == (uint32)'}') {
        token.type = tok_rcb;
        return token;
    }
    if (c == (uint32)'[') {
        token.type = tok_lsb;
        return token;
    }
    if (c == (uint32)']') {
        token.type = tok_rsb;
        return token;
    }
    if (c == 0) {
        token.str = STR("eof");
        token.type = tok_eof;
        return token;
    }
    
    if (IsAlpha(c)) {
        while (IsAlpha((c = GetCodepoint(lexer, &length))))
          token.str.length+=length;
    
        if (StringEquals(token.str, STR("true")) || StringEquals(token.str, STR("false")))
            token.type = tok_bool;
        else if (StringEquals(token.str, STR("null")))
            token.type = tok_null;
        else token.type = tok_error;

        lexer->pos--;
        return token;
    }
    
    if (c == (uint32)'"') {
        while (true) {
            c = GetCodepoint(lexer, &length);
            if (c == (uint32)'\\') {
                token.flags |= 1;
                GetCodepoint(lexer, &length);
                token.str.length+=2;
            }
            else {
                if (c == (uint32)'"') break;
                token.str.length+=length;
            }
        }
    
        token.str.data++;
        token.str.length--;
        token.type = tok_string;
        return token;
    }
    
    if (IsDigit(c) || c == (uint32)'-') {
        bool integer = true;
        if (c == (uint32)'-') {
            c = GetCodepoint(lexer, &length);
            if (!IsDigit(c)) {
                token.type = tok_error;
                return token;
            }
            token.str.length+=length;
        }
        while (true) {
            c = GetCodepoint(lexer, &length);
            if (!IsDigit(c) && c != (uint32)'.') {
                break;
            }
            if (c == (uint32)'.') {
                token.flags |= 2;
                if (integer) integer = false;
                else {
                    token.type = tok_error;
                    return token;
                }
            }
            token.str.length+=length;
        }
        if (c == (uint32)'e' || c == (uint32)'E') {
            token.flags |= 2;
            token.str.length+=length;
            c = GetCodepoint(lexer, &length);
            token.str.length+=length;
            if (c == (uint32)'+' || c == (uint32)'-' || IsDigit(c)){
                while(true) {
                    c = GetCodepoint(lexer, &length);
                    if (!IsDigit(c)) break;
                token.str.length+=length;
                }
            }
            else {
                token.type = tok_error;
                return token;
            }
        }
        lexer->pos--;
    
        token.type = tok_number;
        if (token.str.length > 18) token.flags |= 2;
        return token;
    }
    
    token.type = tok_error;
    return token;
}