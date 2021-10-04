struct JNode {
	String key;
	Token token; // RawValue and Type
	int64 count;
	JNode* first;
	JNode* last;
	JNode* next;
};

static JNode JNodes[256] = {};
static int32 JCount = 0;

JNode* CreateNewNode() {
	JNode* result = &JNodes[JCount];
	JCount++;
	return result;
}

void AddChild(JNode* parent, JNode* child) {
	if (parent->last) parent->last->next = child;
	else parent->first = child;
	parent->last = child;
	parent->count++;
}

JNode* Parse(Lexer* lexer, String key);

JNode* ParseObject(Lexer* lexer, Token token, String key) {
	JNode* object = CreateNewNode();
	object->key = key;
	object->token = token;

	while (true) {
		Token keyToken = GetToken(lexer);
		if (keyToken.type != tok_string) return Fail("expected key string");
		Token colonToken = GetToken(lexer);
		if (colonToken.type != tok_colon) return Fail("expected colon");

		JNode* child = Parse(lexer, keyToken.str);
		if (!child) return NULL;
		AddChild(object, child);

		Token commaToken = GetToken(lexer);
		if (commaToken.type == tok_rcb) break;
		if (commaToken.type != tok_comma) return Fail("expected comma");
	}

	return object;
}

JNode* ParseArray(Lexer* lexer, Token token, String key) {
	JNode* array = CreateNewNode();
	array->key = key;
	array->token = token;

	while (true) {
		JNode* child = Parse(lexer, {});
		if (!child) return NULL;
		AddChild(array, child);

		Token commaToken = GetToken(lexer);
		if (commaToken.type == tok_rsb) break;
		if (commaToken.type != tok_comma) return Fail("expected comma");
	}

	return array;
}

JNode* Parse(Lexer* lexer, String key) {
	Token token = GetToken(lexer);
	switch (token.type) {
		case tok_lcb: {return ParseObject(lexer, token, key);}
		case tok_lsb: {return ParseArray(lexer, token, key);}
		case tok_string: 
		case tok_number:
		case tok_bool:
		case tok_null: {
			JNode* node = CreateNewNode();
			node->token = token;
			node->key = key;
			return node;
		}
	}
	return NULL;
}