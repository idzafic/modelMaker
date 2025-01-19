#include "./../../inc/generateXML.h"
#include "./../../inc/nodes.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include <td/String.h>
#include "xml/Writer.h"
#include <map>
#include <utility>
#include <xml/DOMParser.h> 
#include <xml/SAXParser.h> 

#define INDENT_CHAR "\t"
#define IMPLICIT_MULTIPLY false


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



static bool beginsWith(const char* string, const char *what) {
	while (*string != '\0' && *string++ == *what++)
		if (*what == '\0')
			return true;
	return false;
}

static inline bool compareUpperCase(const char* string, const char* uppercaseStringToCompareWith) {

	while (*string != '\0' && std::toupper(*string++) == *uppercaseStringToCompareWith++) {
		if (*uppercaseStringToCompareWith == '\0')
			return true;
	}

	#ifdef MU_DEBUG
		--uppercaseStringToCompareWith;
		if (std::toupper(*uppercaseStringToCompareWith) != *uppercaseStringToCompareWith)
			assert(!"compareUpperCase compared string with something not written in upper case");
	#endif // MU_DEBUG

	return false;
}


static inline const char* skipWhiteSpace(const char* str) {
	while (*str == ' ' || *str == '\t')
		++str;
	return str;
}

static inline const char* returnToNonWhiteSpace(const char * const begin, const char* endPos) {
	while (*endPos == ' ' || *endPos == '\t')
		if(--endPos == begin)
			break;
	return endPos;
}

static inline td::String getStringFromPtr(const char* start, const char* end) {
	return td::String(start, end - start);
}

class conditionNode : public BaseNode {
public:
	enum class type { thenn, elsee };
protected:
	type tip;
public:
	conditionNode(type);
	conditionNode(const conditionNode& node, const td::String &alias):
		BaseNode(node, alias){
		tip = node.tip;
	}
	BaseNode* createCopy(const td::String& alias) const override {
		return new conditionNode(*this, alias);
	}
	bool prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override{
		if(getName()[0] != 'T'){ //not Then
			indent.reduceSize(1);
			str << indent << "else";
			prettyPrintAttribs<"fx">(str, this);

			str << ":";
			indent += INDENT_CHAR;

			if (auto itFx = _attribs.find("fx"); itFx != _attribs.end()){
				str << indent << "\n" << indent << itFx->second;
			}

			return true;
		}

		if (auto itFx = _attribs.find("fx"); itFx != _attribs.end()){
			str << "\n" << indent << itFx->second;
			return true;
		}

		return true;

	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override{
		//str << "\n";
	}

	bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override;
	inline const char* getName() const override {
		return (tip == type::thenn) ? "Then" : "Else";
	}
};

conditionNode::conditionNode(type t) : tip(t) {}


class conditionNodeInline : public conditionNode {
	bool expectingEnd = false;
public:
	conditionNodeInline(conditionNode::type type):
		conditionNode(type)
	{}
	conditionNodeInline(const conditionNode& node, const td::String &alias):
		conditionNode(node, alias)
	{}
	BaseNode* createCopy(const td::String& alias) const override {
		return new conditionNodeInline(*this, alias);
	}

	bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override;
};


static inline void addAlias(td::String& line, const td::String& alias) { //doda ime ispred varijable npr import.x, takoder dodaje "*" ispred varijable npr 3x pretvara u 3*x

	std::cmatch match;
	cnt::StringBuilderSmall str;

	const td::UTF8* start = line.begin();
	const td::UTF8* previousEnd = start;
	previousEnd = start;

	while (std::regex_search(start, line.end(), match, ModelNode::varPatten)) {
		td::String var(match[2].first, match[2].length());
		if constexpr (IMPLICIT_MULTIPLY) {
			if (match[2].first != line.begin() && std::isdigit(*(match[2].first - 1))) {
				str.appendString(previousEnd, match[2].first - previousEnd);
				str << "*";
				previousEnd = match[2].first;
			}
		}
		if (!alias.isNull()) {
			if (!ModelNode::functionKeywords.contains(var)) {
				str.appendString(previousEnd, match[2].first - previousEnd);
				str << alias << ".";
				previousEnd = match[2].first;
			}
		}


		start = match.suffix().first;
	}

	if (start != line.begin()) {
		str.appendString(previousEnd, line.end() - previousEnd);
		str.getString(line);
	}

}


template<typename conditionNodeType>
class singleEquation : public BaseNode {
	bool consumeEnd = false;
	bool addIf = false;
public:
	singleEquation() = default;
	singleEquation(const singleEquation &n, const td::String &alias ):BaseNode(n, alias)
	{
		consumeEnd = n.consumeEnd;
	}
	BaseNode* createCopy(const td::String& alias) const override {
		singleEquation &equation = *new singleEquation(*this, alias);
		
		if (auto itFx = equation._attribs.find("fx"); itFx != equation._attribs.end())
			addAlias(itFx->second, alias);
		if (auto itW = equation._attribs.find("w"); itW != equation._attribs.end())
			addAlias(itW->second, alias);
		if (auto it = equation._attribs.find("cond"); it != equation._attribs.end())
			addAlias(it->second, alias);
		
		return &equation;
	}

