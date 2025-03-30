#include "../include/register.h"
#include "../include/debugger.h"

#include <array>
#include <string>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <cstdint>
#include <algorithm>
#include <bit>
#include <iostream>

using namespace reg;


//namespace block for reg
namespace reg {
    const std::array<regDescriptor, 27> regDescriptorList = {{
        {15, "r15", Reg::r15},
		{14, "r14", Reg::r14},
		{13, "r13", Reg::r13},  
		{12, "r12", Reg::r12},
		{6, "rbp", Reg::rbp},
		{3, "rbx", Reg::rbx},
		{11, "r11", Reg::r11},
		{10, "r10", Reg::r10},
		{9, "r9", Reg::r9},
		{8, "r8", Reg::r8},
		{0, "rax", Reg::rax},  
		{2, "rcx", Reg::rcx},
		{1, "rdx", Reg::rdx},
		{4, "rsi", Reg::rsi},
		{5, "rdi", Reg::rdi},
		{-1, "orig_rax", Reg::orig_rax},    //Doesnt have a dwarf
		{-1, "rip", Reg::rip},               //Doesnt have a dwarf
		{51, "cs", Reg::cs},
		{49, "eflags", Reg::eflags},        //Under rflags
		{7, "rsp", Reg::rsp},
		{52, "ss", Reg::ss},
		{58, "fs_base", Reg::fs_base},
		{59, "gs_base", Reg::gs_base},
		{53, "ds", Reg::ds},
		{50, "es", Reg::es},
		{54, "fs", Reg::fs},
		{55, "gs", Reg::gs}
    }};

	bool setRegisterValue(pid_t pid, Reg r, uint64_t val) {
		errno = 0;
		user_regs_struct regVals;
		ptrace(PTRACE_GETREGS, pid, nullptr, &regVals);

		auto it =
			std::find_if(regDescriptorList.begin(), regDescriptorList.end(), [r](auto&& rd) {
				return r == rd.r;
			}
		);
		*(std::bit_cast<uint64_t*>(&regVals) + (it - regDescriptorList.begin())) = val;

		return !(ptrace(PTRACE_SETREGS, pid, nullptr, &regVals)) && !errno;
	}

    uint64_t getRegisterValue(const pid_t pid, const Reg r) {
		user_regs_struct regVals;
		ptrace(PTRACE_GETREGS, pid, nullptr, &regVals);

		//auto&& -> reference to const regDescriptor struct
		auto it =
			std::find_if(regDescriptorList.begin(), regDescriptorList.end(), [r](auto&& rd) {
				return r == rd.r;
			}
		);
		return *(std::bit_cast<uint64_t*>(&regVals) + (it - regDescriptorList.begin()));
	}

    uint64_t getRegisterValue(const pid_t pid, const int dwarfNum) {
		user_regs_struct regVals;
		ptrace(PTRACE_GETREGS, pid, nullptr, &regVals);

		//auto&& -> reference to const regDescriptor struct
		auto it =
			std::find_if(regDescriptorList.begin(), regDescriptorList.end(), [dwarfNum](auto&& rd) {
				return dwarfNum == rd.dwarfNum;
			}
		);
		return *(std::bit_cast<uint64_t*>(&regVals) + (it - regDescriptorList.begin()));
	}

    std::string getRegisterName(const Reg r) {
		return std::find_if(regDescriptorList.begin(), regDescriptorList.end(), [r](auto&& rd){
			return rd.r == r;
		})->regName;
	}

    Reg getRegFromName(const std::string_view regName) {
		auto it = std::find_if(regDescriptorList.begin(), regDescriptorList.end(), [regName](auto&& rd){
			return rd.regName == regName;
		});

		if(it == regDescriptorList.end())
			return Reg::INVALID_REG;
		return it->r;
	}

	uint64_t* getAllRegisterValues(const pid_t pid, user_regs_struct& rawRegVals) {
		ptrace(PTRACE_GETREGS, pid, nullptr, &rawRegVals);
		return std::bit_cast<uint64_t*>(&rawRegVals);	//dangling pointer if regVals is not a parameter!
	}

	bool setAllRegisterValues(const pid_t pid, uint64_t* rawRegVals) {
		static_assert(sizeof(user_regs_struct) % sizeof(uint64_t) == 0, "Mismatched size");
		auto userRegsStruct = *(std::bit_cast<user_regs_struct*>(rawRegVals));
		return !(ptrace(PTRACE_SETREGS, pid, nullptr, &userRegsStruct)) && !errno;
	}

}



//Debugger register related member functions

void Debugger::dumpRegisters() const {
    /*  Issues:
        - When dumping, registers with 0 in bytes above lsb are omitted
        - dumps in a line, kinda looks ugly
        - maybe format into a neat table
    */
    user_regs_struct rawRegVals;
    auto regVals = getAllRegisterValues(pid_, rawRegVals);
    for(auto& rd : regDescriptorList) {     //uppercase vs nouppercase (default)
        std::cout << "\n" << rd.regName << ": " << std::hex << std::uppercase << "0x" << *regVals;    
        ++regVals;
    }
    std::cout << std::endl;

}


//add support for multiple words eventually (check notes/TODO)
void Debugger::readMemory(const uint64_t addr, uint64_t &data) const {  //PEEKDATA, show errors if needed
    errno = 0;
    long res = ptrace(PTRACE_PEEKDATA, pid_, addr, nullptr);
    data = std::bit_cast<uint64_t>(res);
    
    if(errno && res == -1) {
        throw std::runtime_error("ptrace error: " + std::string(strerror(errno)) + 
            ".\n Check Memory Address!\n");
    }
}

void Debugger::writeMemory(const uint64_t addr, const uint64_t &data) { //POKEDATA, show errors if needed
    errno = 0;
    long res = ptrace(PTRACE_POKEDATA, pid_, addr, data);     //const on data since its just a write

    if(errno && res == -1) {
        throw std::runtime_error("ptrace error: " + std::string(strerror(errno)) + 
            ".\n Check Memory Address!\n");
    }
}
