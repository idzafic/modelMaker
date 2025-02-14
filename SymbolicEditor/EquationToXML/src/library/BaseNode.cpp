#include "./../../inc/nodes.h"
#include <vector>
#include <cnt/PushBackVector.h>
#include "td/String.h"
#include <stack>

#define BACK_COMMENT_CHAR "`"
#define INDENT_CHAR "\t"

const std::unordered_set<td::String> BaseNode::attributeKeywords{"type", "domain", "name", "eps", "dT", "signal", "out", "desc", "method", "w", "conj", "limit", "setFlag", "dev", "mean", "width"};
const std::unordered_set<td::String> BaseNode::functionKeywords{ "abs","acos","asin","atg","cos","exp","sqrt","ln","log",\
"lim", "int", "sin","tg", "sqr", "atan2", "sign", "sinh", "cosh", "tgh", "asnh", "acsh", "atgh", "disc","conj", "real", "imag" };

const std::unordered_set<td::String> BaseNode::constantsKeywords{"pi", "e", "true", "false", "LowLimit", "HighLimit"};
const std::unordered_set<td::String> BaseNode::syntaxKeywords{"base", "if", "else", "end", "Model", "Vars", "Var", "Params", "Param", "Params", "NLEqs", "NLE", \
"RndGens", "Group", "ODEqs", "ODE", "Init", "PreProc", "PostProc", "MeasEqs", "Limits", "EC", "ECs", "TFs", "TF", "Expressions", "SubModel"};

int BaseNode::_processingLine = 0;

const std::regex BaseNode::varPatten = std::regex(R"((base\.|^|[^A-Za-z_\.])([a-zA-Z_](?:[\w0-9.]+?)?)(?:$|[^\w.]))");
const std::regex BaseNode::_lineExtract = std::regex(R"(\s*([^\n]*?)[ \t]*:?[ \t]*(?:(?:$|\n)|(?:(?://|#)([^\n]*))))");
const std::regex BaseNode::_attribsExtract(R"((?:,|\[|^)\s*([^=,\s]+?)\s*=\s*([^=,]+?)\s*(?=\]|,|$))");

std::cmatch BaseNode::match;
std::cmatch BaseNode::match2;


void BaseNode::printNodeToString(td::String& string) const
{
	xml::Writer w;
	w.startDocument();
	printNode(w);
	w.getString(string);
}

bool BaseNode::printNode(const td::String& path) const
{
	xml::Writer w;
	w.open(path);
	if(!w.isOk())
		return false;
	w.startDocument();
	printNode(w);
	w.close();
	if(!w.isOk())
		return false;
	return true;
}

void BaseNode::printNode(xml::Writer& w) const
{
	if(strcmp(this->getName(), "SubModel") == 0){
		w.startNode("Init");
		w.startNode("Model");
	}
	else
		w.startNode(this->getName());
	for (auto & at : this->_attribs)
		w.attributeC(at.first.c_str(), at.second);

	if (nodes.size() == 0){
		if(strcmp(this->getName(), "SubModel") == 0)
			w.endNode();
		w.endNode();
	}

	cnt::PushBackVector<td::String> comSides;

	const char* backCommentStart = _comment.end();

	if (!_comment.isNull()) {
		const char* debug = _comment.c_str();

		int poz = _comment.find(BACK_COMMENT_CHAR);
		if (poz != -1)
			backCommentStart = _comment.begin() + poz;

		if (poz != 0) {

			const char* ptr = nullptr, * begin = _comment.begin();

			ptr = std::find(begin, backCommentStart, '\n');
			if (ptr != begin)
				w.comment(td::String(begin, ptr - begin).c_str());
			begin = ptr + 1;

			while (ptr != backCommentStart) {
				ptr = std::find(begin, backCommentStart, '\n');
				if (ptr == begin)
					w.comment("\n");
				else
					w.commentInNewLine(td::String(begin, ptr - begin).c_str());
				begin = ptr + 1;
			}
		}

	}

	if (nodes.size() != 0) {
		for (BaseNode * var : nodes)
			var->printNode(w);
			
		if(strcmp(this->getName(), "SubModel") == 0)
			w.endNode();
		w.endNode();
	}

	

	//'BACK_COMMENT_CHAR' oznacava da se komentar printa na kraju elementa


if (backCommentStart != _comment.end()) {
	const char* ptr = nullptr;

	++backCommentStart;

		while (ptr != _comment.end()) {
			ptr = std::find(backCommentStart, _comment.end(), BACK_COMMENT_CHAR[0]);
			if (ptr == backCommentStart)
				w.comment("\n");
			else
				w.commentInNewLine(td::String(backCommentStart, ptr - backCommentStart).c_str());
			backCommentStart = ptr + 1;
		}
	}
	

}




