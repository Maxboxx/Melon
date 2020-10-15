#pragma once

#include "Boxx/String.h"
#include "Boxx/List.h"
#include "Boxx/Math.h"

#include "Kiwi.h"

namespace Kiwi {
	enum class x86_64Syntax : Boxx::UByte {
		Intel,
		ATnT
	};

	class x86_64Converter : public Converter {
	protected:
		x86_64Syntax syntax;
		Boxx::UInt labelId = 0;
		Boxx::String labelStart = "_L";

	public:
		x86_64Converter(const x86_64Syntax syntax) : Converter() {
			this->syntax = syntax;
		}

		x86_64Converter(const x86_64Syntax syntax, const Boxx::Logger& logger) : Converter(logger) {
			this->syntax = syntax;
		}

		~x86_64Converter() {}

		virtual void ConvertToFile(Boxx::FileWriter& file, const Boxx::List<Instruction>& instructions) {
			Boxx::List<Instruction> converted = ConvertInstructions(instructions);

			switch (syntax) {
				case x86_64Syntax::Intel: {
					file.Write(".code\nmain PROC\n");
					break;
				}

				case x86_64Syntax::ATnT: {
					file.Write(".data\n.text\n.global main\nmain:\n");
					break;
				}
			}

			for (const Instruction& inst : converted) {
				file.Write(ConvertInstruction(inst));
			}

			switch (syntax) {
				case x86_64Syntax::Intel: {
					file.Write("main ENDP\nEND\n");
					break;
				}
			}
		}

		Boxx::String ATnTSize(const Boxx::UByte size) {
			switch (size) {
				case 8: return "q";
				case 4: return "l";
				case 2: return "w";
				case 1: return "b";
				default: {
					logger.Error("undefined size");
					return " undefined size";
				}
			}
		}

		Boxx::String GetSizeName(const Boxx::String& name, const Boxx::UByte size) {
			switch (syntax) {
				case x86_64Syntax::Intel: return name;
				case x86_64Syntax::ATnT:  return name + ATnTSize(size);
			}

			logger.Error("undefined syntax");
			return name;
		}

		static Register GetARegister() {
			return Register("a");
		}

		static Register GetCRegister() {
			return Register("c");
		}

		static Register GetDRegister() {
			return Register("d");
		}

		static bool IsReservedRegister(const Register& reg) {
			return reg == GetARegister() || reg == GetDRegister() || reg == GetCRegister();
		}

		static bool IsReservedRegister(const Argument& arg) {
			return arg.type == ArgumentType::Register && IsReservedRegister(arg.reg);
		}

		virtual Boxx::String ConvertInstruction(const Instruction& instruction) {
			Boxx::String inst = instruction.instructionName;

			if (instruction.type != InstructionType::Label && instruction.type != InstructionType::Function) {
				inst = "\t" + inst;
			}
			else {
				inst += ":";
			}

			switch (syntax) {
				case x86_64Syntax::Intel: {
					for (Boxx::UInt i = 0; i < instruction.arguments.Size(); i++) {
						if (i != 0) inst += ",";
						inst += " " + ConvertArgument(instruction.arguments[i], instruction.sizes[i]);
					}

					break;
				}

				case x86_64Syntax::ATnT: {
					for (Boxx::Int i = instruction.arguments.Size() - 1; i >= 0; i--) {
						if (i < instruction.arguments.Size() - 1) inst += ",";
						inst += " " + ConvertArgument(instruction.arguments[i], instruction.sizes[i]);
					}

					break;
				}
			}

			return inst + "\n";
		}

		Boxx::String GetRegisterName8(const Register& reg) {
			switch (reg.index) {
				case 0:  return "rbx";
				case 1:  return "rsi";
				case 2:  return "rdi";
				case 3:  return "r8";
				case 4:  return "r9";
				case 5:  return "r10";
				case 6:  return "r11";
				case 7:  return "r12";
				case 8:  return "r13";
				case 9:  return "r14";
				case 10: return "r15";
			}

			logger.Error("undefined qword register");
			return "undefined qword register";
		}

		Boxx::String GetRegisterName4(const Register& reg) {
			switch (reg.index) {
				case 0:  return "ebx";
				case 1:  return "esi";
				case 2:  return "edi";
				case 3:  return "r8d";
				case 4:  return "r9d";
				case 5:  return "r10d";
				case 6:  return "r11d";
				case 7:  return "r12d";
				case 8:  return "r13d";
				case 9:  return "r14d";
				case 10: return "r15d";
			}

			logger.Error("undefined dword register");
			return "undefined dword register";
		}

		Boxx::String GetRegisterName2(const Register& reg) {
			switch (reg.index) {
				case 0:  return "bx";
				case 1:  return "si";
				case 2:  return "di";
				case 3:  return "r8w";
				case 4:  return "r9w";
				case 5:  return "r10w";
				case 6:  return "r11w";
				case 7:  return "r12w";
				case 8:  return "r13w";
				case 9:  return "r14w";
				case 10: return "r15w";
			}

			logger.Error("undefined word register");
			return "undefined word register";
		}

		Boxx::String GetRegisterName1(const Register& reg) {
			switch (reg.index) {
				case 0:  return "bl";
				case 1:  return "sil";
				case 2:  return "dil";
				case 3:  return "r8b";
				case 4:  return "r9b";
				case 5:  return "r10b";
				case 6:  return "r11b";
				case 7:  return "r12b";
				case 8:  return "r13b";
				case 9:  return "r14b";
				case 10: return "r15b";
			}

			logger.Error("undefined byte register");
			return "undefined byte register";
		}

		Boxx::String GetStackRegisterName(const Boxx::UByte size) {
			switch (size) {
				case 8: return "rsp";
				case 4: return "esp";
				case 2: return "sp";
				case 1: return "spl";
			}

			logger.Error("undefined stack register");
			return "undefined stack register";
		}

		Boxx::String GetFrameRegisterName(const Boxx::UByte size) {
			switch (size) {
				case 8: return "rbp";
				case 4: return "ebp";
				case 2: return "bp";
				case 1: return "bpl";
			}

			logger.Error("undefined frame register");
			return "undefined frame register";
		}

		Boxx::String GetReservedRegisterName(const Register& reg, const Boxx::UByte size) {
			if (reg.name == "a") {
				switch (size) {
					case 8: return "rax";
					case 4: return "eax";
					case 2: return "ax";
					case 1: return "al";
				}
			}
			else if (reg.name == "d") {
				switch (size) {
					case 8: return "rdx";
					case 4: return "edx";
					case 2: return "dx";
					case 1: return "dl";
				}
			}
			else if (reg.name == "c") {
				switch (size) {
					case 8: return "rcx";
					case 4: return "ecx";
					case 2: return "cx";
					case 1: return "cl";
				}
			}
			else {
				logger.Error("undefined reserved register");
				return "undefined reserved register";
			}
		}

