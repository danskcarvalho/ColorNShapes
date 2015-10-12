/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#include "pch.h"
#include "WinPackerDelegate.h"
#include "Packer.h"

using namespace sb;

bool createConfigFile(int argc, const char * argv[]) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-g") == 0)
			return true;
	}
	return false;
}

int main(int argc, const char * argv[])
{
	auto delegate = WinPackerDelegate::create();
	auto packer = Packer::create(delegate);
	if (createConfigFile(argc, argv))
		packer->generateOptionsFile();
	else
		packer->pack();

	delete packer;
	return 0;
}

