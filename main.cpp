/*		main.cpp	-	Code Virtual Machine
		Author: Sesiria
*/
#define  _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <ctype.h>


#define UPPER_CASE_MASK	0xDF
#define MEMORY_BUFSIZ	1024 * 64
#define CHECK_BOUNDARY(val, low, high)  ((val) <= (high) && (val) >= low)
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
// output String
#define MEMORY_DUMP_TITLE	"address    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F     ................"
#define MEMORY_DUMP_LINE	"-------------------------------------------------------------------------------"

typedef struct _kernalObj
{
	unsigned char * m_BUF;			// the main memory for the machine
	unsigned char  m_A;				// the main register for the machine.
	unsigned char  m_CI;			// the 'Carry character' In register for the accumulator.
	unsigned char  m_CO;			// the 'Carry character' Out register for the accumulator.
	unsigned char  m_ZF;			// the output zero flag for the accumulator.
	unsigned char  m_CODE;			// the register for the Instructions
	unsigned char  m_CL;			// the low 8bit address register for the Code.
	unsigned char  m_CH;			// the high 8bit address register for the Code.
	unsigned char  m_IP;			// the Instruction Pointer point to the memory address.
}
kernalObj;

static kernalObj g_kernalObj = { 0 };


// function declaration.
void initMachine();
void uninitMachine();
void printMemoryDump(unsigned int start, unsigned int end);


// Function for the UI Interface.
void printMenu() {
	std::cout << "Please Input command:" << std::endl;
	std::cout << "1. Input instructions(machine code). (I)" << std::endl;
	std::cout << "2. Input assembly code. (A)" << std::endl;
	std::cout << "3. Run code. (S)" << std::endl;
	std::cout << "4. Reset machine. (R)" << std::endl;
	std::cout << "5. Dump memory. (D)" << std::endl;
	std::cout << "6. View Register. (V)" << std::endl;
	std::cout << "7. Help. (H)" << std::endl;
	std::cout << "8. Exit. (E)" << std::endl;
}

void processInputInstruction() {
	unsigned int ptrAddress = 0;	// init to the first address.
	char outputbuf[BUFSIZ] = { 0 }; // the out put buffer to screen.
	char inputbuf[BUFSIZ] = { 0 };	// the input buffer to screen.

	std::cout << "1. Return to Main Menu. (R)" << std::endl;
	std::cout << "Input format: [address] [code]" << std::endl;
	std::cout << "[code] to current address" << std::endl;
	
	while (true)
	{
		sprintf(outputbuf, "[%04x]:", ptrAddress);
		std::cout << outputbuf;
		std::cin.getline(inputbuf, BUFSIZ);
		if ((inputbuf[0] & UPPER_CASE_MASK) == 'R') // convert to upper case
			break;

		unsigned int addr = 0;
		unsigned int value = 0;
		if (sscanf(inputbuf, "%x%x", &addr, &value) <= 0)
		{
			sprintf(outputbuf, "illegal input : %s", inputbuf);
			std::cout << outputbuf << std::endl;
			continue;	// next loop
		}

		if (value == 0) // the user only input the value without address
		{
			value = addr;
			if (!CHECK_BOUNDARY(value, 0, 0xff)) {	// the upper bound of char type
				sprintf(outputbuf, "illegal value : %04x", value);
				std::cout << outputbuf << std::endl;
				continue;	// next loop
			}
		}
		else
		{
			// check the address is legal.
			if (CHECK_BOUNDARY(addr, 0, BUFSIZ - 1))
			{
				sprintf(outputbuf, "illegal address : [%04x]", addr);
				std::cout << outputbuf << std::endl;
				continue;	// next loop
			}
			ptrAddress = addr;
		}
		// store the value to address.
		g_kernalObj.m_BUF[ptrAddress] = (unsigned char)value;

		// default point to next address.
		ptrAddress++;
		// fix the point to 0 if it equal MEMORY_BUFSIZ
		if (ptrAddress == MEMORY_BUFSIZ)
			ptrAddress = 0;
	}
}

// print the memory value procedure.
void dumpMemory()
{
	unsigned int ptrAddress = 0;	// init to the first address.
	char outputbuf[BUFSIZ] = { 0 }; // the out put buffer to screen.
	char inputbuf[BUFSIZ] = { 0 };	// the input buffer to screen.

	std::cout << "1. Return to Main Menu. (R)" << std::endl;
	std::cout << 
		"Input format: [address] (print 16 address of memory)" << std::endl;
	std::cout << 
		"[From] [To] print the memory between [from] [to]" << std::endl;

	while (true) {
		std::cout << ">";
		std::cin.getline(inputbuf, BUFSIZ);
		if ((inputbuf[0] & UPPER_CASE_MASK) == 'R') // convert to upper case
			break;

		unsigned int addr1 = 0;
		unsigned int addr2 = 0;
		unsigned int readCount = 0;
		if ((readCount = sscanf(inputbuf, "%x%x", &addr1, &addr2)) <= 0)
		{
			sprintf(outputbuf, "illegal input : %s", inputbuf);
			std::cout << outputbuf << std::endl;
			continue;	// next loop
		}
		else if (readCount == 1) {
			if (!CHECK_BOUNDARY(addr1, 0, MEMORY_BUFSIZ - 1))// checkbound
			{
				sprintf(outputbuf, 
					"illegal address boundary overflow : [%04x]", addr1);
				std::cout << outputbuf << std::endl;
				continue;	// next loop
			}
			addr2 = addr1 + 16 * 8 - 1;
			addr2 = min(MEMORY_BUFSIZ - 1, addr2);
		}
		else{
			// checkbound
			if (!CHECK_BOUNDARY(addr1, 0, MEMORY_BUFSIZ - 1) ||
				!CHECK_BOUNDARY(addr2, 0, MEMORY_BUFSIZ - 1)
				)// checkbound
			{
				sprintf(outputbuf, 
					"illegal address boundary overflow : [%04x] [%04x]",
					addr1, addr2);
				std::cout << outputbuf << std::endl;
				continue;	// next loop
			}
			else if (addr1 > addr2)
			{
				sprintf(outputbuf, 
					"illegal address [%04x] is bigger than [%04x]",
					addr1, addr2);
				std::cout << outputbuf << std::endl;
				continue;
			}
		}
		printMemoryDump(addr1, addr2);
	}
}