		Boxx::String GetRegisterName(const Register& reg, const Boxx::UByte size) {
			if (reg.type == RegisterType::Register) {
				switch (size) {
					case 8: return GetRegisterName8(reg);
					case 4: return GetRegisterName4(reg);
					case 2: return GetRegisterName2(reg);
					case 1: return GetRegisterName1(reg);
				}
			}
			else if (reg.type == RegisterType::Stack) {
				return GetStackRegisterName(size);
			}
			else if (reg.type == RegisterType::Frame) {
				return GetFrameRegisterName(size);
			}
			else if (reg.type == RegisterType::Name) {
				return GetReservedRegisterName(reg, size);
			}

			logger.Error("undefined register");
			return "undefined register";
		}

		virtual Boxx::String ConvertRegister(const Register& reg, const Boxx::UByte size) override {
			switch (syntax) {
				case x86_64Syntax::Intel: return GetRegisterName(reg, size);
				case x86_64Syntax::ATnT:  return "%" + GetRegisterName(reg, size);
			}

			logger.Error("undefined syntax");
			return "undefined syntax";
		}

		virtual Boxx::String ConvertMemoryLocation(const MemoryLocation& stackValue, const Boxx::UByte size) override {
			if (syntax == x86_64Syntax::Intel) {
				Boxx::String sizeStr = "";

				switch (size) {
					case 8: sizeStr = "QWORD PTR "; break;
					case 4: sizeStr = "DWORD PTR "; break;
					case 2: sizeStr = "WORD PTR "; break;
					case 1: sizeStr = "BYTE PTR "; break;
				}

				if (stackValue.offset > 0) {
					return sizeStr + "[" + GetRegisterName(stackValue.reg, 8) + " + " + (Boxx::Int)stackValue.offset + "]";
				}
				else if (stackValue.offset < 0) {
					return sizeStr + "[" + GetRegisterName(stackValue.reg, 8) + " - " + abs((Boxx::Int)stackValue.offset) + "]";
				}
				else {
					return sizeStr + "[" + GetRegisterName(stackValue.reg, 8) + "]";
				}
			}
			else if (syntax == x86_64Syntax::ATnT) {
				if (stackValue.offset != 0) return Boxx::String::ToString((Boxx::Int)stackValue.offset) + "(%" + GetRegisterName(stackValue.reg, 8) + ")";
				return "(%" + GetRegisterName(stackValue.reg, 8) + ")";
			}

			logger.Error("undefined stack");
			return "undefined stack";
		}

		virtual Boxx::String ConvertNumber(const Boxx::Long number, const Boxx::UByte size) override {
			Boxx::Long num = number;

			switch (size) {
				case 8: num &= Boxx::Math::ULongMax(); break;
				case 4: num &= Boxx::Math::UIntMax(); break;
				case 2: num &= Boxx::Math::UShortMax(); break;
				case 1: num &= Boxx::Math::UByteMax(); break;
			}

			switch (syntax) {
				case x86_64Syntax::Intel: return Boxx::String::ToString((Boxx::ULong)num);
				case x86_64Syntax::ATnT:  return "$" + Boxx::String::ToString((Boxx::ULong)num);
			}

			logger.Error("undefined syntax");
			return "undefined syntax";
		}

	private:

		Boxx::List<Instruction> ConvertInstructions(const Boxx::List<Instruction>& instructions) {
			Boxx::List<Instruction> converted = Boxx::List<Instruction>(instructions.Size());

			for (const Instruction& instruction : instructions) {
				Instruction inst = instruction.Copy();

				switch (inst.type) {
					case InstructionType::Mov:
					case InstructionType::Adr: {
						ConvertMov(converted, inst);
						break;
					}

					case InstructionType::Add:
					case InstructionType::Sub:
					case InstructionType::Mul:
					case InstructionType::Div:
					case InstructionType::Mod:
					case InstructionType::Or:
					case InstructionType::And:
					case InstructionType::Xor:
					case InstructionType::Nor:
					case InstructionType::Nand:
					case InstructionType::Xnor:
					case InstructionType::ShL: 
					case InstructionType::ShR: {
						ConvertBinaryOperator(converted, inst);
						break;
					}

					case InstructionType::Neg:
					case InstructionType::Not: {
						ConvertUnaryOperator(converted, inst);
						break;
					}

					case InstructionType::Eq:
					case InstructionType::Ne:
					case InstructionType::Lt:
					case InstructionType::Gt:
					case InstructionType::Le:
					case InstructionType::Ge: {
						ConvertComp(converted, inst);
						break;
					}

					case InstructionType::Label:
					case InstructionType::Function: {
						converted.Add(inst.Copy());
						break;
					}

					case InstructionType::Call: {
						inst.type = InstructionType::Custom;
						inst.instructionName = "call";
						converted.Add(inst);
						break;
					}

					case InstructionType::Jmp: {
						inst.type = InstructionType::Custom;
						inst.instructionName = "jmp";
						converted.Add(inst);
						break;
					}

					case InstructionType::Ret: {
						inst.type = InstructionType::Custom;
						inst.instructionName = "ret";
						converted.Add(inst);
						break;
					}

					case InstructionType::Exit: {
						logger.Warning("Exit instruction was converted to 'ret'");

						inst.type = InstructionType::Custom;
						inst.instructionName = "ret";
						inst.arguments.Clear();
						converted.Add(inst);
						break;
					}

					case InstructionType::Push:
					case InstructionType::Pop: {
						ConvertPushPop(converted, inst);
						break;
					}

					default: {
						Instruction in = inst.Copy();
						in.instructionName = "undefined";
						logger.Error("undefined instruction");
						converted.Add(in);
						break;
					}
				}
			}

			return converted;
		}

		void ConvertNumber(Argument& arg, const Boxx::UByte size, const bool sign) {
			Boxx::ULong n = arg.number;

			switch (size) {
				case 4: n &= Boxx::Math::UIntMax(); break;
				case 2: n &= Boxx::Math::UShortMax(); break;
				case 1: n &= Boxx::Math::UByteMax(); break;
			}

			if (sign) {
				switch (size) {
					case 4: {
						if (n & 0x80000000) {
							n |= Boxx::Math::ULongMax() ^ (Boxx::ULong)Boxx::Math::UIntMax();
						}

						break;
					}

					case 2: {
						if (n & 0x8000) {
							n |= Boxx::Math::ULongMax() ^ (Boxx::ULong)Boxx::Math::UShortMax();
						}

						break;
					}

					case 1: {
						if (n & 0x80) {
							n |= Boxx::Math::ULongMax() ^ (Boxx::ULong)Boxx::Math::UByteMax();
						}

						break;
					}
				}
			}

			arg.number = n;
		}