	bool prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override{
		if (auto it = _attribs.find("cond"); it != _attribs.end()){
			str << indent << "if " << it->second;
			if(getNodes().size() != 0)
				prettyPrintAttribs<"fx", "cond">(str, getNodes()[0]);
			str << ":";
			indent += INDENT_CHAR;
			return false;
		}

		if (auto itFx = _attribs.find("fx"); itFx != _attribs.end())
			str << indent << itFx->second;

		prettyPrintAttribs<"fx">(str, this);
		
		return false;

	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override{
		if (auto it = _attribs.find("cond"); it != _attribs.end()) {
			indent.reduceSize(1);
			str << indent << "end";
		}
		str << "\n";
	}


	virtual bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override {

		if (compareUpperCase(cmndStart, "THEN"))
			return true;


		if (compareUpperCase(cmndStart, "END")) {
			if (consumeEnd) {
				_done = true;
				consumeEnd = false;
				return true;
			}
			return false;
		}

		if (_done)
			return false;

		if (compareUpperCase(cmndStart, "ELSE")) {
			auto elseN = new conditionNodeType(conditionNode::type::elsee);
			addChild(elseN);
			newChild = nodes.back();
			_done = true;
			consumeEnd = true;
			static const char* ifPtr = "if";
			if(const char *poz = std::search(cmndStart, cmndEnd, ifPtr, ifPtr+2); poz != cmndEnd) {
				auto lastNode = new conditionNodeType(conditionNode::type::thenn);
				auto equation = new singleEquation;
				elseN->addChild(equation);
				equation->setAttrib("cond", getStringFromPtr(poz+2, cmndEnd));
				equation->consumeEnd = true;
				equation->addChild(lastNode);
				newChild = lastNode;
				consumeEnd = true;
				return true;
			}
			
			return true;
		}
		
		if (beginsWith(cmndStart, "if")) {
			setAttrib("cond", getStringFromPtr(skipWhiteSpace(cmndStart+2), cmndEnd));
			addChild(new conditionNodeType(conditionNode::type::thenn));
			newChild = nodes.back();
			consumeEnd = true;
			return true;
		}

		_done = true;

		setAttrib("fx", getStringFromPtr(cmndStart, cmndEnd));

		return true;
	}
	inline const char* getName() const override {
		return "Eq";
	}
};

typedef singleEquation<conditionNode> Equation;
typedef singleEquation<conditionNodeInline> EquationCounted;


bool conditionNode::nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild)
{
	if (compareUpperCase(cmndStart, "END")) {
		_done = true;
		return false;
	}

	if (compareUpperCase(cmndStart, "ELSE")) {
		_done = true;
		return false;
	}

	addChild(new Equation());
	newChild = nodes.back();

	return false;
}



bool conditionNodeInline::nodeAction(const char *cmndStart, const char *cmndEnd, BaseNode *&newChild){
	if(_done)
		return false;

	if (compareUpperCase(cmndStart, "END")){
		_done = true;
		return false;
	}

	 if (tip == conditionNode::type::thenn && compareUpperCase(cmndStart, "ELSE")) {
		_done = true;
		return false;
	}

	if(expectingEnd){
		td::String msg;
		if(tip == conditionNode::type::thenn)
			msg = "Expected \"else\" or \"end\" but instead got ";
		else
			msg = "Expected \"end\" but instead got ";
		msg += getStringFromPtr(cmndStart, cmndEnd);
		throw ModelNode::exceptionInvalidCommand{msg, _processingLine};
	}

	expectingEnd = true;

	if (compareUpperCase(cmndStart, "IF")) {
		newChild = new EquationCounted;
		addChild(newChild);
		return false;
	}

	_attribs["fx"] = getStringFromPtr(cmndStart, cmndEnd);
	return true;
}


template<BaseNode::ConstExprString containerName, typename nodeType>
class containerNode : public BaseNode {
public:
	containerNode() = default;
	containerNode(const containerNode<containerName, nodeType>& node, const td::String &alias):
	BaseNode(node,alias){}
	BaseNode* createCopy(const td::String &alias) const override {
		return new containerNode<containerName, nodeType>(*this, alias);
	}
	bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override {
		if (_done) return false;

		if (compareUpperCase(cmndStart, "GROUP")) {
			typedef containerNode<"Group", Equation> GroupNode;
			addChild(new GroupNode);
			newChild = nodes.back();
			return true;
		}


		if (compareUpperCase(cmndStart, "END") || compareUpperCase(cmndStart, "STATS:")) 
		{
			_done = true;
			return false;
		}

		while (*cmndEnd != '\n' && *cmndEnd != '\0') {
			if (*cmndEnd == ':')
				if (!compareUpperCase(cmndStart, "IF") && !compareUpperCase(cmndStart, "ELSE"))
					return false;
			++cmndEnd;
		}

		newChild = new nodeType;
		addChild(newChild);

		return false;

	}
	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String& indent) const override {
		indent.reduceSize(1);
	}
	
