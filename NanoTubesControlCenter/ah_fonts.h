#pragma once

#include "singletone.h"

#ifndef _APP_FONTS_HELPER_H_INCLUDED_
#define _APP_FONTS_HELPER_H_INCLUDED_

//////////////////////////////////////////////////////////////////////////
//  [7/5/2017 Johnny A. Matveichik]
//////////////////////////////////////////////////////////////////////////

class AppFontsHelper : public Singletone<AppFontsHelper>
{
	friend class Singletone<AppFontsHelper>;

public: 

	virtual ~AppFontsHelper();

protected:

	AppFontsHelper();
	AppFontsHelper(const AppFontsHelper& );
	const AppFontsHelper& operator= (const AppFontsHelper&);

public:
	

public:

	bool Init(const char* pszFontsDir);

	const Gdiplus::PrivateFontCollection& AppFontsCollection();

private:

	Gdiplus::PrivateFontCollection* pAppFonts;
	
};

#endif //_APP_FONTS_HELPER_H_INCLUDED_
