// Description of Solution:
// Virtual Page is created or requested through extend. If the page is new, then
// we assign it a new block id. otherwise if not share id, and not new then we
// copy info from previous ppages, and set the ppage num if it is in physmem
// From there when it faults, for the first time we zero fill the page. If the
// a page is only read and then gets evicted, the next time it comes around it
// will only be zeroed in because nothing has changed and it is quicker.
// Otherwise, if there is a write, when it is evictd it needs to be written to
// disk. Any time the block is brough back to physmem, it needs to be read from
// disk.

// Libraries
#include "vm_pager.h"
#include "vm_pager_data.h"
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <climits>
#include <cassert>

using namespace std;

extern void vm_init(unsigned int memory_pages, unsigned int disk_blocks) 
{
	// Initialize availPages
	for(int i = 0; i < memory_pages; i++) 
		availPages.push(i);
	 
	// Initialize page and block stacks and physical page map
	for(int i = 0; i < disk_blocks; i++)
		availBlocks.push(i);
}

extern void vm_create(pid_t pid) 
{
	// Add new process to process container
	pData[pid] = new page_table_t;
	memset(pData[pid], 0, sizeof(*(pData[pid])));
	processArena[pid] = (uintptr_t) VM_ARENA_BASEADDR;
}

extern void vm_switch(pid_t pid) 
{
	// Check to make sure the next process exists
	assert(pData.at(pid));

	// Switch current process info over to next process's info
	current_pid = pid;
	page_table_base_register = pData[current_pid];
}

extern int vm_fault(void *addr, bool write_flag) 
{
	// Check that addr is in valid portion of arena
	if(((uintptr_t) addr < (uintptr_t) VM_ARENA_BASEADDR) || 
	   	((uintptr_t) addr >= processArena[current_pid])) 
		return -1;
	
	// Setup	
	uintptr_t vpn = ((uintptr_t) addr / VM_PAGESIZE) - VM_ARENA_BASEPAGE;
	PPAGE* ppage = pPages[current_pid][vpn];

	// First time vpn access
	if(!ppage->accessed)
	{
		// Assign a physical page and push to back of the clock queue
		ppage->pageNum = getAvailablePage();

		// If share id is not 0 update the pageNum for every vpn
		if(ppage->shareId)
			updateSharedPages(ppage->shareId, ppage->pageNum);
		else
			pData[current_pid]->ptes[vpn].ppage = ppage->pageNum;

		// Update clock and zero the page brought in
		clockQ.push(ppage);
		zeroPage(ppage->pageNum);
	}
	else if(ppage->pageNum == -1) // Not in physmem, but on disk
	{
		// Assign a physical page
		ppage->pageNum = getAvailablePage();
		pData[current_pid]->ptes[vpn].ppage = ppage->pageNum;

		// If share id is not 0 update the pageNum for every vpn
		if(ppage->shareId)
			updateSharedPages(ppage->shareId, ppage->pageNum);
		// else
			// pData[current_pid]->ptes[vpn].ppage = ppage->pageNum;

		// Push to the back of the clock queue and read from disk
		clockQ.push(ppage);
		if(ppage->zero)
			zeroPage(ppage->pageNum);
		else
			readFromDisk(ppage->diskBlockId, ppage->pageNum);
	}
	
	if(write_flag)
	{
		// Set the dirty bit and and write_enable bit if write
		ppage->dirty = true;
		ppage->zero = false;
		// If Share ID is not 0 update the r and w for all vpns
		if(ppage->shareId)
		{
			for(auto it = shareIds[ppage->shareId].begin(); it != shareIds[ppage->shareId].end(); it++)
				pData[it->first]->ptes[it->second].write_enable = 1;
		}
		else
		{
			pData[current_pid]->ptes[vpn].write_enable = 1;
		}
	}
	else 
	{
		if(ppage->shareId) 
		{
			auto vpnPair = shareIds[ppage->shareId][0];
			if(!pData[vpnPair.first]->ptes[vpnPair.second].read_enable) 
			{
				for(int i = 0; i < shareIds[ppage->shareId].size(); i++) 	//mark page as referenced
				{
					auto allVpnPairs = shareIds[ppage->shareId][i];
					if(ppage->dirty)
						pData[allVpnPairs.first]->ptes[allVpnPairs.second].write_enable = 1;
				}
			}
		}
		else 
		{
			if(!pData[ppage->pid]->ptes[ppage->vpn].read_enable)
				if(ppage->dirty)
					pData[current_pid]->ptes[vpn].write_enable = 1;
		}
	}

	// Always make sure read is enabled and access is true after fault
	// Update read bits for all vpn if share id is not 0
	if(ppage->shareId)
		for(auto it = shareIds[ppage->shareId].begin(); it != shareIds[ppage->shareId].end(); it++)
			pData[it->first]->ptes[it->second].read_enable= 1;
	else
		pData[current_pid]->ptes[vpn].read_enable = 1;
	ppage->accessed = true;
	return 0;
}

