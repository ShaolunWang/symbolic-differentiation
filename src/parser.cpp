#include "parser.h"
#include "fmt/format.h"

// checks whether the next token is the one we expect
bool Parser::check(Token type) {
  if (m_it == m_input.end()) {
    return false;
  }
  std::string tok = *m_it;
  bool result = false;
  switch (type) {
  case Token::VARNAME:
    result = isVarName(tok);
    break;
  case Token::ADD:
    result = (tok == "+");
    break;
  case Token::SUB:
    result = tok == "-";
    break;
  case Token::DIV:
    result = tok == "/";
    break;
  case Token::MUL:
    result = tok == "*";
    break;
  case Token::SQ:
    result = tok == "^";
    break;
  case Token::LBRACKET:
    result = tok == "(";
    break;
  case Token::RBRACKET:
    result = tok == ")";
    break;
  case Token::NUM:
    result = isNum(tok);
    break;
  default:
    break;
  }
  return result;
}

// match if token is the one we expected
bool Parser::match(Token type) {
  if (check(type)) {
    this->m_it = m_input.erase(m_it);
    return true;
  }
  return false;
}

void Parser::parseMain() {
  int idx = 0;
  // get all the output var names
  // for each one of them, we should generate a seperate graph
  // NOTE: it's probably possible to re-use subexpr

  // first we get all the out expr
  while (*m_it != "=") {
    m_outCtx.emplace_back(*m_it);
    m_it = m_input.erase(m_it);
    if (*m_it == ",") {
      m_it = m_input.erase(m_it);
    }
  }
  if (*m_it == "=") {
    m_it = m_input.erase(m_it); // skip over "="
  }
  // now we are at the rhs of the expr list
  // if it's comma, we stop
  while (m_it != m_input.end()) {
    auto x = parsePlusMinusExpr();
    this->m_operationList.emplace_back(x);
    if (m_it == m_input.end() || m_input.empty()) {
      break;
    }
    // erase `,`
    if (*m_it == ",") {
      m_it = m_input.erase(m_it);
    }
  }
}

std::shared_ptr<Operation> Parser::parseExpr() {
  std::shared_ptr<Operation> result;
  if (check(Token::LBRACKET)) {
    match(Token::LBRACKET);
    result = parsePlusMinusExpr();
    match(Token::RBRACKET);
    return result;
  }
  if (check(Token::VARNAME)) {
    result = parseVariable();
    return result;
  }
  return nullptr;
}

std::shared_ptr<Operation> Parser::parseUnary() {
  // unary has a higher precedence than expr
  std::shared_ptr<Operation> lhs = parseExpr();
  if (check(Token::SQ)) {
    match(Token::SQ);
    lhs = std::make_shared<UnaryOp>(std::move(lhs), '^');
    auto it = m_operationCtx.find(lhs->_name);
    if (it == m_operationCtx.end()) {
      m_operationCtx.emplace(lhs->_name, lhs);
    } else {
      lhs = it->second;
    }

    match(Token::NUM);
  }
  return lhs;
}

std::shared_ptr<Operation> Parser::parsePlusMinusExpr() {
  std::shared_ptr<Operation> lhs = parseMulDivExpr();

  // find all add/sub tokens, build them into one operation
  while (check(Token::ADD) || check(Token::SUB)) {
    char tok;
    if (check(Token::ADD)) {
      tok = '+';
      match(Token::ADD);
    } else {
      tok = '-';
      match(Token::SUB);
    };
    std::shared_ptr<Operation> rhs = parseMulDivExpr();
    lhs = std::make_shared<BinaryOp>(std::move(lhs), tok, std::move(rhs));
    auto it = m_operationCtx.find(lhs->_name);
    if (it == m_operationCtx.end()) {
      m_operationCtx.emplace(lhs->_name, lhs);
    } else {
      lhs = it->second;
    }
  }
  return lhs;
}