	inline const char* getName() const override {
		return containerName.get();
	}

};

template<ConstExprString variableName>
class variableNode : public BaseNode {
public:
	variableNode() = default;
	variableNode(const variableNode &n) = default;
	variableNode(const variableNode &n, const td::String &alias):
		BaseNode(n,alias){}
	virtual BaseNode* createCopy(const td::String &alias) const override{
		auto &var = *new variableNode(*this, alias);
		if (auto it = var._attribs.find("name"); it != var._attribs.end())
			addAlias(it->second, alias);
		if (auto it = var._attribs.find("val"); it != var._attribs.end())
			addAlias(it->second, alias);
		return &var;
	}
	bool prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override {
		if (auto it = _attribs.find("name"); it != _attribs.end())
			str << indent << it->second;
		if (auto it = _attribs.find("val"); it != _attribs.end())
			str << " = " << it->second;

		prettyPrintAttribs<"name", "val">(str, this);
		return false;
	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override{
		str << "\n";
	}

	bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override {
		if (_done) 
			return false;
		_done = true;

		const char *pozEq = std::find(cmndStart, cmndEnd, '=');
		const char *pozZagrada = std::find(cmndStart, cmndEnd, '{');
		if (pozEq == cmndEnd || pozZagrada != cmndEnd) {
			setAttrib("name", getStringFromPtr(cmndStart, cmndEnd));
			return true;
		}

		setAttrib("name", getStringFromPtr(cmndStart, returnToNonWhiteSpace(cmndStart, pozEq-1)+1));
		setAttrib("val", getStringFromPtr(skipWhiteSpace(pozEq + 1), cmndEnd));
		
		return true;
	}

	inline const char* getName() const override {
		return variableName.get();
	}
};




class SingleExpression : public BaseNode {
public:
	SingleExpression() = default;
	SingleExpression(const SingleExpression& n) = default;
	SingleExpression(const SingleExpression& n, const td::String& alias) :
		BaseNode(n, alias) {}
	virtual BaseNode* createCopy(const td::String& alias) const override {
		return new SingleExpression(*this, alias);
	}
	bool prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override {
		if (auto it = _attribs.find("fx"); it != _attribs.end())
			str << indent << it->second;
		prettyPrintAttribs<"fx">(str, this);
		return false;
	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String& indent) const override {
		str << "\n";
	}

	bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override {
		if (_done) 
			return false;
		_done = true;

		setAttrib("fx", getStringFromPtr(cmndStart, cmndEnd));

		return true;
	}

	inline const char* getName() const override {
		return "Expression";
	}
};

static td::String prepend(const td::String &string, td::String toPrepend){
	toPrepend += ".";
	toPrepend += string;
	return toPrepend;
}

class RndNode : public BaseNode {
public:
	RndNode() = default;
	RndNode(const RndNode& node, const td::String &alias ):
		BaseNode(node, alias){
			auto it = _attribs.find("name");
			if(it == _attribs.end() || alias.isNull())
				return;
			it->second = prepend(it->second, alias);
		}

	BaseNode* createCopy(const td::String &alias) const override {
		return new RndNode(*this, alias);
	}

	virtual bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override {

		if(_done)
			return false;

		_attribs["name"] = getStringFromPtr(cmndStart, cmndEnd);

		_done = true;
		return true;
	}