		///T Convert Mov instruction
		/// Converts a move instruction with sign or zero extension if operands are different sizes
		///W The upper half of an 8 byte reserved register has to be full of zeros before calling this function in order to zero extend from a 4 byte reserved register to the 8 byte version or an 8 byte memory location
		void ConvertMov(Boxx::List<Instruction>& instructions, const Instruction& inst, const Register& tempReg = GetARegister()) {
			Instruction instruction = inst.Copy();

			if (instruction.arguments[1].type == ArgumentType::Number) {
				ConvertNumber(instruction.arguments[1], instruction.sizes[1], instruction.signs[1]);
				instruction.sizes[1] = instruction.sizes[0];
			}

			Boxx::List<Instruction> insts = ExtendMov(instruction, tempReg);

			if (insts.Size() == 1) {
				Instruction mov = insts[0];

				if (instruction.arguments[0].type == ArgumentType::Memory && instruction.arguments[1].type == ArgumentType::Memory) {
					Instruction mov1 = mov.Copy();
					Instruction mov2 = mov.Copy();
				
					mov2.signs[1] = mov2.signs[0];
					mov2.sizes[1] = mov2.sizes[0];
					mov2.instructionName = GetSizeName("mov", mov2.sizes[0]);

					mov1.arguments[0] = tempReg;
					mov2.arguments[1] = tempReg;

					instructions.Add(mov1);
					mov = mov2;
				}

				instructions.Add(mov);
			}
			else for (const Instruction& in : insts) {
				instructions.Add(in);
			}
		}

		Boxx::List<Instruction> Extend4To8Signed(const Instruction& inst, const Register& reg) {
			bool isARegister = inst.arguments[0] == GetARegister();
			Instruction mov = inst;
			Boxx::List<Instruction> insts;

			Instruction mov2 = mov.Copy();
			mov.sizes[0] = 4;
			mov.arguments[0] = GetARegister();

			if (inst.type == InstructionType::Adr) {
				mov.instructionName = GetSizeName("lea", mov.sizes[0]);
			}
			else {
				mov.instructionName = GetSizeName("mov", mov.sizes[0]);
			}

			mov.type = InstructionType::Custom;

			Instruction convert;

			switch (syntax) {
				case x86_64Syntax::Intel: convert = Instruction("cdqe"); break;
				case x86_64Syntax::ATnT:  convert = Instruction("cltq"); break;
			}

			mov2.sizes[1] = 8;
			mov2.arguments[1] = GetARegister();

			mov2.instructionName = GetSizeName("mov", mov2.sizes[0]);
			mov2.type = InstructionType::Custom;

			if (inst.type != InstructionType::Mov || mov.arguments[0] != mov.arguments[1]) {
				insts.Add(mov);
			}

			insts.Add(convert);

			if (!isARegister) {
				insts.Add(mov2);
			}

			return insts;
		}

		Boxx::List<Instruction> Extend4To8Unsigned(const Instruction& inst, const Register& reg) {
			Boxx::List<Instruction> insts;

			if (inst.arguments[0].type == ArgumentType::Register && inst.arguments[1].type == ArgumentType::Register && inst.arguments[0] == inst.arguments[1]) {
				return insts;
			}
			else if (inst.arguments[0].type == ArgumentType::Register) {
				Instruction mov = inst.Copy();
				mov.type = InstructionType::Custom;
				mov.instructionName = GetSizeName("mov", 4);
				mov.sizes[0] = 4;

				insts.Add(mov);
				return insts;
			}

			Instruction mov = inst;

			Instruction mov2 = mov.Copy();
			mov.sizes[0] = 4;
			mov.arguments[0] = reg;

			if (inst.type == InstructionType::Adr) {
				mov.instructionName = GetSizeName("lea", mov.sizes[0]);
			}
			else {
				mov.instructionName = GetSizeName("mov", mov.sizes[0]);
			}

			mov.type = InstructionType::Custom;

			mov2.sizes[1] = 8;
			mov2.arguments[1] = reg;

			mov2.instructionName = GetSizeName("mov", mov2.sizes[0]);
			mov2.type = InstructionType::Custom;

			if (inst.type != InstructionType::Mov || mov.arguments[0] != mov.arguments[1]) {
				insts.Add(mov);
			}

			if (mov2.arguments[0] != mov2.arguments[1]) {
				insts.Add(mov2);
			}

			return insts;
		}

		Boxx::List<Instruction> Extend4To8(const Instruction& inst, const Register& reg) {
			if (inst.signs[1]) {
				return Extend4To8Signed(inst, reg);
			}
			else {
				return Extend4To8Unsigned(inst, reg);
			}
		}

		Boxx::List<Instruction> NoExtendMov(const Instruction& inst, const Register& reg) {
			Instruction mov = inst.Copy();
			Boxx::List<Instruction> insts;

			if (inst.type == InstructionType::Adr) {
				mov.instructionName = GetSizeName("lea", mov.sizes[0]);
			}
			else {
				mov.instructionName = GetSizeName("mov", mov.sizes[0]);
			}

			mov.type = InstructionType::Custom;
			mov.sizes[1] = mov.sizes[0];

			if (mov.arguments[0].type != ArgumentType::Register && mov.arguments[1].type == ArgumentType::Number && mov.sizes[1] == 8) {
				Instruction mov2 = mov.Copy();

				mov.arguments[0] = reg;
				insts.Add(mov);

				mov2.arguments[0] = inst.arguments[0];
				mov2.arguments[1] = reg;
				insts.Add(mov2);
			}
			else {
				insts.Add(mov);
			}

			return insts;
		}

		Boxx::List<Instruction> PlainExtendMov(const Instruction& inst) {
			Instruction mov = inst;
			Boxx::List<Instruction> insts;
			Boxx::String name;

			switch (syntax) {
				case x86_64Syntax::Intel: {
					name = mov.signs[1] ? "movsx" : "movzx";
					break;
				}

				case x86_64Syntax::ATnT: {
					const Boxx::String sign = mov.signs[1] ?  "s" : "z";
					name = "mov" + sign + ATnTSize(mov.sizes[1]) + ATnTSize(mov.sizes[0]);
					break;
				}
			}

			mov.instructionName = name;
			mov.type = InstructionType::Custom;
			insts.Add(mov);

			return insts;
		}

		Boxx::List<Instruction> ExtendMov(const Instruction& inst, const Register& tempReg = GetARegister()) {
			const Register reg = IsReservedRegister(inst.arguments[0]) ? inst.arguments[0].reg : tempReg;

			if (inst.sizes[0] == 8 && inst.sizes[1] == 4) {
				return Extend4To8(inst, reg);
			}
			else if (inst.sizes[0] <= inst.sizes[1]) {
				return NoExtendMov(inst, reg);
			}
			else {
				return PlainExtendMov(inst);
			}
		}