extern void vm_destroy() 
{
	for(auto it = shareIds.begin(); it != shareIds.end(); it++)
	{
		if(!(it->second).size()) // Nothing here
			continue;
			
		PPAGE* ppage = pPages[(it->second)[0].first][(it->second)[0].second];

		bool used_by_another_process = false;		
		for(int i = (it->second).size() - 1; i >= 0; i--)
		{
			if((it->second)[i].first != current_pid) used_by_another_process = true;
			else 
			{
				(it->second).erase((it->second).begin() + i); //erase this process from list of processes/vpns sharing this page
				pPages[current_pid][((it->second)[i]).second] = nullptr;
			}
		}
		
		if(!used_by_another_process) //delete the page
			deletePPAGE(ppage);
	}

	for(auto it = pPages[current_pid].begin(); it != pPages[current_pid].end(); it++)
		if(it->second)
			if(!it->second->shareId)
				deletePPAGE(it->second);
	
	pPages[current_pid].clear();				// clear out map for this process
	pPages.erase(current_pid);					// erase mapped values for this process
	delete [] pData[current_pid]->ptes;	// delete page table for this process
	processArena.erase(current_pid);
	pData.erase(current_pid);						// remove this process's data
}

extern void* vm_extend(unsigned int share_id) 
{
	// The area is already extended to maximum capacity
	if(processArena[current_pid] == (uintptr_t)VM_ARENA_BASEADDR + VM_ARENA_SIZE) 
		return nullptr;
		
	// Share id is 0, no blocks available
	if((share_id == 0) && !availBlocks.size()) 
		return nullptr;

	// Share id is non-zero, this is first virtual page to use it, and no blocks available
	if((share_id != 0) && !shareIds[share_id].size() && !availBlocks.size()) 
		return nullptr;
	
	// Blocks are available
	
	// Calculate the virtual page number for address
	uintptr_t vpn = (processArena[current_pid] / VM_PAGESIZE) - VM_ARENA_BASEPAGE;
	
	// Update Arena
	uintptr_t lowerBoundOfValidArena = processArena[current_pid];
	processArena[current_pid] += VM_PAGESIZE;
	
	// Update read and write bits
	pData[current_pid]->ptes[vpn].read_enable = 0;
	pData[current_pid]->ptes[vpn].write_enable = 0;

	// Share id is not 0
	if(share_id != 0) 
	{
		std::pair<int, int> p;
		p = std::make_pair(current_pid, vpn);
		shareIds[share_id].push_back(p);
		
		if(shareIds[share_id].size() > 1) //this is not the first of its share_id
		{
			auto vpn_that_shares_ppage = shareIds[share_id][0];
			
			// Update Read and Write enables
			pData[current_pid]->ptes[vpn].read_enable = pData[vpn_that_shares_ppage.first]->ptes[vpn_that_shares_ppage.second].read_enable;
			pData[current_pid]->ptes[vpn].write_enable = pData[vpn_that_shares_ppage.first]->ptes[vpn_that_shares_ppage.second].write_enable;

			// Find ppage
			if(pPages[vpn_that_shares_ppage.first][vpn_that_shares_ppage.second]->pageNum >= 0) //in physmem
				pData[current_pid]->ptes[vpn].ppage = pPages[vpn_that_shares_ppage.first][vpn_that_shares_ppage.second]->pageNum;
			pPages[current_pid][vpn] = pPages[vpn_that_shares_ppage.first][vpn_that_shares_ppage.second];
			return (void*) lowerBoundOfValidArena;
		}
	}

	// Create a new ppage
	PPAGE* ppage = new PPAGE;
	ppage->shareId = share_id;
	pPages[current_pid][vpn] = ppage;

	// Assign a disk block
	ppage->diskBlockId = availBlocks.top();
	availBlocks.pop();

	if(share_id == 0) 
	{
		ppage->vpn = vpn;
		ppage->pid = current_pid;
	}
	return (void*) lowerBoundOfValidArena;
}