	bool prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override{
		auto it = _attribs.find("name");
		if(it != _attribs.end())
			str << it->second;
		prettyPrintAttribs<"name">(str, this);
		return false;
	}


	inline const char* getName() const override {
		return "RndGen";
	}
};





typedef containerNode<"Vars", variableNode<"Var">> varsNode;
typedef containerNode<"Params", variableNode<"Param">> paramsNode;
typedef containerNode<"NLEqs", EquationCounted> NLEquationsNode;
typedef containerNode<"ODEqs", EquationCounted> ODEquationsNode;
typedef containerNode<"PreProc", Equation> preProcNode;
typedef containerNode<"PostProc", Equation> postProcNode;
typedef containerNode<"MeasEqs", EquationCounted> MeasEqNode;

typedef containerNode<"Limits", Equation> LimitNode;
typedef containerNode<"ECs", EquationCounted> ECsNode;
typedef containerNode<"TFs", EquationCounted> TFsNode;
typedef containerNode<"Expressions", SingleExpression> ExpressionsNode;
typedef containerNode<"RndGens", RndNode> RndGensNode;


class StatsNode : public BaseNode {
public:
	StatsNode() = default;
	StatsNode(const StatsNode& node, const td::String &alias ):
		BaseNode(node, alias){
			auto it = _attribs.find("name");
			if(it == _attribs.end() || alias.isNull())
				return;
			it->second = prepend(it->second, alias);
		}

	BaseNode* createCopy(const td::String &alias) const override {
		return new StatsNode(*this, alias);
	}

	virtual bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override {

		if(_done)
			return false;

		cnt::PushBackVector<td::String> cnt;
		constexpr int statTagLen = 6; //lenght of string "Stats:"
		getStringFromPtr(cmndStart+statTagLen, cmndEnd).split(",", cnt, true, false);
		if(!cnt.isEmpty()){
			_attribs["name"] = cnt.last();
			cnt.remove_back();
		}
		while(!cnt.isEmpty()){
			auto stat = new StatsNode;
			stat->_attribs["name"] = cnt.last();
			stat->_done = true;
			getParent()->addChild(stat);
			cnt.remove_back();
		}

		_done = true;
		return true;
	}


	bool prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const override{
		auto it = _attribs.find("name");
		if(it != _attribs.end())
			str << "\n" << indent << "Stats: " << it->second;
		//prettyPrintAttribs<"name">(str, this);
		return true;
	}


	inline const char* getName() const override {
		return "Stats";
	}
};


class initNode : public BaseNode {
public:
	initNode() = default;
	initNode(const initNode& node, const td::String &alias ):
		BaseNode(node, alias){

		}
	BaseNode* createCopy(const td::String &alias) const override {
		return new initNode(*this, alias);
	}
	virtual bool nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild) override {

		if(_done)
			return false;

		if (compareUpperCase(cmndStart, "MODEL")) {
			addChild(new ModelNode);
			newChild = nodes.back();
			return true;
		}else{
			_done = true;
			return false;
		}

		return false;
	}

	void prettyPrintClosing(cnt::StringBuilder<>& str, td::String &indent) const override{
		indent.reduceSize(1);
		str << indent << "\n";
	}

	inline const char* getName() const override {
		return "Init";
	}
};


ModelNode::ModelNode(const ModelNode& model, const td::String &alias):
	BaseNode(model, alias)
{

}

ModelNode &ModelNode::operator=(const ModelNode &model)
{
    this->~ModelNode();
	new(this) ModelNode(model);
	return *this;
}

ModelNode::ModelNode(td::String text)
{
    processCommands(text);
}


