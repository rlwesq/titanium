/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include "async_copy.h"
#include "filesystem_binding.h"
#ifdef OS_LINUX
#include <unistd.h>
#endif

namespace ti
{
	AsyncCopy::AsyncCopy(FilesystemBinding* parent,Host *host,std::vector<std::string> files, std::string destination, SharedBoundMethod callback) :
		parent(parent), host(host), files(files), destination(destination), callback(callback), stopped(false)
	{
		this->Set("running",Value::NewBool(true));
		this->thread = new Poco::Thread();
		this->thread->start(&AsyncCopy::Run,this);
	}
	AsyncCopy::~AsyncCopy()
	{
		if (this->thread!=NULL)
		{
			this->thread->tryJoin(10); // precaution, should already be stopped
			delete this->thread;
			this->thread = NULL;
		}
	}
	void AsyncCopy::Copy(Poco::Path &src, Poco::Path &dest)
	{
		Poco::File from(src.toString());
#ifdef DEBUG
		std::cout << "COPY: " << src.toString() << " => " << dest.toString() << " LINK=" << from.isLink() << std::endl;
#endif
		if (from.isDirectory() && !from.isLink())
		{
			Poco::File d(dest.toString());
			if (!d.exists())
			{
				d.createDirectories();
			}
			std::vector<std::string> files;
			from.list(files);
			std::vector<std::string>::iterator i = files.begin();
			while(i!=files.end())
			{
				std::string fn = (*i++);
				Poco::Path sp(kroll::FileUtils::Join(src.toString().c_str(),fn.c_str(),NULL));
				Poco::Path dp(kroll::FileUtils::Join(dest.toString().c_str(),fn.c_str(),NULL));
				this->Copy(sp,dp);
			}
		}
		else
		{
#ifndef OS_WIN32
			if (from.isLink())
			{
#ifdef OS_OSX
				NSString* originalPath = [NSString stringWithCString:src.toString().c_str()];
				NSString* destPath = [NSString stringWithCString:dest.toString().c_str()];
				BOOL worked = [[NSFileManager defaultManager] createSymbolicLinkAtPath:destPath pathContent:originalPath];
#ifdef DEBUG
				NSLog(@"SYMLINK:%@=>%@ (%d)",originalPath,destPath,worked);
#else
				KR_UNUSED(worked);
#endif
#else
				int result = link(src.toString().c_str(),dest.toString().c_str());
				if (result != 0)
				{
					std::string err = "Copy failed: Could not make link (";
					err += dest.toString() + ")";
					throw kroll::ValueException::FromString(err);
				}
#endif
			}
			else
			{
#endif			
				// in this case it's a regular file
				Poco::File s(src.toString());
				s.copyTo(dest.toString().c_str());
#ifndef OS_WIN32
			}
#endif			
		}
	}
	void AsyncCopy::Run(void* data)
	{
#ifdef OS_OSX
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
#endif

		AsyncCopy* ac = static_cast<AsyncCopy*>(data);
#ifdef DEBUG
		std::cout << "async copy started with " << ac->files.size() << " files" << std::endl;
#endif
		std::vector<std::string>::iterator iter = ac->files.begin();
		Poco::Path to(ac->destination);
		int c = 0;
		while(!ac->stopped && iter!=ac->files.end())
		{
			std::string file = (*iter++);
			c++;
#ifdef DEBUG
			std::cout << "copying async file: " << file << " (" << c << "/" << ac->files.size() << ")" << std::endl;
#endif
			try
			{
				Poco::Path from(file);
				Poco::File f(file);
				if (f.isDirectory())
				{
					ac->Copy(from,to);
				}
				else
				{
					Poco::Path dest(to,from.getFileName());
					ac->Copy(from,dest);
				}
#ifdef DEBUG
			std::cout << "copied async file: " << file << " (" << c << "/" << ac->files.size() << ")" << std::endl;
#endif
				SharedValue value = Value::NewString(file);
				ValueList args;
				args.push_back(value);
				args.push_back(Value::NewInt(c));
				args.push_back(Value::NewInt(ac->files.size()));
				ac->host->InvokeMethodOnMainThread(ac->callback, args);
#ifdef DEBUG
			std::cout << "after callback for async file: " << file << " (" << c << "/" << ac->files.size() << ")" << std::endl;
#endif
			}
			catch (Poco::Exception &ex)
			{
				std::cerr << "Error running async file copy: " << ex.displayText() << std::endl;
			}
			catch (std::exception &ex)
			{
				std::cerr << "Error running async file copy: " << ex.what() << std::endl;
			}
			catch (...)
			{
				std::cerr << "Unknown error running async file copy" << std::endl;
			}
		}
		ac->Set("running",Value::NewBool(false));
		ac->stopped = true;
#ifdef DEBUG
		std::cout << "async copy finished by copying " << c << " files" << std::endl;
#endif
#ifdef OS_OSX
		[pool release];
#endif
	}
	void AsyncCopy::ToString(const ValueList& args, SharedValue result)
	{
		result->SetString("[Async Copy]");
	}
	void AsyncCopy::Cancel(const ValueList& args, SharedValue result)
	{
		KR_DUMP_LOCATION
		if (thread!=NULL && thread->isRunning())
		{
			this->stopped = true;
			this->Set("running",Value::NewBool(false));
			result->SetBool(true);
		}
		else
		{
			result->SetBool(false);
		}
	}
}