#include "stdafx.h"

#include "Automation/common.h"
#include "ah_images.h"
#include "ah_project.h"


using namespace std;
//  [9/27/2010 Johnny A. Matveichik]

AppImagesHelper::AppImagesHelper()
{

}

AppImagesHelper::~AppImagesHelper()
{
	for (std::map<std::string, Gdiplus::Bitmap*>::iterator it = vImages.begin();  it != vImages.end(); it++)
		delete (*it).second;

	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons16.begin(); it != mapIcons16.end(); it++)
		delete (*it).second;

	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons24.begin(); it != mapIcons24.end(); it++)
		delete (*it).second;

	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons32.begin(); it != mapIcons32.end(); it++)
		delete (*it).second;

	for (std::map<std::string, CExtCmdIcon*>::iterator it = mapIcons48.begin(); it != mapIcons48.end(); it++)
		delete (*it).second;

	//::DeleteCriticalSection(&m_cs);

}

Gdiplus::Bitmap* AppImagesHelper::GetImageFromIcon(UINT id, UINT iconSize/* = ICON_SIZE_16*/)
{
	//::EnterCriticalSection(&m_cs);

	CString strID;
	strID.Format("res_%05d", id);

	std::map<std::string, Gdiplus::Bitmap*>::const_iterator	fnd = vImages.find( (LPCTSTR)strID );

	if ( fnd == vImages.end() )
	{
		HICON hIcon;
		hIcon = (HICON) ::LoadImage( ::AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, iconSize, iconSize, 0);

		USES_CONVERSION;
		Gdiplus::Bitmap* pImg = Gdiplus::Bitmap::FromHICON(hIcon);

		if ( pImg->GetLastStatus() != Gdiplus::Ok  )
		{
			//::LeaveCriticalSection(&m_cs);
			return NULL;
		}
		else
		{
			vImages.insert(make_pair((LPCTSTR)strID, pImg));
			//::LeaveCriticalSection(&m_cs);

			return pImg;
		}
	}
	//::LeaveCriticalSection(&m_cs);
	return (*fnd).second;
}

Gdiplus::Bitmap* AppImagesHelper::GetImage(std::string name)
{
	//::EnterCriticalSection(&m_cs);


	std::map<std::string, Gdiplus::Bitmap*>::const_iterator	fnd = vImages.find( name );

	if ( fnd == vImages.end() )
	{

		std::string imgPath =  NTCCProject::Instance().GetPath(NTCCProject::PATH_IMAGES, name);

		USES_CONVERSION;
		Gdiplus::Bitmap* pImg = new Gdiplus::Bitmap( A2W(imgPath.c_str()) );

		if ( pImg->GetLastStatus() != Gdiplus::Ok  )
		{
			//::LeaveCriticalSection(&m_cs);
			return NULL;
		}
		else
		{
			vImages.insert(make_pair(name, pImg));
			//::LeaveCriticalSection(&m_cs);

			return pImg;
		}
	}
	//::LeaveCriticalSection(&m_cs);
	return (*fnd).second;
}

CExtCmdIcon* AppImagesHelper::GetIcon(UINT id, UINT iconSize/* = ICON_SIZE_16*/)
{
	std::map<std::string, CExtCmdIcon* >* pIconMap;
	switch (iconSize)
	{
	case ICON_SIZE_16:
		pIconMap = &mapIcons16;
		break;
	case ICON_SIZE_24:
		pIconMap = &mapIcons24;
		break;
	case ICON_SIZE_32:
		pIconMap = &mapIcons32;
		break;
	case ICON_SIZE_48:
		pIconMap = &mapIcons48;
		break;
	default:
		iconSize = ICON_SIZE_16;
		pIconMap = &mapIcons16;
		break;
	}

	CString strID;
	strID.Format("res_%05d", id);

	//::EnterCriticalSection(&m_cs);
	std::map<std::string, CExtCmdIcon*>::const_iterator	fnd = pIconMap->find( (LPCTSTR)strID );

	if ( fnd == pIconMap->end() )
	{
		HICON hIcon;
		hIcon = (HICON) ::LoadImage( ::AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, iconSize, iconSize, 0); 

		if ( hIcon == NULL)
			return NULL;

		CExtCmdIcon* pIcon = new CExtCmdIcon();		
		pIcon->AssignFromHICON(hIcon, true);
		DestroyIcon(hIcon);

		pIconMap->insert(make_pair((LPCTSTR)strID, pIcon));
		//::LeaveCriticalSection(&m_cs);

		return pIcon;
	}

	//::LeaveCriticalSection(&m_cs);
	return (*fnd).second;
}

CExtCmdIcon* AppImagesHelper::GetIcon(std::string fileName, UINT iconSize/* = ICON_SIZE_16*/)
{
	std::map<std::string, CExtCmdIcon* >* pIconMap;
	switch (iconSize)
	{
	case ICON_SIZE_16:
		pIconMap = &mapIcons16;
		break;
	case ICON_SIZE_24:
		pIconMap = &mapIcons24;
		break;
	case ICON_SIZE_32:
		pIconMap = &mapIcons32;
		break;
	case ICON_SIZE_48:
		pIconMap = &mapIcons48;
		break;
	default:
		iconSize = ICON_SIZE_16;
		pIconMap = &mapIcons16;
		break;
	}

	//::EnterCriticalSection(&m_cs);
	std::map<std::string, CExtCmdIcon*>::const_iterator	fnd = pIconMap->find( fileName );

	if ( fnd == pIconMap->end() )
	{

		std::string imgPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_IMAGES, fileName);

		HICON hIcon = (HICON) ::LoadImage(NULL, imgPath.c_str(), IMAGE_ICON, iconSize, iconSize, LR_LOADFROMFILE);
		if ( hIcon == NULL)
			return NULL;

		CExtCmdIcon* pIcon = new CExtCmdIcon();		
		pIcon->AssignFromHICON(hIcon, true);
		DestroyIcon(hIcon);

		pIconMap->insert(make_pair(fileName, pIcon));

		//::LeaveCriticalSection(&m_cs);

		return pIcon;
	}

	//::LeaveCriticalSection(&m_cs);
	return (*fnd).second;
}

ImagesStrip::ImagesStrip() 
{
}

ImagesStrip::~ImagesStrip() 
{
	std::for_each(images.begin(), images.end(), delete_object<Gdiplus::Bitmap>());
}

bool ImagesStrip::Create(Gdiplus::Bitmap* image, int width/* = -1*/)
{
	if ( image == NULL) 
		return false;

	images.clear();

	UINT cy = image->GetHeight();
	UINT cx = (width == -1) ? cy : width;

	if (cx == 0 || cy == 0)
		return false;


	int nCount = image->GetWidth() / cx;

	for (int i = 0; i < nCount; i++)
	{
		Gdiplus::Bitmap* imgPart = image->Clone(cx*i, 0, cx, cy, PixelFormat32bppARGB );
		images.push_back( imgPart );
	}

	return true;
}

bool ImagesStrip::Create(const char* pszFileName, int width)
{
	USES_CONVERSION;
	return Create(Gdiplus::Bitmap::FromFile(A2W(pszFileName)), width);
}

Gdiplus::Bitmap* ImagesStrip::GetImage(int index)
{
	if ( index < 0 || index >= (int)images.size() )
		return NULL;

	return images.at( index );
}