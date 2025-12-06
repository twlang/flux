#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class inst_enum {
  Add,
  Smake,
  Sload,
  Hmake,
  Hload,
  Set, // used to set the value of a variable
  Seq, // sequences dominate what the type is, every other type is disregarded
  Scope,
  Temp,
  Init,
  Ret,
  Mul,
  Div,
  Sub,
  Mod,
  Escope,
  Call,
  Fn,
  If,    // takes bool
  While, // takes bool
  // no For loop, compiler will just create it with a while loop and then add
  // the iterator at the end
  Static, // sets something to data section
  Array,  // takes string seperated by commas
  Type,   // defines the type of last instruction
};

class variable {
public:
  std::string &name;
  std::string &arg;
  std::string &type;
  int scope;
  bool depended_on;
  int to_destroy;
  std::vector<variable> frees;

  variable(std::string &name, std::string &arg, std::string &type, int scope,
           bool depended_on, int to_destroy, std::vector<variable> &frees)
      : name(name), arg(arg), type(type), scope(scope),
        depended_on(depended_on), to_destroy(to_destroy), frees(frees) {}

  variable(std::string &name, std::string &arg, std::string type, int scope,
           bool depended_on, int to_destroy)

      : name(name), arg(arg), type(type), scope(scope),
        depended_on(depended_on), to_destroy(to_destroy) {}
};

class instruction {
public:
  inst_enum inst;
  variable arg;
  instruction(const inst_enum inst, variable arg) : inst(inst), arg(arg) {}
};

class program {
protected:
  std::vector<std::unique_ptr<instruction>> &instructions;
  std::vector<std::unique_ptr<variable>> variables;

public:
  program(std::vector<std::unique_ptr<instruction>> &instructions)
      : instructions(instructions) {}

  void process() {
    int current_scope = 0;
    for (int i = 0; i < instructions.size(); i++) {
      bool not_first = (i > 0) ? true : false;
      bool not_last = (i != instructions.size() - 1) ? true : false;

      switch (instructions[i]->inst) {
      case (inst_enum::Smake): {
        std::string &name = instructions[i]->arg.name;
        std::string arg;
        if (not_last && instructions[i + 1]->inst == inst_enum::Set)
          arg = std::move(instructions[i + 1]->arg.name);
        else
          arg = "";

        std::unique_ptr<variable> new_var = std::make_unique<variable>(
            name, arg, "UNSET", current_scope, false, 0);
        variables.push_back(std::move(new_var));

        break;
      }

      case (inst_enum::Sload): {
        if (instructions[i]->arg.frees.size() == 0)
          break;
        int j = i;
        while (j > 0 && instructions[j]->inst != inst_enum::Seq) {
          j--;
        }

        instructions[j]->arg.frees.append_range(instructions[i]->arg.frees);
        instructions[i]->arg.depended_on = true;

        break;
      }

      case (inst_enum::Hload): {
        int j = i;
        while (j > 0 && instructions[j]->inst != inst_enum::Seq) {
          j--;
        }

        if (instructions[j]->inst != inst_enum::Seq) {
          std::cout << "Attempted heap-load declared outside of a sequence on "
                       "instruction #"
                    << i << '\n';
          exit(EXIT_FAILURE);
        }

        // instructions[j] argument depends on instructions[i] argument
        // to_destroy is set at -1, they're evaluated for every variable at flux
        // phase to_destroy applies to both stack and heap variables since both
        // can have dependencies

        if (instructions[i]->arg.scope < instructions[j]->arg.scope) {
          if (instructions[i]->arg.frees.size() == 0) {
            std::vector<variable> frees;
            instructions[j]->arg.frees = std::move(frees);
          }

          instructions[j]->arg.frees.push_back(instructions[i]->arg);
          instructions[i]->arg.depended_on = true;
        }

        break;
      }
      }
    }
  }

  void flux() {} // does compiler time travel
  void print_ir() const {}
};

int main() {}
