#include "stdafx.h"
#include "ah_fonts.h"

using namespace boost::filesystem;

AppFontsHelper::AppFontsHelper()
{
	pAppFonts = new Gdiplus::PrivateFontCollection();	
}

AppFontsHelper::~AppFontsHelper()
{

}


bool AppFontsHelper::Init(const char* pszFontsDir)
{
	wpath p (pszFontsDir);

	if ( p.empty() )
		return false;

	directory_iterator end_itr;

	// cycle through the directory
	try {
		for (directory_iterator itr(p); itr != end_itr; ++itr)
		{
			// If it's not a directory, list it. If you want to list directories too, just remove this check.
			if (is_regular_file(itr->path())) 
			{
				// assign current file name to current_file and echo it out to the console.
				std::wstring fntFile = itr->path().wstring();
				Gdiplus::Status nResults = pAppFonts->AddFontFile(fntFile.c_str());			
			}
		}
	}
	catch(...)
	{
		return false;
	}

	return true;
}

const Gdiplus::PrivateFontCollection& AppFontsHelper::AppFontsCollection()
{
	return *pAppFonts;
}