std::shared_ptr<Operation> Parser::parseMulDivExpr() {
  std::shared_ptr<Operation> lhs = parseUnary();
  // find all mul/div tokens, build them into one operation
  while (check(Token::MUL) || check(Token::DIV)) {
    char tok;
    if (check(Token::MUL)) {
      tok = '*';
      match(Token::MUL);
    } else {
      tok = '/';
      match(Token::DIV);
    };
    std::shared_ptr<Operation> rhs = parseUnary();

    lhs = std::make_shared<BinaryOp>(std::move(lhs), tok, std::move(rhs));
    auto it = m_operationCtx.find(lhs->_name);
    if (it == m_operationCtx.end()) {
      m_operationCtx.emplace(lhs->_name, lhs);
    } else {
      lhs = it->second;
    }
  }
  return lhs;
}
std::shared_ptr<Operation> Parser::parseVariable() {

  std::shared_ptr<Variable> var = std::make_shared<Variable>(*m_it);
  if (std::ranges::find(m_nameCtx, *m_it) == m_nameCtx.end()) {
    throw std::runtime_error(
        fmt::format("Variable {} not found in given set of input", *m_it));
    exit(-1);
  }
  match(Token::VARNAME);
  auto it = m_operationCtx.find(var->_name);
  if (it == m_operationCtx.end()) {
    m_operationCtx.emplace(var->_name, var);
    return var;
  }

  return it->second;
}
std::string Parser::dump() const {
  std::string result;
  for (int i = 0; i < m_operationList.size(); i++) {
    result += fmt::format("{}:\n", m_outCtx[i]);
    result += m_operationList[i]->dump();
    result += "\n";
  }
  return result;
}
std::string Parser::exportGraphviz() {
  std::string result;

  result += "digraph G {\n";
  for (int i = 0; i < m_operationList.size(); i++) {
    result += fmt::format("subgraph tree_{} {}", i, '{');
    result += "\nnode [style=filled];\n";
    result += fmt::format("\nedge [label={},dir=none];\n", m_outCtx[i]);
    result += fmt::format("{} -> ", m_outCtx[i]);
    result += m_operationList[i]->export_graphviz(i);
    result += "\n}\n";
  }
  result += "}\n";
  // also output the file
  return result;
}
std::string Parser::exportRevGraphviz() {

  std::string result;
  result += "digraph G {\n";
  int i = 0;
  for (auto &x : m_revMap) {
    result += fmt::format("subgraph tree_{} {}", i, '{');
    result += "\nnode [style=filled];\n";
    result += fmt::format("\nedge [label={},dir=none];\n", i);
    result += x.second->export_graphviz(i);
    result += "\n}\n";
    i++;
  }
  result += "}\n";
  // also output the file
  return result;
}

///// Rev OPERATIONS
void Parser::genRev() {
  // doing the first batch manually
  for (int i = 0; i < m_operationList.size(); i++) {
    auto var = std::make_shared<Variable>(m_outCtx[i]);
    std::vector<std::shared_ptr<Operation>> op_list{var};
    m_operationList[i]->makeRevOp(op_list);
    m_revOpWorklist.emplace_back(m_operationList[i]);
  }
  // bfs until worklist is cleared

  while (!m_revOpWorklist.empty()) {
    auto it = m_revOpWorklist[0];

    // only put into the Rev result list iff current node does not have a
    // child, i.e. end
    if (!it->hasChild()) {
      auto res = std::make_shared<RevOp>(it->getRevOps(), it->_name);

      // remove duplicated entries
      auto it = m_revMap.find(res->_name);
      if (it == m_revMap.end()) {
        m_revMap.emplace(res->_name, res);
      }
    } else {
      for (auto &x : it->getChilds()) {
        std::vector<std::shared_ptr<Operation>> vec = it->getRevOps();
        x->makeRevOp(vec);
        m_revOpWorklist.emplace_back(x);
      }
    }
    m_revOpWorklist.erase(m_revOpWorklist.begin());
  }
}
std::string Parser::dumpRev() const {
  std::string result;
  for (const auto &x : m_revMap) {
    result += '\n' + x.second->dump();
  }
  return result;
}