// Print format.
// address	00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F		................
// -----------------------------------------------------------------------------
// 0000		00 00 00
void printMemoryDump(unsigned int start, unsigned int end)
{
	// start to print the formatted memory dump data.
	char outputbuf[BUFSIZ] = { 0 }; // the out put buffer to screen.
	sprintf(outputbuf, 
		"Dump memory value from address :%04X to %04X", start, end);
	std::cout << outputbuf << std::endl;
	sprintf(outputbuf, "%s", MEMORY_DUMP_TITLE);
	std::cout << outputbuf << std::endl;
	sprintf(outputbuf, "%s", MEMORY_DUMP_LINE);
	std::cout << outputbuf << std::endl;

	unsigned int lineCount = (end - start + 16) / 16;
	unsigned int offsetStart = start % 16;
	unsigned int offsetEnd = end % 16;
	unsigned int baseAddr = start - offsetStart;

	lineCount += (offsetStart != 0 && offsetEnd < offsetStart) ? 1 : 0;

	for (unsigned int i = 0; i < lineCount; i++){
		sprintf(outputbuf, "%04X       ", baseAddr + i * 16);
		std::cout << outputbuf;

		// print the value
		for (unsigned int j = 0; j < 16; j++) {
			unsigned int curAddr = baseAddr + i * 16 + j;
			if (!CHECK_BOUNDARY(curAddr, start, end))
				std::cout << "   ";
			else {
				sprintf(outputbuf, "%02X ", g_kernalObj.m_BUF[curAddr]);
				std::cout << outputbuf;
			}		
		}
		std::cout << "    ";

		// print the ascii code
		for (unsigned int j = 0; j < 16; j++) {
			unsigned int curAddr = baseAddr + i * 16 + j;
			if (!CHECK_BOUNDARY(curAddr, start, end))
				std::cout << " ";
			else {
				char c = 
					isprint(g_kernalObj.m_BUF[curAddr]) ? 
					g_kernalObj.m_BUF[curAddr] : '.';
				std::cout << c;
			}
		}
		std::cout << std::endl;
	}
}

void printWelcome() {
	system("cls");
	std::cout << "Welcome to the Code Virtual Machine" << std::endl;
	std::cout << "       v0.1 - Author: Sesiria" << std::endl;
	printMenu();
	std::cout << ">";
}



void getCommand(char * sCmd, size_t bufSize) {
	std::cin.getline(sCmd, bufSize);
}


bool processCommand(const std::string& sCmd) {

	switch (sCmd[0] & UPPER_CASE_MASK)	// convert to upper case
	{
	case 'I':
		processInputInstruction();
		printMenu();
		break;
	case 'A':
		break;
	case 'S':
		break;
	case 'R':
		initMachine();
		break;
	case 'D':
		dumpMemory();
		printMenu();
		break;
	case 'V':
		break;
	case 'H':
		break;
	case 'E':		// exit the main loop
		return false;
		break;
	default:
		std::cout << "Invalid Command for Code Machine." << std::endl;
		printMenu();
	}
	std::cout << ">";
	return true;
}


// Function for kernel machine.


void initMachine() {
	if (g_kernalObj.m_BUF)
		delete[] g_kernalObj.m_BUF;
	memset(&g_kernalObj, 0, sizeof(kernalObj));

	g_kernalObj.m_BUF = new unsigned char[MEMORY_BUFSIZ];
	memset(g_kernalObj.m_BUF, 0, sizeof(unsigned char) * MEMORY_BUFSIZ);
	std::cout << "Reset machine complete!" << std::endl;
}

void uninitMachine() {
	if (g_kernalObj.m_BUF)
		delete[] g_kernalObj.m_BUF;
}



// Main function.

int main(int argc, char * argv[], char* env[]) {

	char sCommand[BUFSIZ] = { 0 };
	initMachine();

	printWelcome();
	// main loop for standard IO
	while (true){
		getCommand(sCommand, _countof(sCommand));
		if (processCommand(sCommand) == false)
			break;
	}

	uninitMachine();
	return 0;
}