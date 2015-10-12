/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/
#pragma once

class Packer_implementation;
namespace sb {
	class PackerDelegate;

	class Packer {
	public:
		static Packer* create(PackerDelegate* delegate);
		void pack() const;
		void generateOptionsFile() const;
		virtual ~Packer();
	private:
		Packer(PackerDelegate* delegate);
		Packer_implementation* m_impl;
	};
}