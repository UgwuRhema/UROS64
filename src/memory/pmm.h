#include <stdint.h>

#define BLOCK_SIZE 4096
#define BLOCK_BY_BYTE 8

struct __attribute__((packed)) MemoryMapEntry
{
	uint64_t base_address;
	uint64_t length;
	uint32_t type;
	uint32_t acpi_extended;
};
