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

void encode(const string& path);
void decode(const string& path);
uint8_t * getMacAddress();

int main(int argc, char ** argv) {
	if (argc < 3) {
		cerr << "Usage: " << argv[0] << " <encode | decode> <file>\n";
		return 1;
	}

	if(!strcmp(argv[1],"encode")) {
		encode(argv[2]);
	}
	else if (!strcmp(argv[1], "decode")) {
		decode(argv[2]);
	}
	else {
		cerr << "Invalid option: \"" << argv[1] << "\". Expected \"encode\" or \"decode\"\n";
		cerr << "For info. See: https://github.com/vix597/flaxor \n";
		return 1;
	}
	return 0;
}

void encode(const string& path) {
	if (!PathFileExistsA(path.c_str())) {
		cerr << "Path: " << path << " does not exist. Cannot encode\n";
		exit(1);
	}

	ifstream t(path,std::ios::binary);
	string str;

	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);
	str.assign((std::istreambuf_iterator<char>(t)),std::istreambuf_iterator<char>());

	int i = 0;
	uint8_t * mac = getMacAddress();
	for (auto& c : str) {
		if (i == 8)
			i = 0;
		c = c ^ mac[i];
		i++;
	}

	ofstream o(path + ".encode",std::ios::binary);
	o.write(str.c_str(),str.length());
	return;
}

void decode(const string& path) {
	if (!PathFileExistsA(path.c_str())) {
		cerr << "Path: " << path << " does not exist. Cannot encode\n";
		exit(1);
	}

	ifstream t(path, std::ios::binary);
	string str;

	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);
	str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	int i = 0;
	uint8_t * mac = getMacAddress();
	for (auto& c : str) {
		if (i == 8)
			i = 0;
		c = c ^ mac[i];
		i++;
	}

	ofstream o(path + ".decode", std::ios::binary);
	o.write(str.c_str(), str.length());
	return;
}

uint8_t * getMacAddress() {
	IP_ADAPTER_INFO *info = NULL, *pos;
	DWORD size = 0;
	uint8_t * mac = new uint8_t[8];

	GetAdaptersInfo(info, &size);

	info = (IP_ADAPTER_INFO *)malloc(size);

	GetAdaptersInfo(info, &size);

	for (pos = info; pos != NULL; pos = pos->Next) {
		printf("\n%s\n\t", pos->Description);
		printf("%2.2x", pos->Address[0]);
		for (int i = 1; i<pos->AddressLength; i++)
			printf(":%2.2x", pos->Address[i]);
	}

	RtlCopyMemory(mac, info->Address, info->AddressLength);
	free(info);
	return mac;
}