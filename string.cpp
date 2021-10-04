struct String {
  char* data;
  int64 length;
};

#define STR(x) String{(x), (sizeof(x)-1)}

bool StringEquals(String string, const char* c_str) {
  if (c_str[string.length]) return false;
  return (memcmp(string.data, c_str, string.length) == 0);
}

int64 AnsiToInt64(String str) {
  int64 result = 0;
  int32 start = str.data[0] == '-' ? 1 : 0;
  for (int32 i = start; i < str.length; i++) {
    char c = str.data[i];
    result *= 10;
    result += c-'0';
  }
  if (start == 1) result = -result;
  return result;
}

float64 AnsiToFloat64(String str) {
  float64 result = 0.0;
  int32 start = str.data[0] == '-' ? 1 : 0;
  bool radix = false;
  for (int32 i = start; i < str.length; i++) {
    char c = str.data[i];
    char digit = c-'0';
    if (c == '.') {
      radix = true;
      continue;     
    }
    if (c == 'e' || c == 'E') {
      String exp;
      int32 j = str.data[i+1] == '+' ? i+2 : i+1;
      exp.data = &str.data[j];
      exp.length = str.length - j;
      result *= pow10((int32)AnsiToInt64(exp));
      break;
    }
    if (radix) {
      result += digit/10.0;
    }
    else {
      result *= 10.0;
      result += digit;
    }
  }
  if (start == 1) result = -result;
  return result;
}

struct StringNode {
  String string;
  StringNode* next;
};

struct StringList {
  StringNode* first;
  StringNode* last;
  int64 count;
  int64 length;
};

void Concat(StringList* list, StringNode* node) {
  if (node->string.length == 0) return;
  if (list->last) list->last->next = node;
  else list->first = node;
  list->last = node;
  list->count++;
  list->length += node->string.length;
}

static StringNode StringNodes[512] = {};
static int SCount = 0;

StringNode* CreateStringNode() {
  StringNode* result = &StringNodes[SCount];
  SCount++;
  return result;
}