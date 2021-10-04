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
  char* data;
};

inline char GetChar(Lexer* lexer) {
  return *(lexer->data++);
}

inline bool IsWhiteSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

inline bool IsDigit(char ch) {
  return '0' <= ch && ch <= '9';
}

inline bool IsAlpha(char ch) {
  return ('a' <= ch && ch <= 'z');
}

Token GetToken(Lexer* lexer) {
  Token token = {};
  char c;
  
  // Skip any whitespace.
  do {
    c = GetChar(lexer);
  } while (IsWhiteSpace(c));

  token.str.data = lexer->data - 1;
  token.str.length = 1;

  if (c == ':') {
    token.type = tok_colon;
    return token;
  }
  if (c == ',') {
    token.type = tok_comma;
    return token;
  }
  if (c == '{') {
    token.type = tok_lcb;
    return token;
  }
  if (c == '}') {
    token.type = tok_rcb;
    return token;
  }
  if (c == '[') {
    token.type = tok_lsb;
    return token;
  }
  if (c == ']') {
    token.type = tok_rsb;
    return token;
  }
  if (c == 0) {
    token.type = tok_eof;
    return token;
  }

  if (IsAlpha(c)) {
    while (IsAlpha((c = GetChar(lexer))))
      token.str.length++;

    if (StringEquals(token.str, "true") || StringEquals(token.str, "false"))
      token.type = tok_bool;
    else if (StringEquals(token.str, "null"))
      token.type = tok_null;
    else token.type = tok_error;

    lexer->data--;
    return token;
  }

  if (c == '"') {
    while (true) {
      c = GetChar(lexer);
      if (c == '\\') {
        token.flags |= 1;
        GetChar(lexer);
        token.str.length+=2;
      }
      else {
        if (c == '"') break;
        token.str.length++;
      }
    }

    token.str.data++;
    token.str.length--;
    token.type = tok_string;
    return token;
  }

  if (IsDigit(c) || c == '-') {
    bool integer = true;
    if (c == '-') {
      c = GetChar(lexer);
      if (!IsDigit(c)) {
        token.type = tok_error;
        return token;
      }
      token.str.length++;
    }
    while (true) {
      c = GetChar(lexer);
      if (!IsDigit(c) && c != '.') {
        break;
      }
      if (c == '.') {
        token.flags |= 2;
        if (integer) integer = false;
        else {
          token.type = tok_error;
          return token;
        }
      }
      token.str.length++;
    }
    if (c == 'e' || c == 'E') {
      token.flags |= 2;
      token.str.length++;
      c = GetChar(lexer);
      token.str.length++;
      if (c == '+' || c == '-' || IsDigit(c)){
        while(true) {
          c = GetChar(lexer);
          if (!IsDigit(c)) break;
          token.str.length++;
        }
      }
      else {
        token.type = tok_error;
        return token;
      }
    }
    lexer->data--;

    token.type = tok_number;
    if (token.str.length > 18) token.flags |= 2;
    return token;
  }

  token.type = tok_error;
  return token;
}