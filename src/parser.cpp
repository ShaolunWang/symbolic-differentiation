#include "parser.h"
#include "fmt/format.h"

// checks whether the next token is the one we expect
bool Parser::check(Token type) {
  if (it == input.end()) {
    return false;
  }
  std::string tok = *it;
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
    this->it = input.erase(it);
    return true;
  }
  return false;
}

void Parser::parse_main() {
  int idx = 0;
  // get all the output var names
  // for each one of them, we should generate a seperate graph
  // NOTE: it's probably possible to re-use subexpr

  // first we get all the out expr
  while (*it != "=") {
    out_ctx.emplace_back(*it);
    it = input.erase(it);
    if (*it == ",") {
      it = input.erase(it);
    }
  }
  if (*it == "=") {
    it = input.erase(it); // skip over "="
  }
  // now we are at the rhs of the expr list
  // if it's comma, we stop
  while (it != input.end()) {
    auto x = parse_plus_minus_expr();
    this->operation_list.emplace_back(x);
    if (it == input.end() || input.empty()) {
      break;
    }
    // erase `,`
    if (*it == ",") {
      it = input.erase(it);
    }
  }
}

std::shared_ptr<Operation> Parser::parse_expr() {
  std::shared_ptr<Operation> result;
  if (check(Token::LBRACKET)) {
    match(Token::LBRACKET);
    result = parse_plus_minus_expr();
    match(Token::RBRACKET);
    return result;
  }
  if (check(Token::VARNAME)) {
    result = parse_variable();
    return result;
  }
  return nullptr;
}

std::shared_ptr<Operation> Parser::parse_unary() {
  // unary has a higher precedence than expr
  std::shared_ptr<Operation> lhs = parse_expr();
  if (check(Token::SQ)) {
    match(Token::SQ);
    lhs = std::make_shared<UnaryOp>(std::move(lhs), '^');
    auto it = operation_ctx.find(lhs->_name);
    if (it == operation_ctx.end()) {
      operation_ctx.emplace(lhs->_name, lhs);
    } else {
      lhs = it->second;
    }

    match(Token::NUM);
  }
  return lhs;
}

std::shared_ptr<Operation> Parser::parse_plus_minus_expr() {
  std::shared_ptr<Operation> lhs = parse_mul_div_expr();

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
    std::shared_ptr<Operation> rhs = parse_mul_div_expr();
    lhs = std::make_shared<BinaryOp>(std::move(lhs), tok, std::move(rhs));
    auto it = operation_ctx.find(lhs->_name);
    if (it == operation_ctx.end()) {
      operation_ctx.emplace(lhs->_name, lhs);
    } else {
      lhs = it->second;
    }
  }
  return lhs;
}

std::shared_ptr<Operation> Parser::parse_mul_div_expr() {
  std::shared_ptr<Operation> lhs = parse_unary();
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
    std::shared_ptr<Operation> rhs = parse_unary();

    lhs = std::make_shared<BinaryOp>(std::move(lhs), tok, std::move(rhs));
    auto it = operation_ctx.find(lhs->_name);
    if (it == operation_ctx.end()) {
      operation_ctx.emplace(lhs->_name, lhs);
    } else {
      lhs = it->second;
    }
  }
  return lhs;
}
std::shared_ptr<Operation> Parser::parse_variable() {

  std::shared_ptr<Variable> var = std::make_shared<Variable>(*it);
  if (std::ranges::find(name_ctx, *it) == name_ctx.end()) {
    throw std::runtime_error(
        fmt::format("Variable {} not found in given set of input", *it));
    exit(-1);
  }
  match(Token::VARNAME);
  auto it = operation_ctx.find(var->_name);
  if (it == operation_ctx.end()) {
    operation_ctx.emplace(var->_name, var);
    return var;
  }

  return it->second;
}
std::string Parser::dump() const {
  std::string result;
  for (int i = 0; i < operation_list.size(); i++) {
    result += fmt::format("{}:\n", out_ctx[i]);
    result += operation_list[i]->dump();
    result += "\n";
  }
  return result;
}
std::string Parser::export_graphviz() {
  std::string result;
  result += "digraph G {\n";
  for (int i = 0; i < operation_list.size(); i++) {
    result += fmt::format("subgraph tree_{} {}", i, '{');
    result += "\nnode [style=filled];\n";
    result += fmt::format("\nedge [label={},dir=none];\n", out_ctx[i]);
    result += fmt::format("{} -> ", out_ctx[i]);
    result += operation_list[i]->export_graphviz(i);
    result += "\n}\n";
  }
  result += "}\n";
  // also output the file
  return result;
}
std::string Parser::export_rev_graphviz() {

  std::string result;
  result += "digraph G {\n";
  int i = 0;
  for (auto &x : rev_map) {
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
void Parser::gen_rev() {
  // doing the first batch manually
  for (int i = 0; i < operation_list.size(); i++) {
    auto var = std::make_shared<Variable>(out_ctx[i]);
    std::vector<std::shared_ptr<Operation>> op_list{var};
    operation_list[i]->makeRevOp(op_list);
    revOp_worklist.emplace_back(operation_list[i]);
  }
  // bfs until worklist is cleared

  while (!revOp_worklist.empty()) {
    auto it = revOp_worklist[0];

    // only put into the Rev result list iff current node does not have a
    // child, i.e. end
    if (!it->hasChild()) {
      auto res = std::make_shared<RevOp>(it->getRevOps(), it->_name);

      // remove duplicated entries
      auto it = rev_map.find(res->_name);
      if (it == rev_map.end()) {
        rev_map.emplace(res->_name, res);
      }
    } else {
      for (auto &x : it->getChilds()) {
        std::vector<std::shared_ptr<Operation>> vec = it->getRevOps();
        x->makeRevOp(vec);
        revOp_worklist.emplace_back(x);
      }
    }
    revOp_worklist.erase(revOp_worklist.begin());
  }
}
std::string Parser::dump_rev() const {
  std::string result;
  for (const auto &x : rev_map) {
    result += '\n' + x.second->dump();
  }
  return result;
}
