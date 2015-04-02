#ifndef VM_PAGER_DATA
#define VM_PAGER_DATA

// Libraries
#include <cstdint>
#include <stack>
#include <queue>
#include <vector>
#include <map>
#include <utility>

// Physical Page Struct; Used when page is in phys mem or is in disk
struct PPAGE
{
	// Struct constructor
	PPAGE(): 	shareId{0}, diskBlockId{0}, dirty{0}, 
			 	accessed{0}, zero{1}, pageNum{-1} {}

	// Member variables
	unsigned 	shareId;	
	unsigned	diskBlockId;	
	unsigned	vpn;			// Only for PPAGES with share_id == 0
	unsigned	pid;			// Only for PPAGES with share_id ==0;
	bool 		dirty;
	bool 		accessed;
	bool		zero;
	int			pageNum;		// Physical page number, -1 if not in phys mem
};

// Maps the process to a map from the vpn to a PPAGE
std::map<int, std::map<int, PPAGE*> > pPages;

// Map a process to its virtual page information
std::map<int, page_table_t*> pData;	// Process data

// Process arena's upper limit
std::map<int, uintptr_t> processArena; // Valid address must be less than this

// Map of share ids
std::map<int, std::vector<std::pair<int, int> > > shareIds; // Pair<process,vpn>

// Clock queue
std::queue<PPAGE*> clockQ;

// Block and physical page book keeping
std::stack<int> availBlocks; 		// Contains blocks not in use
std::stack<int> availPages;			// Contains pages not in use

// Current process
int current_pid;

// Function Declarations--------------------------------------------------------

static int 	getAvailablePage(); // Find free page, (might need to evict)
static void zeroPage(int pageNum);
static void writeToDisk(PPAGE*);
static void readFromDisk(unsigned, unsigned);
static void deletePPAGE(PPAGE* ppage);
static void updateSharedPages(int shareId, int pageNum); // Updates ppage

#endif