		void ConvertBinaryOperator(Boxx::List<Instruction>& instructions, const Instruction& instruction) {
			BinaryInfo info;
			info.instName = "";
			info.addBitNot = false;
			info.isSwapable = false;
			info.firstA = false;

			switch (instruction.type) {
				case InstructionType::Add:  info.instName = "add"; info.isSwapable = true; break;
				case InstructionType::Sub:  info.instName = "sub"; break;
				case InstructionType::Mul:  info.instName = "imul"; info.firstA = true; info.isSwapable = true; break;
				case InstructionType::Div:  info.instName = "div"; break;
				case InstructionType::Mod:  info.instName = "mod"; break;
				case InstructionType::Or:   info.instName =  "or"; info.isSwapable = true; break;
				case InstructionType::And:  info.instName = "and"; info.isSwapable = true; break;
				case InstructionType::Xor:  info.instName = "xor"; info.isSwapable = true; break;
				case InstructionType::Nor:  info.instName =  "or"; info.addBitNot = true; info.isSwapable = true; break;
				case InstructionType::Nand: info.instName = "and"; info.addBitNot = true; info.isSwapable = true; break;
				case InstructionType::Xnor: info.instName = "xor"; info.addBitNot = true; info.isSwapable = true; break;
				case InstructionType::ShL:  info.instName = instruction.signs[0] ? "sal" : "shl"; break;
				case InstructionType::ShR:  info.instName = instruction.signs[0] ? "sar" : "shr"; break;
			}

			if (instruction.arguments.Size() == 3) {
				ConvertBinaryThree(instructions, instruction, info);
			}
			else {
				ConvertBinaryTwo(instructions, instruction, info);
			} 
		}

		struct BinaryInfo {
			Boxx::String instName;
			bool addBitNot;
			bool isSwapable;
			bool firstA;

			~BinaryInfo() {};
		};

		void MoveBinaryThreeResult(Boxx::List<Instruction>& instructions, const Instruction& instruction, Argument arg, Boxx::UByte size, const bool signedResult, const bool firstA) {
			if (instruction.type == InstructionType::Mod) {
				arg = GetDRegister();
			}

			if (instruction.arguments[2].type != ArgumentType::Register) {
				bool sign = signedResult;

				if (size < instruction.sizes[2] && instruction.arguments[2].type == ArgumentType::Memory && !(size == 4 && instruction.sizes[2] == 8)) {
					Instruction mov2 = Instruction(InstructionType::Mov);
					mov2.arguments.Add(arg);
					mov2.arguments.Add(arg);
					mov2.sizes[0] = instruction.sizes[2];
					mov2.sizes[1] = size;
					mov2.signs[0] = instruction.signs[2];
					mov2.signs[1] = sign;
					ConvertMov(instructions, mov2);

					sign = instruction.signs[2];
					size = instruction.sizes[2];
				}

				Instruction mov3 = Instruction(InstructionType::Mov);
				mov3.arguments.Add(instruction.arguments[2]);
				mov3.arguments.Add(arg);
				mov3.sizes[0] = instruction.sizes[2];
				mov3.sizes[1] = size;
				mov3.signs[0] = instruction.signs[2];
				mov3.signs[1] = sign;
				ConvertMov(instructions, mov3);
			}
			else if (firstA || instruction.sizes[2] > size) {
				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(instruction.arguments[2]);
				mov.arguments.Add(arg);
				mov.sizes[0] = instruction.sizes[2];
				mov.sizes[1] = size;
				mov.signs[0] = instruction.signs[2];
				mov.signs[1] = signedResult;
				ConvertMov(instructions, mov);
			}
		}

		void MoveBinaryThreeOperands(Boxx::List<Instruction>& instructions, const Instruction& instruction, const Argument arg, Argument& arg2, const Boxx::UByte size, Boxx::UByte& arg2Size, const bool firstA) {
			if (size == 8 && ((instruction.sizes[1] == 4 && instruction.signs[1]) || arg2.type == ArgumentType::Number)) {
				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(firstA ? GetCRegister() : GetARegister());
				mov.arguments.Add(arg2);
				mov.sizes[0] = 8;
				mov.sizes[1] = arg2.type == ArgumentType::Number ? 8 : instruction.sizes[1];
				mov.signs[0] = instruction.signs[1];
				mov.signs[1] = instruction.signs[1];
				ConvertMov(instructions, mov);

				arg2 = firstA ? GetCRegister() : GetARegister();
				arg2Size = 8;
			}

			Instruction mov1 = Instruction(InstructionType::Mov);
			mov1.arguments.Add(arg);
			mov1.arguments.Add(instruction.arguments[0]);
			mov1.sizes[0] = size;
			mov1.sizes[1] = instruction.sizes[0];
			mov1.signs[0] = instruction.signs[0];
			mov1.signs[1] = instruction.signs[0];
			ConvertMov(instructions, mov1);
		}

		void ConvertBinaryThree(Boxx::List<Instruction>& instructions, const Instruction& instruction, const BinaryInfo& info) {
			bool minSize2 = instruction.type == InstructionType::Mul ||
				instruction.type == InstructionType::Div ||
				instruction.type == InstructionType::Mod;

			Argument arg = GetARegister();
			Boxx::UByte size = Boxx::Math::Max(instruction.sizes[0], instruction.sizes[1]);
			bool signedResult = instruction.signs[0] || instruction.signs[1];

			bool firstA = minSize2 || (size < instruction.GetLargestSize() && (signedResult || size != 4)) || (
				instruction.arguments[1].type == ArgumentType::Number && 
				instruction.arguments[2].type != ArgumentType::Register
			);

			if (minSize2 && size < 2) size = 2;

			if (!firstA && instruction.arguments[2].type == ArgumentType::Register) {
				arg = instruction.arguments[2];
			}

			Argument arg2 = instruction.arguments[1];
			Boxx::UByte arg2Size = instruction.sizes[1];

			MoveBinaryThreeOperands(instructions, instruction, arg, arg2, size, arg2Size, firstA);

			Instruction inst = instruction.Copy();
			inst.arguments[0] = arg;
			inst.arguments[1] = arg2;
			inst.sizes[0] = size;
			inst.sizes[1] = arg2Size;
			inst.arguments.RemoveAt(2);
			ConvertBinaryTwo(instructions, inst, info);

			MoveBinaryThreeResult(instructions, instruction, arg, size, signedResult, firstA);
		}

		void ConvertBinaryTwoMov(Boxx::List<Instruction>& instructions, const Instruction& inst, const BinaryInfo& info, const bool useD, const bool swap) {
			Argument arg1 = GetARegister();

			Instruction mov1 = Instruction(InstructionType::Mov);
			mov1.arguments.Add(arg1);
			mov1.arguments.Add(inst.arguments[1]);
			mov1.sizes[0] = inst.sizes[0];
			mov1.sizes[1] = inst.sizes[1];
			mov1.signs[0] = inst.signs[1];
			mov1.signs[1] = inst.signs[1];
			ConvertMov(instructions, mov1);

			Argument arg2 = useD ? GetDRegister() : inst.arguments[0];

			if (useD) {
				Instruction mov2 = Instruction(InstructionType::Mov);
				mov2.arguments.Add(arg2);
				mov2.arguments.Add(inst.arguments[0]);
				mov2.sizes[0] = inst.sizes[0];
				mov2.sizes[1] = inst.sizes[0];
				mov2.signs[0] = inst.signs[0];
				mov2.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov2);
			}