extern int vm_syslog(void *message, size_t len)
{
	uintptr_t addr = (uintptr_t)message;

	// Error Checking: message starting before valid arena, message + len
	// address overflows uint size, message + len end ouside of valid portion
	// of arena, and len of message = 0 
	if((addr < (uintptr_t)VM_ARENA_BASEADDR) ||(addr > UINT_MAX - len)
		||(addr + len > processArena[current_pid]) ||(len == 0))
		return -1;

	string message_str;								
	uintptr_t end_addr = addr + len;	// End address of message
	while(addr < end_addr)
	{
		// Check if current page is in physmem
		uintptr_t current_vpn = (addr / VM_PAGESIZE) - VM_ARENA_BASEPAGE;
		PPAGE* current_page = pPages[current_pid][current_vpn];
		if(current_page->pageNum < 0) //not in physmem, so read in from disk
		{
			// Get location of where to put in mem and put page on clock queue
			current_page->pageNum = getAvailablePage(); 
			
			// Update the ppage for all vpns, more than 1 only needed if share id != 0
			if(current_page->shareId)
			{
				updateSharedPages(current_page->shareId, current_page->pageNum);
				for(int i = 0; i < shareIds[current_page->shareId].size(); i++) 	//mark page as referenced
				{
					auto vpnPair = shareIds[current_page->shareId][i];
					pData[vpnPair.first]->ptes[vpnPair.second].read_enable = 1;
					//pData[vpnPair.first]->ptes[vpnPair.second].write_enable = 1;
				}
			}
			else
				pData[current_pid]->ptes[current_vpn].ppage = current_page->pageNum;

			pData[current_pid]->ptes[current_vpn].read_enable = 1;
			//pData[current_pid]->ptes[current_vpn].write_enable = 1;
			clockQ.push(current_page);

			if(!current_page->accessed)	//not read/written yet, so zero out
			{
				zeroPage(current_page->pageNum);
				current_page->accessed = true;
			}
			else
			{
				if(current_page->zero)
					zeroPage(current_page->pageNum);
				else
					readFromDisk(current_page->diskBlockId, current_page->pageNum);
			}
		}
		else 
		{
			//Share ID
				//REad not enabled
					//Dirty
			if(current_page->shareId) 
			{
				auto vpnPair = shareIds[current_page->shareId][0];	//check if read is enabled for this page
				if(!pData[vpnPair.first]->ptes[vpnPair.second].read_enable) 
				{
					for(int i = 0; i < shareIds[current_page->shareId].size(); i++) 	//mark page as referenced
					{
						auto allVpnPairs = shareIds[current_page->shareId][i];
						if(current_page->dirty)
							pData[allVpnPairs.first]->ptes[allVpnPairs.second].write_enable = 1;
						pData[allVpnPairs.first]->ptes[allVpnPairs.second].read_enable = 1;
					}
				}
			}
			else 
			{
			//Non ShareID
				//REad not enabled
					//Dirty
				if(!pData[current_page->pid]->ptes[current_page->vpn].read_enable) {
					if(current_page->dirty)
						pData[current_page->pid]->ptes[current_page->vpn].write_enable = 1;
					pData[current_page->pid]->ptes[current_page->vpn].read_enable = 1;
				}
			}
		}
		
		//now read relevant bytes to message str
		unsigned nextPageAddr = (current_vpn + 1 + VM_ARENA_BASEPAGE) * VM_PAGESIZE;
		unsigned end;
		
		if(nextPageAddr <= end_addr - 1)
			end = nextPageAddr;
		else
			end = end_addr;

		for(unsigned i = addr; i < end; i++)
			message_str.append((char*)(vm_physmem) + (current_page->pageNum * VM_PAGESIZE) + (i & 0xFFF), 1);
		addr = end;
	}
	
	cout << "syslog\t\t\t" << message_str << endl;
	return 0;
}

