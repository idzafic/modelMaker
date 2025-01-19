#pragma once
#include <unordered_map>
#include<map>
#include <utility>
#include "xml/Writer.h"
#include "cnt/PushBackVector.h"
#include "td/String.h"
#include <vector>
#include <cnt/StringBuilder.h>
#include <regex>
#include <unordered_set>
#include <td/StringConverter.h>
#include <array>

#include <array>
#include <algorithm>
#include <cstddef>



class BaseNode {
public:

	template <size_t N>
	struct ConstExprString {
		char data[N];
		constexpr ConstExprString(const char(&str)[N]) {
			for (std::size_t i = 0; i < N; ++i)
				data[i] = str[i];
		}

		constexpr const char*get() const {
			return data;
		}
	};

private:

	BaseNode* parent = nullptr;
	BaseNode* lastChlid = this;

	static std::cmatch match;
	const static std::regex _lineExtract;
	static std::cmatch match2;
	static const std::regex _attribsExtract;

protected:

	bool _done = false;

	static int _processingLine;
	std::vector<BaseNode*> nodes;
	td::String _comment;
	virtual bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) = 0;
	/*
	
	nodeAction prestavlja glavnu funkciju koju node koristi da procesira svoje komande.
	return false -> node ne moze da procesira ovu komandu, prosljedi je parrent-u
	return true -> node je procesirao datu komandu

	_done = true oznacava da je node zatvoren, nece vise primati komandi i ako je vratio true. Naredni komentari se nece ispisivati u ovom elementu

	ukoliko node stvori novo dijete onda njegov pointer stavlja u newChild argument.
	Time se komande nastavljaju slati njemu.

	*/
	
	void addLine(const char* start, const char *end, const char*comment, int commentLen);
	virtual bool prettyPrint(cnt::StringBuilder<>& str, td::String &indent) const;
	virtual void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const;
	template<ConstExprString... excludeAttribs>
	static void prettyPrintAttribs(cnt::StringBuilder<>& str, const BaseNode *nodeAtribs);
	void clear();
	BaseNode(const BaseNode& node, const td::String &alias);

public:
	
	static const std::unordered_set<td::String> attributeKeywords, functionKeywords, constantsKeywords, syntaxKeywords;
	std::map<td::String, td::String> _attribs;
	static const std::regex varPatten;
	const std::vector<BaseNode*> &getNodes() const;
	BaseNode *getParent() const;
	BaseNode();
	BaseNode(BaseNode&&) = delete;
	BaseNode(const BaseNode& node);
	virtual BaseNode* createCopy(const td::String &alias) const = 0;
	BaseNode &operator =(BaseNode&) = delete;

	void printNodeToString(td::String& string) const;
	bool printNode(const td::String &path) const;
	virtual void printNode(xml::Writer& w) const;

	void prettyPrint(td::String& text) const;
	inline td::String& operator[](td::String attrib) {
		return _attribs[attrib];
	}
	void addComment(const td::String& comment, bool forceNewline = false, bool exitComment = false);
	void addChild(BaseNode *childNode);
	void setAsDone();

	void processCommands(const td::String& text);
	virtual inline const char* getName() const = 0;

	virtual void setAttrib(const td::String& name, const td::String& val) {
		_attribs[name] = val;
	}

	virtual ~BaseNode() {
		for (BaseNode * var : nodes) {
			delete var;
		}
	}





};


class ModelNode : public BaseNode {
	bool done = false;
	ModelNode(const ModelNode& model, const td::String &alias);
public:

	enum class addType{combine, init};

	struct exceptionInvalidBlockName { td::String message; int line; };
	struct exceptionInvalidCommand { td::String message; int line;};
	struct exceptionInvalidAttribute { td::String message; int line; };
	ModelNode() {};
	ModelNode(const ModelNode& model) = default;
	ModelNode &operator =(const ModelNode &model);
	ModelNode(td::String command);
	bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override;
	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override;
	ModelNode& addWtih(const ModelNode &model, const td::String &alias, addType type);
	void clear();
	bool readFromFile(const td::String &path);
	BaseNode* createCopy(const td::String& alias) const override;
	inline const char* getName() const override {
		return "Model";
	}

};

