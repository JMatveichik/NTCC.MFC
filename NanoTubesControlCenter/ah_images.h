#pragma once

#include "singletone.h"

#ifndef _APP_IMAGES_HELPER_H_INCLUDED_
#define _APP_IMAGES_HELPER_H_INCLUDED_

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
//  IMAGES HELPER
//////////////////////////////////////////////////////////////////////////
enum e_tIconSize
{
	ICON_SIZE_16 = 16,
	ICON_SIZE_24 = 24,
	ICON_SIZE_32 = 32,
	ICON_SIZE_48 = 48
};

class AppImagesHelper: public Singletone<AppImagesHelper>
{
	friend class Singletone<AppImagesHelper>;
public:

	virtual ~AppImagesHelper();

protected:

	AppImagesHelper();
	AppImagesHelper(const AppImagesHelper& );
	const AppImagesHelper& operator= (const AppImagesHelper&);

public:

	Gdiplus::Bitmap* GetImage(std::string fileName);
	Gdiplus::Bitmap* GetImageFromIcon(UINT id, UINT iconSize = ICON_SIZE_16);

	CExtCmdIcon* GetIcon(std::string fileName, UINT iconSize = ICON_SIZE_16);
	CExtCmdIcon* GetIcon(UINT id, UINT iconSize = ICON_SIZE_16);

protected:	

	//std::string imgsPath;
	//CRITICAL_SECTION m_cs;

	std::map<std::string, Gdiplus::Bitmap*>	vImages;
	std::map<std::string, CExtCmdIcon*>	mapIcons16;
	std::map<std::string, CExtCmdIcon*>	mapIcons24;
	std::map<std::string, CExtCmdIcon*>	mapIcons32;
	std::map<std::string, CExtCmdIcon*>	mapIcons48;
};

class ImagesStrip
{
public:	

	ImagesStrip();	
	~ImagesStrip();

	bool Create(const char* pszFileName, int width = -1);
	bool Create(Gdiplus::Bitmap* image, int width = -1);

	Gdiplus::Bitmap* GetImage(int index);
	int Size() const { return (int)images.size(); }


protected:

	std::vector<Gdiplus::Bitmap*> images;

private:

};

#endif _APP_IMAGES_HELPER_H_INCLUDED_