			Instruction in = inst.Copy();
			in.type = InstructionType::Custom;
			in.instructionName = GetSizeName(info.instName, inst.sizes[0]);

			if (swap) {
				in.arguments[0] = arg1;
				in.arguments[1] = arg2;
			}
			else {
				in.arguments[0] = arg2;
				in.arguments[1] = arg1;
			}

			in.sizes[1] = inst.sizes[0];
			instructions.Add(in);

			if (info.addBitNot) {
				Instruction bitNot = Instruction(InstructionType::Custom);
				bitNot.instructionName = GetSizeName("not", inst.sizes[0]);

				if (swap) {
					bitNot.arguments.Add(arg1);
				}
				else {
					bitNot.arguments.Add(arg2);
				}

				bitNot.sizes[0] = inst.sizes[0];
				bitNot.signs[0] = inst.signs[0];
				instructions.Add(bitNot);
			}

			if (useD || swap) {
				Instruction mov3 = Instruction(InstructionType::Mov);
				mov3.arguments.Add(inst.arguments[0]);

				if (swap) {
					mov3.arguments.Add(GetARegister());
				}
				else {
					mov3.arguments.Add(arg1);
				}

				mov3.sizes[0] = inst.sizes[0];
				mov3.sizes[1] = inst.sizes[0];
				mov3.signs[0] = inst.signs[0];
				mov3.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov3);
			}
		}

		void ConvertBinaryTwoPlain(Boxx::List<Instruction>& instructions, const Instruction& inst, const BinaryInfo& info, const bool useD) {
			Argument arg1 = useD ? GetDRegister() : inst.arguments[0];

			if (useD) {
				Instruction mov1 = Instruction(InstructionType::Mov);
				mov1.arguments.Add(arg1);
				mov1.arguments.Add(inst.arguments[0]);
				mov1.sizes[0] = inst.sizes[0];
				mov1.sizes[1] = inst.sizes[0];
				mov1.signs[0] = inst.signs[0];
				mov1.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov1);
			}

			Instruction in = inst.Copy();
			in.type = InstructionType::Custom;
			in.instructionName = GetSizeName(info.instName, inst.sizes[0]);
			in.arguments[0] = arg1;
			in.sizes[1] = inst.sizes[0];

			if (inst.arguments[1].type == ArgumentType::Number && inst.sizes[1] == 8) {
				Instruction mov1 = Instruction(InstructionType::Mov);
				mov1.arguments.Add(GetARegister());
				mov1.arguments.Add(inst.arguments[1]);
				mov1.sizes[0] = 8;
				mov1.sizes[1] = 8;
				ConvertMov(instructions, mov1);

				in.arguments[1] = GetARegister();
			}

			instructions.Add(in);

			if (info.addBitNot) {
				Instruction bitNot = Instruction(InstructionType::Custom);
				bitNot.instructionName = GetSizeName("not", inst.sizes[0]);
				bitNot.arguments.Add(arg1);
				bitNot.sizes[0] = inst.sizes[0];
				bitNot.signs[1] = inst.signs[0];
				instructions.Add(bitNot);
			}

			if (useD) {
				Instruction mov2 = Instruction(InstructionType::Mov);
				mov2.arguments.Add(inst.arguments[0]);
				mov2.arguments.Add(arg1);
				mov2.sizes[0] = inst.sizes[0];
				mov2.sizes[1] = inst.sizes[0];
				mov2.signs[0] = inst.signs[0];
				mov2.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov2);
			}
		}

		void ConvertBinaryTwoShift(Boxx::List<Instruction>& instructions, const Instruction& instruction, const BinaryInfo& info) {
			Instruction inst = instruction.Copy();

			if (inst.arguments[1].type != ArgumentType::Number && inst.arguments[1] != GetCRegister()) {
				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(GetCRegister());
				mov.arguments.Add(inst.arguments[1]);
				mov.sizes[0] = 1;
				mov.sizes[1] = inst.sizes[1];
				mov.signs[0] = false;
				mov.signs[1] = inst.signs[1];
				ConvertMov(instructions, mov);

				inst.arguments[1] = GetCRegister();
			}

			Instruction shift = Instruction(GetSizeName(info.instName, inst.sizes[0]));
			shift.arguments.Add(inst.arguments[0]);
			shift.arguments.Add(inst.arguments[1]);
			shift.sizes[0] = inst.sizes[0];
			shift.sizes[1] = 1;
			shift.signs[0] = inst.signs[1];
			shift.signs[1] = false;
			instructions.Add(shift);
		}

		void ConvertBinaryTwo(Boxx::List<Instruction>& instructions, const Instruction& instruction, const BinaryInfo& info) {
			Instruction inst = instruction.Copy();

			if (instruction.type == InstructionType::Mul || instruction.type == InstructionType::Div || instruction.type == InstructionType::Mod) {
				ConvertBinaryTwoA(instructions, inst, info);
				return;
			}

			if (inst.arguments[1].type == ArgumentType::Number) {
				ConvertNumber(inst.arguments[1], inst.sizes[1], inst.signs[1]);
				inst.sizes[1] = inst.sizes[0];
			}

			bool useA = inst.arguments[0].type == ArgumentType::Memory || 
				inst.arguments[1].type == ArgumentType::Memory ||
				inst.sizes[0] == 8;

			bool useD = info.addBitNot && inst.arguments[0].type == ArgumentType::Memory;
			bool swap = info.isSwapable && useA && useD && inst.arguments[0].type == ArgumentType::Memory;
			bool doubleStack = inst.arguments[0].type == ArgumentType::Memory && inst.arguments[1].type == ArgumentType::Memory;

			if (swap) useD = false;

			if (inst.type == InstructionType::ShL || inst.type == InstructionType::ShR) {
				ConvertBinaryTwoShift(instructions, inst, info);
			}
			else if (inst.sizes[0] > inst.sizes[1] || (!useD && doubleStack)) {
				ConvertBinaryTwoMov(instructions, inst, info, useD, swap);
			}
			else {
				ConvertBinaryTwoPlain(instructions, inst, info, useD);
			}
		}

		void ExtendAD(Boxx::List<Instruction>& instructions, const Boxx::UByte size, const bool sign) {
			if (sign) {
				Instruction extend = Instruction(InstructionType::Custom);
			
				switch (syntax) {
					case x86_64Syntax::Intel: {
						switch (size) {
							case 8: extend.instructionName = "cqo"; break;
							case 4: extend.instructionName = "cdq"; break;
							case 2: extend.instructionName = "cwd"; break;
							case 1: extend.instructionName = "cbw"; break;
							default: extend.instructionName = "undefined"; logger.Error("undefined AD extend size"); break;
						}

						break;
					}

					case x86_64Syntax::ATnT: {
						switch (size) {
							case 8: extend.instructionName = "cqto"; break;
							case 4: extend.instructionName = "cltd"; break;
							case 2: extend.instructionName = "cwtd"; break;
							case 1: extend.instructionName = "cbtw"; break;
							default: extend.instructionName = "undefined"; logger.Error("undefined AD extend size"); break;
						}

						break;
					}
				}

				instructions.Add(extend);
			}
			else {
				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(GetDRegister());
				mov.arguments.Add(Argument(0));
				mov.sizes[0] = size;
				mov.sizes[1] = size;
				mov.signs[0] = true;
				mov.signs[1] = true;
				ConvertMov(instructions, mov);
			}
		}

		void ConvertBinaryTwoMul(Boxx::List<Instruction>& instructions, const Instruction& inst, const BinaryInfo& info, const Boxx::UByte size, const bool firstA) {
			Argument arg = inst.arguments[1];

			if (inst.sizes[1] < size && inst.arguments[1].type != ArgumentType::Number) {
				arg = GetDRegister();

				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(arg);
				mov.arguments.Add(inst.arguments[1]);
				mov.sizes[0] = size;
				mov.sizes[1] = inst.sizes[1];
				mov.signs[0] = inst.signs[1];
				mov.signs[1] = inst.signs[1];
				ConvertMov(instructions, mov);
			}

			if (!firstA) {
				Instruction mov1 = Instruction(InstructionType::Mov);
				mov1.arguments.Add(GetARegister());
				mov1.arguments.Add(inst.arguments[0]);
				mov1.sizes[0] = size;
				mov1.sizes[1] = inst.sizes[0];
				mov1.signs[0] = inst.signs[0];
				mov1.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov1);
			}

			Instruction in = Instruction(InstructionType::Custom);
			in.instructionName = GetSizeName(info.instName, size);
			in.arguments.Add(GetARegister());
			in.arguments.Add(arg);
			in.sizes[0] = size;
			in.sizes[1] = size;
			in.signs[0] = inst.signs[0];
			in.signs[1] = inst.signs[1];
			instructions.Add(in);

			if (!firstA) {
				Instruction mov2 = Instruction(InstructionType::Mov);
				mov2.arguments.Add(inst.arguments[0]);
				mov2.arguments.Add(GetARegister());
				mov2.sizes[0] = inst.sizes[0];
				mov2.sizes[1] = size;
				mov2.signs[0] = inst.signs[0];
				mov2.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov2);
			}
		}

		void ConvertBinaryTwoDiv(Boxx::List<Instruction>& instructions, const Instruction& inst, const Boxx::UByte size, const bool firstA) {
			Argument arg = inst.arguments[1];
			bool sign = inst.signs[0] || inst.signs[1];

			if (inst.sizes[1] < size || inst.arguments[1].type == ArgumentType::Number) {
				arg = GetCRegister();

				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(arg);
				mov.arguments.Add(inst.arguments[1]);
				mov.sizes[0] = size;
				mov.sizes[1] = inst.sizes[1];
				mov.signs[0] = inst.signs[1];
				mov.signs[1] = inst.signs[1];
				ConvertMov(instructions, mov);
			}

			if (!firstA) {
				Instruction mov1 = Instruction(InstructionType::Mov);
				mov1.arguments.Add(GetARegister());
				mov1.arguments.Add(inst.arguments[0]);
				mov1.sizes[0] = size;
				mov1.sizes[1] = inst.sizes[0];
				mov1.signs[0] = inst.signs[0];
				mov1.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov1);
			}

			ExtendAD(instructions, size, inst.signs[0]);

			Instruction in = Instruction(InstructionType::Custom);
			in.instructionName = GetSizeName(sign ? "idiv" : "div", size);
			in.arguments.Add(arg);
			in.sizes[0] = size;
			in.signs[0] = sign;
			instructions.Add(in);

			if (!firstA) {
				Instruction mov2 = Instruction(InstructionType::Mov);
				mov2.arguments.Add(inst.arguments[0]);
				mov2.arguments.Add(inst.type == InstructionType::Div ? GetARegister() : GetDRegister());
				mov2.sizes[0] = inst.sizes[0];
				mov2.sizes[1] = size;
				mov2.signs[0] = inst.signs[0];
				mov2.signs[1] = sign;
				ConvertMov(instructions, mov2);
			}
		}

		void ConvertBinaryTwoA(Boxx::List<Instruction>& instructions, const Instruction& instruction, const BinaryInfo& info) {
			Instruction inst = instruction.Copy();
			bool firstA = inst.arguments[0] == GetARegister();

			Boxx::UByte size = Boxx::Math::Max(inst.sizes[0], inst.sizes[1]);
			if (size < 2) size = 2;

			if (inst.arguments[1].type == ArgumentType::Number) {
				ConvertNumber(inst.arguments[1], inst.sizes[1], inst.signs[1]);
				inst.sizes[1] = Boxx::Math::Max(inst.sizes[0], (Boxx::UByte)2);
				size = inst.sizes[1];
			}

			if (inst.type == InstructionType::Mul) {
				ConvertBinaryTwoMul(instructions, inst, info, size, firstA);
			}
			else if (inst.type == InstructionType::Div || inst.type == InstructionType::Mod) {
				ConvertBinaryTwoDiv(instructions, inst, size, firstA);
			}
		}

		void ConvertUnaryOperator(Boxx::List<Instruction>& instructions, const Instruction& instruction) {
			Boxx::String instName = "";

			switch (instruction.type) {
				case InstructionType::Neg: instName = "neg"; break;
				case InstructionType::Not: instName = "not"; break;
			}

			if (instruction.arguments.Size() == 1) {
				Instruction inst = instruction.Copy();
				inst.type = InstructionType::Custom;
				inst.instructionName = GetSizeName(instName, instruction.sizes[0]);
				instructions.Add(inst);
			}
			else if (instruction.arguments.Size() == 2) {
				Argument arg = GetARegister();

				if (instruction.arguments[1].type == ArgumentType::Register) {
					arg = instruction.arguments[1];
				}

				Instruction mov1 = Instruction(InstructionType::Mov);
				mov1.arguments.Add(arg);
				mov1.arguments.Add(instruction.arguments[0]);
				mov1.sizes[0] = instruction.sizes[0];
				mov1.sizes[1] = instruction.sizes[0];
				mov1.signs[0] = instruction.signs[0];
				mov1.signs[1] = instruction.signs[0];
				ConvertMov(instructions, mov1);

				Instruction inst = Instruction(GetSizeName(instName, instruction.sizes[0]));
				inst.arguments.Add(arg);
				inst.sizes[0] = instruction.sizes[0];
				inst.signs[0] = instruction.signs[0];
				instructions.Add(inst);

				if (arg != instruction.arguments[1] || instruction.sizes[1] > instruction.sizes[0]) {
					Instruction mov2 = Instruction(InstructionType::Mov);
					mov2.arguments.Add(instruction.arguments[1]);
					mov2.arguments.Add(arg);
					mov2.sizes[0] = instruction.sizes[1];
					mov2.sizes[1] = instruction.sizes[0];
					mov2.signs[0] = instruction.signs[1];
					mov2.signs[1] = instruction.signs[0];
					ConvertMov(instructions, mov2);
				}
			}
		}

		Boxx::String GetCompSet(const InstructionType type, const bool sign) {
			switch (type) {
				case InstructionType::Eq: return "sete";
				case InstructionType::Ne: return "setne";
				case InstructionType::Lt: return sign ? "setl" : "setb";
				case InstructionType::Gt: return sign ? "setg" : "seta";
				case InstructionType::Le: return sign ? "setle" : "setbe";
				case InstructionType::Ge: return sign ? "setge" : "setae";
			}

			logger.Error("undefined comp instruction");
			return "undefined";
		}

		Boxx::String GetCompJmp(const InstructionType type, const bool sign) {
			switch (type) {
				case InstructionType::Eq: return "je";
				case InstructionType::Ne: return "jne";
				case InstructionType::Lt: return sign ? "jl" : "jb";
				case InstructionType::Gt: return sign ? "jg" : "ja";
				case InstructionType::Le: return sign ? "jle" : "jbe";
				case InstructionType::Ge: return sign ? "jge" : "jae";
			}

			logger.Error("undefined comp instruction");
			return "undefined";
		}

		InstructionType CompSwap(const InstructionType type) {
			switch (type) {
				case InstructionType::Eq: return InstructionType::Eq;
				case InstructionType::Ne: return InstructionType::Ne;
				case InstructionType::Lt: return InstructionType::Ge;
				case InstructionType::Gt: return InstructionType::Le;
				case InstructionType::Le: return InstructionType::Gt;
				case InstructionType::Ge: return InstructionType::Lt;
			}

			logger.Error("undefined comp instruction");
			return InstructionType::Nop;
		}

		void ConvertCompJump(Boxx::List<Instruction>& instructions, const Instruction& inst, const Boxx::UInt signedIndex, const bool sign, const bool mismatch8, const Argument& mismatchLabel) {
			Instruction jmp = Instruction(GetCompJmp(inst.type, mismatch8 ? false : sign));
			jmp.arguments.Add(inst.arguments[2]);
			instructions.Add(jmp);

			if (mismatch8) {
				const bool mismatchJump = GetCompMismatchNumber(inst.type, signedIndex) == 1;
				Argument mismatchEndLabel;

				if (mismatchJump) {
					mismatchEndLabel = Argument(ArgumentType::Label, labelStart + labelId++);

					Instruction jmp = Instruction("jmp");
					jmp.arguments.Add(mismatchEndLabel);
					instructions.Add(jmp);
				}

				Instruction lbl = Instruction(InstructionType::Label);
				lbl.instructionName = mismatchLabel.label;
				instructions.Add(lbl);

				if (mismatchJump) {
					Instruction jmp = Instruction("jmp");
					jmp.arguments.Add(inst.arguments[2]);
					instructions.Add(jmp);

					Instruction elbl = Instruction(InstructionType::Label);
					elbl.instructionName = mismatchEndLabel.label;
					instructions.Add(elbl);
				}
			}
		}

		void CompSetMemory(Boxx::List<Instruction>& instructions, const Instruction& inst, const Boxx::UInt outputIndex) {
			Instruction mov1 = Instruction(InstructionType::Mov);
			mov1.arguments.Add(GetARegister());
			mov1.arguments.Add(GetARegister());
			mov1.sizes[0] = inst.sizes[outputIndex];
			mov1.sizes[1] = 1;
			mov1.signs[0] = inst.signs[outputIndex];
			mov1.signs[1] = false;
			ConvertMov(instructions, mov1);

			Instruction mov2 = Instruction(InstructionType::Mov);
			mov2.arguments.Add(inst.arguments[outputIndex]);
			mov2.arguments.Add(GetARegister());
			mov2.sizes[0] = inst.sizes[outputIndex];
			mov2.sizes[1] = inst.sizes[outputIndex];
			mov2.signs[0] = inst.signs[outputIndex];
			mov2.signs[1] = inst.signs[outputIndex];
			ConvertMov(instructions, mov2);
		}

		void CompSetValue(Boxx::List<Instruction>& instructions, const Instruction& inst, const Boxx::UInt outputIndex) {
			Instruction mov = Instruction(InstructionType::Mov);
			mov.arguments.Add(inst.arguments[outputIndex]);
			mov.arguments.Add(GetARegister());
			mov.sizes[0] = inst.sizes[outputIndex];
			mov.sizes[1] = 1;
			mov.signs[0] = inst.signs[outputIndex];
			mov.signs[1] = false;
			ConvertMov(instructions, mov);
		}

		void ConvertCompSet(Boxx::List<Instruction>& instructions, const Instruction& inst, const Boxx::UByte signedIndex, const bool sign, const bool mismatch8, const Argument& mismatchLabel) {
			Instruction set = Instruction(GetCompSet(inst.type, mismatch8 ? false : sign));
			set.arguments.Add(GetARegister());
			set.sizes[0] = 1;
			set.signs[0] = false;
			instructions.Add(set);

			Boxx::UInt outputIndex = inst.arguments.Size() == 2 ? 0 : 2;

			if (inst.sizes[outputIndex] > 1 && inst.arguments[outputIndex].type == ArgumentType::Memory) {
				CompSetMemory(instructions, inst, outputIndex);
			}
			else {
				CompSetValue(instructions, inst, outputIndex);
			}

			if (mismatch8) {
				Argument mismatchEndLabel = Argument(ArgumentType::Label, labelStart + labelId++);

				Instruction jmp = Instruction("jmp");
				jmp.arguments.Add(mismatchEndLabel);
				instructions.Add(jmp);

				Instruction lbl = Instruction(InstructionType::Label);
				lbl.instructionName = mismatchLabel.label;
				instructions.Add(lbl);

				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(inst.arguments[outputIndex]);

				mov.arguments.Add(Argument(GetCompMismatchNumber(inst.type, signedIndex)));
				mov.sizes[0] = inst.sizes[outputIndex];
				mov.sizes[1] = inst.sizes[outputIndex];
				mov.signs[0] = inst.signs[outputIndex];
				mov.signs[1] = inst.signs[outputIndex];
				ConvertMov(instructions, mov);

				Instruction elbl = Instruction(InstructionType::Label);
				elbl.instructionName = mismatchEndLabel.label;
				instructions.Add(elbl);
			}
		}

		void ConvertCompArgs(Boxx::List<Instruction>& instructions, const Instruction& inst, Argument& arg1, Argument& arg2, const Boxx::UByte size, const Boxx::UInt signedIndex, const bool mismatch8, const Argument& mismatchLabel) {
			if (mismatch8 && inst.arguments[signedIndex].type != ArgumentType::Number) {
				CreateCompMismatch8SignCheck(instructions, inst.arguments[signedIndex], inst.sizes[signedIndex], mismatchLabel);
			}

			if (inst.sizes[1] < size || (arg2.type == ArgumentType::Number && inst.sizes[1] == 8)) {
				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(inst.sizes[0] == size ? GetARegister() : GetDRegister());
				mov.arguments.Add(arg2);
				mov.sizes[0] = size;
				mov.sizes[1] = inst.sizes[1];
				mov.signs[0] = inst.signs[1];
				mov.signs[1] = inst.signs[1];
				ConvertMov(instructions, mov);

				arg2 = inst.sizes[0] == size ? GetARegister() : GetDRegister();

				if (mismatch8 && inst.arguments[signedIndex].type == ArgumentType::Number && signedIndex == 1) {
					CreateCompMismatch8SignCheck(instructions, arg2, size, mismatchLabel);
				}
			}
			else if (mismatch8 && inst.arguments[signedIndex].type == ArgumentType::Number && signedIndex == 1) {
				CreateCompMismatch8SignCheck(instructions, arg2, size, inst.sizes[1], mismatchLabel, inst.sizes[0] == size);
			}

			bool doubleStack = arg1.type == ArgumentType::Memory && arg2.type == ArgumentType::Memory;

			if (inst.sizes[0] < size || arg1.type == ArgumentType::Number || doubleStack) {
				Instruction mov = Instruction(InstructionType::Mov);
				mov.arguments.Add(GetARegister());
				mov.arguments.Add(arg1);
				mov.sizes[0] = size;
				mov.sizes[1] = inst.sizes[0];
				mov.signs[0] = inst.signs[0];
				mov.signs[1] = inst.signs[0];
				ConvertMov(instructions, mov);

				arg1 = GetARegister();

				if (mismatch8 && inst.arguments[signedIndex].type == ArgumentType::Number && signedIndex == 0) {
					CreateCompMismatch8SignCheck(instructions, arg1, size, mismatchLabel);
				}
			}
		}

		void ConvertComp(Boxx::List<Instruction>& instructions, const Instruction& instruction) {
			Boxx::String instName = "cmp";
			Instruction inst = instruction.Copy();

			bool mismatch = instruction.signs[0] != instruction.signs[1];
			bool mismatch8 = false;
			Boxx::UInt signedIndex = 0;

			Boxx::UByte size = Boxx::Math::Max(instruction.sizes[0], instruction.sizes[1]);
			bool sign = mismatch || instruction.signs[0];
			bool isJump = inst.arguments.Size() == 3 && inst.arguments[2].type == ArgumentType::Label;

			if (mismatch) {
				UpdateMismatchCompInfo(instruction, signedIndex, size, mismatch8);
			}

			if (inst.arguments[1].type == ArgumentType::Number) {
				ConvertNumber(inst.arguments[1], inst.sizes[1], inst.signs[1]);
				inst.sizes[1] = size;
			}

			Argument arg1 = inst.arguments[0];
			Argument arg2 = inst.arguments[1];

			Argument mismatchLabel;

			if (mismatch8) {
				mismatchLabel = Argument(ArgumentType::Label, labelStart + labelId++);
			}

			ConvertCompArgs(instructions, inst, arg1, arg2, size, signedIndex, mismatch8, mismatchLabel);

			Instruction comp = Instruction(GetSizeName("cmp", size));
			comp.arguments.Add(arg1);
			comp.arguments.Add(arg2);
			comp.sizes[0] = size;
			comp.sizes[1] = size;
			comp.signs[0] = sign;
			comp.signs[1] = sign;
			instructions.Add(comp);

			if (isJump) {
				ConvertCompJump(instructions, inst, signedIndex, sign, mismatch8, mismatchLabel);
			}
			else {
				ConvertCompSet(instructions, inst, signedIndex, sign, mismatch8, mismatchLabel);
			}
		}

		void UpdateMismatchCompInfo(const Instruction& instruction, Boxx::UInt& signedIndex, Boxx::UByte& size, bool& mismatch8) {
			signedIndex = instruction.signs[0] ? 0 : 1;

			if (size < 8) {
				if (instruction.signs[0] == false)
					size = Boxx::Math::Max(instruction.sizes[0] * 2, (Boxx::Int)instruction.sizes[1]);
				else
					size = Boxx::Math::Max(instruction.sizes[1] * 2, (Boxx::Int)instruction.sizes[0]);
			}
			else if (
				(instruction.signs[0] == false && instruction.sizes[0] == 8) ||
				(instruction.signs[1] == false && instruction.sizes[1] == 8)
			) {
				mismatch8 = true;
			}
		}

		void CreateCompMismatch8SignCheck(Boxx::List<Instruction>& instructions, const Argument& arg, const Boxx::UByte size, const Boxx::UByte argSize, const Argument& mismatchLabel, const bool useA) {
			Instruction mov = Instruction(InstructionType::Mov);
			mov.arguments.Add(useA ? GetARegister() : GetDRegister());
			mov.arguments.Add(arg);
			mov.sizes[0] = size;
			mov.sizes[1] = argSize;
			mov.signs[0] = true;
			mov.signs[1] = true;
			ConvertMov(instructions, mov);

			CreateCompMismatch8SignCheck(instructions, useA ? GetARegister() : GetDRegister(), size, mismatchLabel);
		}

		void CreateCompMismatch8SignCheck(Boxx::List<Instruction>& instructions, const Argument& arg, const Boxx::UByte size, const Argument& mismatchLabel) {
			Instruction comp = Instruction(GetSizeName("cmp", size));
			comp.arguments.Add(arg);
			comp.arguments.Add(Argument(0));
			comp.sizes[0] = size;
			comp.sizes[1] = size;
			comp.signs[0] = true;
			comp.signs[1] = true;
			instructions.Add(comp);

			Instruction jmp = Instruction("jl");
			jmp.arguments.Add(mismatchLabel);
			instructions.Add(jmp);
		}

		Boxx::UByte GetCompMismatchNumber(const InstructionType type, const Boxx::UInt signedIndex) {
			if (signedIndex == 0) switch (type) {
				case InstructionType::Ne:
				case InstructionType::Lt:
				case InstructionType::Le: {
					return 1;
				}
			}

			if (signedIndex == 1) switch (type) {
				case InstructionType::Ne:
				case InstructionType::Gt:
				case InstructionType::Ge: {
					return 1;
				}
			}

			return 0;
		}

		void ConvertPushPop(Boxx::List<Instruction>& instructions, const Instruction& instruction) {
			const bool isPush = instruction.type == InstructionType::Push;

			if (instruction.arguments.IsEmpty()) {
				Instruction inst = Instruction(isPush ? InstructionType::Sub : InstructionType::Add, KiwiLang::maxSize);
				inst.arguments.Add(Argument(Register(RegisterType::Stack)));
				inst.arguments.Add(Argument(instruction.sizes[0]));
				ConvertBinaryOperator(instructions, inst);
			}
			else {
				Instruction inst = instruction.Copy();
				inst.type = InstructionType::Custom;
				inst.instructionName = GetSizeName(isPush ? "push" : "pop", inst.sizes[0]);
				instructions.Add(inst);
			}
		}
	};
}