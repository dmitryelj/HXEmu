// =============================================================================
// @author  Pontus Rodling <frigolit@frigolit.net>
// @license MIT license - See LICENSE for more information
// =============================================================================

#include <stdint.h>
#include <stdio.h>

#include "memorybus.h"

CMemoryBus::CMemoryBus() {
	entries = new vector<CMemoryBusEntry *>();
	p_fallback = NULL;
}

CMemoryBus::~CMemoryBus() {
	int sz = entries->size();

	for (int i = 0; i < sz; i++) {
		delete(entries->at(i));
	}

	delete(entries);
}

void CMemoryBus::add(CMemoryDevice *dev, uint16_t start, uint16_t size) {
	CMemoryBusEntry *ent = new CMemoryBusEntry();
	ent->p_device = dev;
	ent->i_start = start;
	ent->i_size = size;

	entries->push_back(ent);
}

uint8_t CMemoryBus::read(uint16_t addr) {
	// Iterate over all entries and pick the first one that matches the specified address.
	int sz = entries->size();

	for (int i = 0; i < sz; i++) {
		CMemoryBusEntry *e = entries->at(i);
		if (addr >= e->i_start && addr < e->i_start + e->i_size) {
			return e->p_device->read(addr - e->i_start);
		}
	}

	// No matching entry found, read from fallback if available, otherwise return 0xFF (pull-up resistors).
	if (p_fallback == NULL) return 0xFF;
	else return p_fallback->read(addr);
}

void CMemoryBus::write(uint16_t addr, uint8_t data) {
	// Iterate over all entries and write to the first one that matches the specified address.
	int sz = entries->size();

	for (int i = 0; i < sz; i++) {
		CMemoryBusEntry *e = entries->at(i);
		if (addr >= e->i_start && addr < e->i_start + e->i_size) {
			e->p_device->write(addr - e->i_start, data);
			return;
		}
	}

	// No matching entry found, write to the fallback if available.
	if (p_fallback != NULL) p_fallback->write(addr, data);
}