static void zeroPage(int pageNum)
{
	// Go through each byte of the page and set it to 0
	for(unsigned i = 0; i < VM_PAGESIZE; i++)
		((char*)vm_physmem)[(pageNum * VM_PAGESIZE) + i] = 0;
}

static void writeToDisk(PPAGE* ppage)
{
	unsigned diskBlock = ppage->diskBlockId;
	char buffer[VM_PAGESIZE];
	
	// Copy the page into a buffer and call disk write
	for(unsigned i = 0; i < VM_PAGESIZE; i++)
		buffer[i] = ((char*)vm_physmem)[ppage->pageNum * VM_PAGESIZE + i];
	disk_write(diskBlock, (void*)buffer);
}

static void readFromDisk(unsigned diskBlock, unsigned pageNum)
{
	char buffer[VM_PAGESIZE];

	// Copy the block, whose info is stored in buffer into a physical page in mem
	disk_read(diskBlock, (void*)buffer);
	for(unsigned offset = 0; offset < VM_PAGESIZE; offset++)
		((char*)vm_physmem)[(pageNum * VM_PAGESIZE) + offset] = buffer[offset];
}

static int getAvailablePage()
{
	// If physical memory is not full, get next page from availPages
	if(availPages.size())	
	{
		int freePage = availPages.top();
		availPages.pop();
		return freePage;
	}
	
	// If no free space in phys mem, evict next possible page and return that location
	while(1)
	{
		PPAGE* ppage = clockQ.front();
		clockQ.pop();
		
		bool referenced = false;
		
		if(ppage->shareId) //nonzero share id
		{
			// Get a virtual page number and process id
			auto pid = shareIds[ppage->shareId][0].first;
			auto vpn = shareIds[ppage->shareId][0].second;
			
			// If the virtual page number had read enable not set to 0, reset r and w
			if(pData[pid]->ptes[vpn].read_enable) 
			{
				referenced = true;
				// Indicate page was referenced and reset bits
				for(int i = 0; i < shareIds[ppage->shareId].size(); i++) 
				{
					// Get virtual page number
					pid = shareIds[ppage->shareId][i].first;
					vpn = shareIds[ppage->shareId][i].second;
					
					// Set that page was referenced and reset r and w
					pData[pid]->ptes[vpn].read_enable = 0;
					pData[pid]->ptes[vpn].write_enable = 0;
				}
			}
		}
		else if(pData[ppage->pid]->ptes[ppage->vpn].read_enable) // 0 share id and used
		{
			//Set that page was referenced and reset r and w
			referenced = true;
			pData[ppage->pid]->ptes[ppage->vpn].read_enable = 0;
			pData[ppage->pid]->ptes[ppage->vpn].write_enable = 0;
		}
		
		// If referenced push back on to back of clock queue
		if(referenced)
			clockQ.push(ppage);

		else //page can be evicted
		{
			// If page was dirty, write back to disk
			if(ppage->dirty) 
			{
				ppage->dirty = 0;
				writeToDisk(ppage);
			}
			
			// Set physical page number to be -1 (outside of physical memory)
			int pageNum = ppage->pageNum;
			ppage->pageNum = -1;
			
			// Return location in phys mem it was in
			return pageNum;
		}
	}
}

static void deletePPAGE(PPAGE* ppage)
{
	if(ppage->pageNum >=0) //In physmem, so remove from clock queue
	{
		// Go through and if page to be evicted found, don't push back on q
		int elts_left = clockQ.size();
		while(elts_left > 0)
		{
			PPAGE* top_page = clockQ.front();
			clockQ.pop();
			if(top_page != ppage)
				clockQ.push(top_page);
			else
				break;
			elts_left--;
		}
		// If page is in phys mem, 
		availPages.push(ppage->pageNum);
	}
	availBlocks.push(ppage->diskBlockId);
	delete ppage;

}

static void updateSharedPages(int shareId, int pageNum)
{
	// Update all the vpns of other shared pages with the correct ppage
	for(auto it = shareIds[shareId].begin(); it != shareIds[shareId].end(); it++)
		pData[it->first]->ptes[it->second].ppage = pageNum;
}