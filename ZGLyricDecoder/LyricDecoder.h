#pragma once

struct FileNode {
	char *data;
	int len;
	bool success;
	FileNode() {
		data = nullptr;
		len = 0;
		success = false;
	}
	FileNode(char *a, int b, bool c) {
		data = a;
		len = b;
		success = true;
	}
};
FileNode krcdecode(char *src, int src_len);
FileNode qrcdecode(char *src, int src_len);