bool ModelNode::nodeAction(const char* cmndStart, const char* cmndEnd, BaseNode*& newChild)
{
	if (_done) 
		return false;

	if (compareUpperCase(cmndStart, "END")) {
		_done = true;
		return true;
	}

	if (compareUpperCase(cmndStart, "VARS"))
		addChild(new varsNode());
	else if (compareUpperCase(cmndStart, "PARAMS"))
		addChild(new paramsNode());
	else if (compareUpperCase(cmndStart, "MODEL")) {
		newChild = this;
		return true;
	}
	else if (compareUpperCase(cmndStart, "NLEQS") || compareUpperCase(cmndStart, "NL"))
		addChild(new NLEquationsNode());
	else if (compareUpperCase(cmndStart, "ODEQS") || compareUpperCase(cmndStart, "ODE"))
		addChild(new ODEquationsNode());
	else if (compareUpperCase(cmndStart, "INIT"))
		addChild(new initNode);
    else if (compareUpperCase(cmndStart, "PREPROC"))
        addChild(new preProcNode);
	else if (compareUpperCase(cmndStart, "POSTPROC"))
		addChild(new postProcNode);
	else if (compareUpperCase(cmndStart, "MEASEQS") || compareUpperCase(cmndStart, "MEAS"))
		addChild(new MeasEqNode);
	else if (compareUpperCase(cmndStart, "LIMITS"))
		addChild(new LimitNode);
	else if (compareUpperCase(cmndStart, "ECS") || compareUpperCase(cmndStart, "EC"))
		addChild(new ECsNode);
	else if (compareUpperCase(cmndStart, "TFS") || compareUpperCase(cmndStart, "TF"))
		addChild(new TFsNode);
	else if (compareUpperCase(cmndStart, "EXPRESSIONS"))
		addChild(new ExpressionsNode);
	else if (compareUpperCase(cmndStart, "RNDGENS"))
		addChild(new RndGensNode);
	else if (compareUpperCase(cmndStart, "STATS")){
		addChild(new StatsNode);
		newChild = nodes.back();
		return false;
	}
	else
		throw exceptionInvalidBlockName{td::String(cmndStart, cmndEnd-cmndStart), _processingLine};

	
	newChild = nodes.back();
	return true;
}

void ModelNode::prettyPrintClosing(cnt::StringBuilder<> &str, td::String &indent) const
{
	if (getParent() != nullptr)
		indent.reduceSize(1);
	str << indent << "end\n";
}

ModelNode &ModelNode::addWtih(const ModelNode &model, const td::String &alias, addType type)
{
	if(type == addType::combine){

		for (const auto& a : model._attribs) {
			if (a.first.cCompareNoCase("domain") == 0) {
				if (!_attribs.contains("domain"))
					_attribs["domain"] = a.second;
				else if(_attribs["domain"].cCompareNoCase("real") == 0)
					_attribs["domain"] = a.second;
			}

			if (a.first.cCompareNoCase("type") == 0) {
				if (!_attribs.contains("type"))
					_attribs["type"] = a.second;
				else if (_attribs["type"].cCompareNoCase("NR") == 0)
					_attribs["type"] = a.second;
				else if (_attribs["type"].cCompareNoCase("ODE") == 0 && a.second.cCompareNoCase("DAE") == 0)
					_attribs["type"] = "DAE";

			}

		}

		bool found;
		for (const auto& n : model.nodes) {
			found = false;
			for (const auto& thisNode : nodes)
				if (std::strcmp(thisNode->getName(), n->getName()) == 0) {
					found = true;
					for (const auto& child : n->getNodes())
						thisNode->addChild(child->createCopy(alias));
				}
				
			if (!found)
				nodes.push_back(n->createCopy(alias));
		}
	}else if(type == addType::init){
		bool found = false;

		for (const auto& n : nodes) {
			if (std::strcmp(n->getName(), "Init") == 0) {
				found = true;
				n->addChild(model.createCopy(alias));
			}
		}

		if(!found){
			auto ptr = new initNode;
			ptr->addChild(model.createCopy(alias));
			nodes.insert(nodes.begin()+2, ptr);
		}

	}


	return *this;
}

void ModelNode::clear(){
	BaseNode::clear();
	_done = false;
}




BaseNode *ModelNode::createCopy(const td::String& alias) const
{
	return new ModelNode(*this, alias);
}


void generateXML(const td::String& equations, const td::String& output_path) {
	ModelNode m(equations);

	xml::Writer w(output_path);
	w.startDocument();

	m.printNode(w);

	w.endDocument();
	w.close();
}




class TextParser : public xml::SAXParser<TextParser, FileBuffer4k, 1023U, false, false>
{
private:
	ModelNode::exceptionInvalidBlockName _exception;
	ModelNode& _model;
	BaseNode* _lastNode, *_closedNode;
	mem::StringMemoryManager<> _stringPool;
	bool _skipNode = true, _commentAlone = true;
protected:
	typedef xml::SAXParser<TextParser, FileBuffer4k, 1023U, false, false> TSAX;
protected:

public:

	inline td::StringExt* allocStringObject(td::UINT4 len, const char* pStr)
	{
		return _stringPool.allocObject(len, pStr);
	}