template<BaseNode::ConstExprString ...excludeAttribs>
void BaseNode::prettyPrintAttribs(cnt::StringBuilder<>& str, const BaseNode* nodeAtribs)
{
	bool firstAttrib = true;
	for (const auto& a : nodeAtribs->_attribs) {
		if (((a.first.cCompare(excludeAttribs.get()) == 0) || ...))
			continue;
		if (firstAttrib) {
			str << " [ ";
			firstAttrib = false;
		}
		else
			str << ", ";
		str << a.first << " = " << a.second;
	}
	if (!firstAttrib)
		str << " ]";
}


template void BaseNode::prettyPrintAttribs<"fx">(cnt::StringBuilder<>&, const BaseNode* nodeAtribs);
template void BaseNode::prettyPrintAttribs<"fx", "cond">(cnt::StringBuilder<>&, const BaseNode* nodeAtribs);
template void BaseNode::prettyPrintAttribs<"name", "val">(cnt::StringBuilder<>&, const BaseNode* nodeAtribs);
template void BaseNode::prettyPrintAttribs<"name">(cnt::StringBuilder<>&, const BaseNode* nodeAtribs);


void BaseNode::prettyPrint(td::String& text) const
{
	cnt::StringBuilder str;
	td::String indent;
	
	std::stack<const BaseNode*> stack;
	stack.push(this);
	const BaseNode* current=nullptr;
	bool newline;

	const auto closeNode = [&current, &str, &indent](const BaseNode *node) {
		node->prettyPrintClosing(str, indent);

		if (node->_comment.isNull())
			return;
	
		const char* ptr;
		const char* begin = std::find(node->_comment.begin(), node->_comment.end(), BACK_COMMENT_CHAR[0]);

		if (begin == node->_comment.end())
			return;

		++begin;
		while (true) {
			ptr = std::find(begin, node->_comment.end(), BACK_COMMENT_CHAR[0]);
			if (ptr != node->_comment.end()) {
				str << indent << " //";
				str.appendString(begin, ptr - begin);
				str << "\n";
				begin = ptr + 1;
			}
			else
				break;
		}
		str << indent << " //";
		str.appendString(begin, node->_comment.end() - begin);
		str << "\n";
		
	};

	while (!stack.empty()) {
		current = stack.top();
		
		if (current == nullptr) {
			stack.pop();
			closeNode(stack.top());
			stack.pop();
			continue;
		}
		
		newline = current->prettyPrint(str, indent);

		if (!current->_comment.isNull()) {
			const char* bc = std::find(current->_comment.begin(), current->_comment.end(), BACK_COMMENT_CHAR[0]);
			const char* ptr, * begin = current->_comment.begin();
			
			while (true) {
				ptr = std::find(begin, bc, '\n');
				if (ptr != bc) {
					if (begin != ptr) {
						str << " //";
						str.appendString(begin, ptr - begin);
						
					}
					str << "\n" << indent;
					begin = ptr + 1;
				}
				else
					break;
			}

			if (bc != begin) {
				str << " //";
				str.appendString(begin, bc - begin);
			}
		}

		if (newline)
			str << "\n";
		
		if (!current->nodes.empty()) {
			stack.push(nullptr);

			for (auto it = current->nodes.rbegin(); it != current->nodes.rend(); ++it)
			stack.push(*it);

		}else{
			stack.pop();
			closeNode(current);
		}

		
	}

	str.getString(text);
}




