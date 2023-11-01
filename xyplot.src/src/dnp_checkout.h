#ifndef _DNP_CHECKOUT_H_
#define _DNP_CHECKOUT_H_

#ifdef DNP_PROTECT
	#include <assert.h>
	#ifndef DNP_SOFTWARE_NAME
		#error DNP_SOFTWARE_NAME has not been defined
	#endif

	#ifndef DNP_SOFTWARE_CRC
		#error DNP_SOFTWARE_CRC has not been defined
	#endif
#endif

namespace dnp_checkout {

	class dnp_checkout_failure
	{
	public:
		dnp_checkout_failure() {}
	};

#ifdef DNP_PROTECT

	long getHardDriveComputerID();

	#define BEGIN_CHECKOUT_MAP void* checkout_functs[] = {
	#define CHECKOUT_FUNCT(a) ((void*)a), 
	#define END_CHECKOUT_MAP (void*)0xFFFFFFFF };

	#define DNP_CHECKPOINT_BEGIN(DNP_FUNCT_ID, DNP_FUNCT_NAME) try { \
		if (!decrypt((unsigned char*)checkout_functs, sizeof(checkout_functs))) throw dnp_checkout_failure(); \
		unsigned long dnp_co_addr = (unsigned long)checkout_functs[DNP_FUNCT_ID]; \
		__asm push eax \
		__asm push ebx \
		__asm push edx \
		__asm mov eax, dnp_co_addr \
		__asm lea ebx, DNP_FUNCT_NAME \
		__asm lea edx, DNP_LEX \
		__asm add eax, edx \
		__asm sub eax, ebx \
		__asm mov dnp_co_addr, eax \
		__asm pop edx \
		__asm pop ebx \
		__asm pop eax \
		__asm jmp dnp_co_addr \
		__asm nop \
		__asm sub	esp, 64 \
		__asm push	ebx \
		__asm push	esi \
		__asm push	edi \
		__asm lea	edi, DWORD PTR [ebp-64] \
		__asm mov	ecx, 16 \
		__asm mov	eax, -858993460 \
		__asm rep stosd \
		__asm pop	edi \
		__asm pop	esi \
		__asm pop	ebx \
		__asm ret	0 \
		__asm DNP_LEX:  nop \
		} catch (dnp_checkout_failure&) {

#define DNP_CHECKPOINT_END } catch(...) { };

unsigned char* get_serial_key(const char* pszSoftware)
{
	const int BUFSIZE = 128;
	static unsigned char serial[BUFSIZE];

	serial[0] = 0;
	HKEY hKeySerial;
	long res = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\DNP Software\\Serial", 0, KEY_READ, &hKeySerial);
	if (res != ERROR_SUCCESS) 
	{
		res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\DNP Software\\Serial", 0, KEY_READ, &hKeySerial);
		if (res != ERROR_SUCCESS)
			return serial;
	}

	DWORD type, size = BUFSIZE;
	res = RegQueryValueEx(hKeySerial, pszSoftware, NULL, &type, serial, &size);

	return serial;
}

unsigned char* get_pc_key()
{
	const int BUFSIZE = 128;
	static unsigned char c_id[BUFSIZE];
	static bool first = true;
	if (first) {
		first = false;
		long id = dnp_checkout::getHardDriveComputerID();
		_snprintf((char*)c_id, BUFSIZE, "%08X", id);
	}
	return c_id;
}

unsigned char get_crc()
{
	static unsigned char dnp_table_checksum = DNP_SOFTWARE_CRC;
	return dnp_table_checksum;
}

unsigned char calc_crc(unsigned char* buf, int len)
{
	unsigned char crc = 0xFF;
	for (int i=0; i<len-4; i++)
		crc ^= buf[i];
	return crc;
}

bool decrypt(unsigned char* buf, int len)
{
	static bool first = true;
	// Already decrypted?
	if (get_crc() == calc_crc(buf, len)) {
		if (first)
			assert(false);
		else
			return true;
	}
	first = false;

	unsigned char* buffer = NULL;
	try {
		// Check keys
		unsigned char* pszSerialKey = get_serial_key(DNP_SOFTWARE_NAME);
		unsigned char* pszPCKey = get_pc_key();
		
		unsigned key_len = strlen((char*)pszSerialKey);
		if (key_len == 0 || key_len != strlen((char*)pszPCKey))
			return false;

		// Temporary buffer
		buffer = new unsigned char [len];

		// Decrypt
		unsigned key_index = 0;
		for (int i=0; i<len; i++)
		{
			buffer[i] = buf[i] ^ (pszSerialKey[key_index] + pszPCKey[key_index]);

			if (++key_index == key_len)
				key_index = 0;
		}

		// Check CRC
		if (get_crc() == calc_crc(buffer, len))
		{
			memcpy(buf, buffer, len);
			delete [] buffer;
			return true;
		}
		else
		{
			delete [] buffer;
			return false;
		}
	}
	catch (...) {
		delete [] buffer;
		return false;
	}
}

#else // #ifdef DNP_PROTECT
	#define BEGIN_CHECKOUT_MAP
	#define CHECKOUT_FUNCT(a)
	#define END_CHECKOUT_MAP
	#define DNP_CHECKPOINT_BEGIN(DNP_FUNCT_ID, DNP_FUNCT_NAME) if (0) {
	#define DNP_CHECKPOINT_END }
#endif

} //namespace dnp_checkout

#endif //_DNP_CHECKOUT_H_