	bool onOpenNode()
	{
		if (_skipNode) {
			_skipNode = false;
			return true;
		}
		
		_commentAlone = false;

		td::StringExt* name = TSAX::_pLastNode->pName;
	
		if (name->cCompare("Model") == 0)
			_lastNode->addChild(new ModelNode);
		else if (name->cCompare("Vars") == 0)
			_lastNode->addChild(new varsNode);
		else if (name->cCompare("Var") == 0)
			_lastNode->addChild(new variableNode<"Var">);
		else if (name->cCompare("Params") == 0)
			_lastNode->addChild(new paramsNode);
		else if (name->cCompare("Param") == 0)
			_lastNode->addChild(new variableNode<"Param">);
		else if (name->cCompare("NLEqs") == 0)
			_lastNode->addChild(new NLEquationsNode);
		else if (name->cCompare("Eq") == 0)
			_lastNode->addChild(new Equation);
		else if (name->cCompare("Then") == 0)
			_lastNode->addChild(new conditionNode(conditionNode::type::thenn));
		else if (name->cCompare("Else") == 0)
			_lastNode->addChild(new conditionNode(conditionNode::type::elsee));
		else if (name->cCompare("Group") == 0)
			_lastNode->addChild(new containerNode<"Group", Equation>);
		else if (name->cCompare("ODEqs") == 0)
			_lastNode->addChild(new ODEquationsNode);
		else if (name->cCompare("Init") == 0)
			_lastNode->addChild(new initNode);
        else if (name->cCompare("PreProc") == 0)
            _lastNode->addChild(new preProcNode);
		else if (name->cCompare("PostProc") == 0)
			_lastNode->addChild(new postProcNode);
		else if (name->cCompare("MeasEqs") == 0)
			_lastNode->addChild(new MeasEqNode);
		else if (name->cCompare("Limits") == 0)
			_lastNode->addChild(new LimitNode);
		else if (name->cCompare("ECs") == 0)
			_lastNode->addChild(new ECsNode);
		else if (name->cCompare("TFs") == 0)
			_lastNode->addChild(new TFsNode);
		else if (name->cCompare("Expressions") == 0)
			_lastNode->addChild(new ExpressionsNode);
		else if (name->cCompare("Expression") == 0)
			_lastNode->addChild(new SingleExpression);
		else if(name->cCompare("RndGens") == 0)
			_lastNode->addChild(new RndGensNode);
		else if(name->cCompare("RndGen") == 0)
			_lastNode->addChild(new RndNode);
		else if(name->cCompare("Stats") == 0)
			_lastNode->addChild(new StatsNode);
		else {
			_exception.message = name->c_str();
			_exception.line = TSAX::_nLines;
			TSAX::reload(); //prekini parsiranje
		}
		
		_lastNode = _lastNode->getNodes().back();

		return true;
		
	}

	void onDummyNode()
	{
		//        mu::dbgLog("Dummy node:%s", TSAX::_pLastNode->pName->c_str());
	}

	void onCloseNode()
	{
		_closedNode = _lastNode;
		_lastNode->setAsDone();
		if (_lastNode->getParent() != nullptr)
			_lastNode = _lastNode->getParent();
		_commentAlone = false;
	}



	void onCDATA()
	{

	}

	void onDocType()
	{

	}

	void onAttrib()
	{
		_lastNode->_attribs[TSAX::_pLastAttrib->pName->c_str()] = TSAX::_outBuffer.c_str();
	}

	void onNodeText()
	{
		const char* p = (const td::UTF8*)TSAX::_outBuffer.begin();
		const char* k = TSAX::_outBuffer.size() + p;
		_commentAlone = std::find(p, k, '\n') != k;
	}

	void onNodeComment() {
		auto str = TSAX::_outBuffer.c_str();


		if (!_commentAlone) {
			if (_lastNode->getNodes().empty())
				_lastNode->addComment(str);
			else
				_closedNode->addComment(str);
			return;
		}


		if (_lastNode->getNodes().empty()) 
			_lastNode->addComment(str, true);
		else
			_lastNode->getNodes().back()->addComment(str, false, true);
		

	}


public:
	TextParser(ModelNode& model) :
		_model(model),
		_lastNode(&model),
		_closedNode(&model)
	{
		_callOnNodeTextForWhitespace = true;
	}
	~TextParser()
	{

	}

	bool parseFile(const td::String& fileName, const mu::ILogger* pLog = nullptr)
	{
		_model.clear();
		bool bRet = TSAX::parseFile(fileName, pLog);
		if (!_exception.message.isNull())
			throw _exception;
		return true;
	}
};



bool ModelNode::readFromFile(const td::String& path)
{
	TextParser par(*this);
	return par.parseFile(path);

}

