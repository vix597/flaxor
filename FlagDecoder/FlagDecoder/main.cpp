#include <iostream>
#include <string>
#include <Windows.h>
#include <memory>
#include <Shlwapi.h>
#include <fstream>
#include <iphlpapi.h>

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Iphlpapi.lib")

using namespace std;

void encode_decode(const string& path, const string& mode);
uint8_t * getMacAddress();

int main(int argc, char ** argv) {
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " <encode | decode> <file>\n";
		cout << "For info. See: https://github.com/vix597/flaxor \n";
		return 1;
	}

	if(!strcmp(argv[1],"encode")) {
		encode_decode(argv[2],".encode");
	}
	else if (!strcmp(argv[1], "decode")) {
		encode_decode(argv[2],".decode");
	}
	else {
		cout << "Invalid option: \"" << argv[1] << "\". Expected \"encode\" or \"decode\"\n";
		cout << "For info. See: https://github.com/vix597/flaxor \n";
		return 1;
	}
	return 0;
}

void encode_decode(const string& path, const string& mode) {
	if (!PathFileExistsA(path.c_str())) {
		cout << "Path: " << path << " does not exist. Cannot encode\n";
		exit(1);
	}

	FILE * f = NULL;

	errno_t err = fopen_s(&f, path.c_str(), "rb");
	if (err) {
		cout << "Unable to open " << path << " for reading. Errno: " << err << "\n";
		exit(1);
	}

	fseek(f, 0L, SEEK_END);
	uint64_t size = ftell(f);
	uint8_t * buf = new uint8_t[size];
	fseek(f, 0L, SEEK_SET);

	fread_s(buf, size, sizeof(uint8_t), size, f);
	fclose(f);

	const uint8_t * mac = getMacAddress();
	for (int i = 0; i < size; i++) {
		buf[i] ^= mac[i % 6];
	}
	delete mac;

	err = fopen_s(&f, (path + mode).c_str(), "wb");
	if (err) {
		cout << "Unable to open " << path << " for writing. Errno: " << err << "\n";
		exit(1);
	}

	fwrite(buf, sizeof(uint8_t), size, f);
	fclose(f);
	cout << "Flag written to: " << path << mode << "\n";
	return;
}

uint8_t * getMacAddress() {
	IP_ADAPTER_INFO *info = NULL, *pos;
	DWORD size = 0;
	GetAdaptersInfo(info, &size);

	info = (IP_ADAPTER_INFO *)malloc(size);

	GetAdaptersInfo(info, &size);

	for (pos = info; pos != NULL; pos = pos->Next) {
		printf("\n%s\n\t", pos->Description);
		printf("%2.2x", pos->Address[0]);
		for (int i = 1; i<pos->AddressLength; i++)
			printf(":%2.2x", pos->Address[i]);
	}
	cout << "\n";
	uint8_t * mac = new uint8_t[info->AddressLength];
	memset(mac, 0, info->AddressLength);
	RtlCopyMemory(mac, info->Address, info->AddressLength);
	free(info);
	return mac;
}