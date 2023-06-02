#include "encodings.cpp"
#include "lexer.cpp"
#include "parser.cpp"

JNode* Parse(void* buffer, int64 cap) {
	Lexer lexer;
	lexer.pos = (byte*)buffer;
	lexer.cap = cap;
	return __parse(&lexer, {});
}

int64 GetInteger(JNode* node) {
	String str = node->token.str;
	return ParseInt64(str);
}

float64 GetFloatingPoint(JNode* node) {
	String str = node->token.str;
	return ParseFloat64(str);
}

static byte escaped[] = {'"', '\\', '/', '\b', '\f', '\n', '\r', '\t'};
static StringNode StringNodes[512] = {};
static int SCount = 0;

StringNode* CreateStringNode() {
  StringNode* result = &StringNodes[SCount];
  SCount++;
  return result;
}

StringList __unescape(String str) {
  StringList list = {};
  StringNode* node = CreateStringNode();
  node->string.data = str.data;
  for (int32 i = 0; i < str.length; i++) {
      byte c = str.data[i];
      if (c == '\\') {
        StringListAppend(&list, node);
        node = CreateStringNode();
        node->string.length = 1;
        c = str.data[++i];
        switch (c) {
          case '"': node->string.data = &escaped[0]; break;
          case '\\': node->string.data = &escaped[1]; break;
          case '/': node->string.data = &escaped[2]; break;
          case 'b': node->string.data = &escaped[3]; break;
          case 'f': node->string.data = &escaped[4]; break;
          case 'n': node->string.data = &escaped[5]; break;
          case 'r': node->string.data = &escaped[6]; break;
          case 't': node->string.data = &escaped[7]; break;
        }
        StringListAppend(&list, node);
        node = CreateStringNode();
        node->string.data = &str.data[i+1];
      }
      else {
        node->string.length++;
      }
  }
  StringListAppend(&list, node);
  return list;
}

int64 ReadString(JNode* node, void* buffer, int64 cap) {
	StringList list = __unescape(node->token.str);
	int64 left = MIN(cap, list.length);
	int64 read = 0;
	StringNode* snode = list.first;
	while (snode && left) {
		int64 min = MIN(left, snode->string.length);
		memcpy(buffer, snode->string.data, min);
		left -= min;
		read += min;
		snode = snode->next;
	}
	return read;
}

const char* GetTypeName(JNode* node){
	switch (node->token.type) {
		case tok_lcb: return "Object";
		case tok_lsb: return "Array";
		case tok_number: return "Number";
		case tok_string: return "String";
		case tok_bool: return "Boolean";
		case tok_null: return "Null";
	}
	return NULL;
}

JNode* GetChildByKey(JNode* node, String key) {
	JNode* child = node->first;
	while (child) {
		if (StringEquals(child->key, key)) return child;
		child = child->next;
	}
	return NULL;
}

bool IsInteger(JNode* node) {
	return node->token.type == tok_number && !(node->token.flags & 2);
}

int32 GetType(JNode* node){
	switch (node->token.type) {
		case tok_lcb: return 0;
		case tok_lsb: return 1;
		case tok_number: return IsInteger(node) ? 2 : 3;
		case tok_string: return 4;
		case tok_bool: return 5;
		case tok_null: return 6;
	}
	return -1;
}