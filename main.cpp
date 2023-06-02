
#define TokenType SomeOtherName
#include "../common/basic/basic.cpp"
#undef TokenType

#include "jnode.cpp"

#define PrintSpace() Print(STR(" "))
#define PrintNewLn() Print(STR("\n")) 

void PrintList(StringList list) {
	for (StringNode* node = list.first; node != NULL; node = node->next)
		Print(node->string);
}

void PrintNode(JNode* node, int depth) {
	for (int32 i = 0; i < depth; i++) PrintSpace();

	if (node->key.length) {
		Print(node->key);
		String str = STR(" : ");
		Print(str);
	}
 
 	if (node->token.type == tok_string && node->token.flags & 1)
 		PrintList(__unescape(node->token.str));
	else
		Print(node->token.str);
	PrintNewLn();

	JNode* child = node->first;
	while (child) {
		PrintNode(child, depth + 2);
		child = child->next;
	}
}

void PrintToken(Token token) {
	Print(token.str);
	PrintNewLn();
}

int main() {
	char buffer[512] = {};
	HANDLE hFile = OSOpenFile(L"bloop.json");
	OSReadFile(hFile, buffer, 511);

	JNode* root = Parse(buffer, 512);
	PrintNode(root, 0);

	/*
	Lexer lexer;
	lexer.pos = (byte*)buffer;
	lexer.cap = 512;
	while (true) {
		Token token = GetToken(&lexer);
		PrintToken(token); 
		if (token.type == tok_eof) break;
	}
	*/

	return 0;
}