void BaseNode::addLine(const char* start, const char* end, const char* comment, int commentLen)
{

	td::String attrib;
	BaseNode* child = nullptr;
	bool childAdded = false;

	const char* poz1 = std::find(start, end, '[');
	const char *poz2 = std::find(start, end, ']'); // oznacavanje pocetka i kraja atributa

	if(!(poz1 == end && poz2 == end))
		if (poz1 > poz2 || (poz1 != end && poz2 == end) || (poz1 == end && poz2 != end) || (poz1 != start && poz2 != end-1)) //ako nisu ispravno postavljenje zagrade []
		{
			throw ModelNode::exceptionInvalidCommand{td::String(start, end-start), _processingLine};
		}


	const char* cmndStart = (poz1 == start) ? poz2 + 1 : start;
	const char* cmndEnd = (poz2 == end-1) ? poz1 : end;



	if(!(poz1 == start && poz2 == end)) { //procesiranje komande ako postoji
		// ovdje bi se mogao dodati check da ako je je current node model ili single equation koji posjeduje atribut cond ili condition node tipa 'else', onda *end mora biti ':'

		while (*cmndStart == ' ' || *cmndStart == '\t')
			++cmndStart;

		--cmndEnd;
		while (*cmndEnd == ' ' || *cmndEnd == '\t')
			--cmndEnd;
		++cmndEnd;

		while (!lastChlid->nodeAction(cmndStart, cmndEnd, child)) {
			if (child != nullptr) {
				lastChlid = child;
				child = nullptr;
				childAdded = true;
				continue;
			}
			lastChlid = lastChlid->parent;
			if(lastChlid == nullptr)//nema se vise kome proslijediti komanda svi node-ovi su je odbili. Ovo se moze desit ako se stavi previse puta 'end'
				throw ModelNode::exceptionInvalidCommand{td::String(cmndStart, cmndEnd - cmndStart), _processingLine};
			}


		if (childAdded == true || child != nullptr) {
			if(child != nullptr)
				lastChlid = child;
			lastChlid->_comment.fromKnownString(comment, commentLen);
		}
		else {
			lastChlid->addComment(td::String(comment, commentLen), false, true);

			if (child == nullptr && poz1 != end) //komanda ne stvara novi node ali postavlja atribute
				throw ModelNode::exceptionInvalidCommand{td::String(match[1].first, match[1].length()), _processingLine};
		}

		
		if (poz1 != end) { //pocesiranje atributa ako postoje
			while (std::regex_search(poz1, poz2, match2, _attribsExtract)) {
				attrib.fromKnownString(match2[1].first, match2[1].length());

				if (!attributeKeywords.contains(attrib))
					throw ModelNode::exceptionInvalidAttribute{std::move(attrib), _processingLine};

				lastChlid->_attribs[attrib].fromKnownString(match2[2].first, match2[2].length());

				poz1 = match2.suffix().first;
			}
		}

	}else
		throw ModelNode::exceptionInvalidCommand{td::String(start, end - start), _processingLine};

}



void BaseNode::processCommands(const td::String& text)
{
	parent = nullptr;
	_processingLine = 0;
	lastChlid = this;
	const char* start = text.begin();
	while (std::regex_search(start, text.end(), match, _lineExtract)) {
		if (match[1].length() == 0) {
			if (match[2].matched) {
				if (lastChlid != nullptr) {
					if(lastChlid->_done)
						lastChlid->addComment(td::String(match[2].first, match[2].length()), false, true);
					else
						lastChlid->addComment(td::String(match[2].first, match[2].length()), true, false);
				}
			}
			else
				break;
		}
		else {
			addLine(match[1].first, match[1].first + match[1].length(), match[2].first, match[2].length());
		}
		++_processingLine;
		start = match.suffix().first;
	}

}

bool BaseNode::prettyPrint(cnt::StringBuilder<>& str, td::String& indent) const
{
	str << indent << getName();
	prettyPrintAttribs<>(str, this);
	str << ":";
	if (parent != nullptr)
		indent += INDENT_CHAR;
	return true;
}



void BaseNode::prettyPrintClosing(cnt::StringBuilder<> &str, td::String &indent) const
{
	str << "\n";
	if (parent != nullptr)
		indent.reduceSize(1);
}
void BaseNode::addComment(const td::String &comment,bool forceNewline, bool exitComment)
{
    if (comment.isNull())
		return;
	if (!exitComment) {
		if (!this->_comment.isNull() || forceNewline) {
			this->_comment += "\n";
			this->_comment += comment;
		}
		else
			this->_comment = comment;
	}
	else {
		this->_comment += BACK_COMMENT_CHAR;
		this->_comment += comment;
	}

}


void BaseNode::addChild(BaseNode *childNode)
{
	nodes.emplace_back(childNode);
	nodes.back()->parent = this;
}

void BaseNode::setAsDone()
{
	_done = true;
}


void BaseNode::clear()
{
	for (const auto& node : nodes)
		delete node;
	nodes.clear();
	_attribs.clear();
	_comment.clean();
	lastChlid = this;
	parent = nullptr;
}

BaseNode::BaseNode(const BaseNode &node, const td::String &alias)
{
	_comment = node._comment;
	lastChlid = node.lastChlid;
	parent = node.parent;
	_attribs = node._attribs;
	_done = node._done;
	for (const auto& n : node.nodes)
		nodes.emplace_back(n->createCopy(alias));
}
const std::vector<BaseNode *> &BaseNode::getNodes() const
{
    return nodes;
}

BaseNode *BaseNode::getParent() const
{
    return parent;
}
BaseNode::BaseNode()
{
}

BaseNode::BaseNode(const BaseNode& node)
{
	_comment = node._comment;
	lastChlid = node.lastChlid;
	parent = node.parent;
	_attribs = node._attribs;
	for (const auto& n : node.nodes)
		nodes.emplace_back(n->createCopy(""));
}
