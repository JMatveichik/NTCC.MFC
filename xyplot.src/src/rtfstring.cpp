#include "stdafx.h"
#include "rtfstring.h"
#include "formattedtextdraw.h"
#include "fontmanager.h"
#include <atlconv.h>

using namespace std;

const static char szBegin[] = "[RTFSTR]";
const static char szEnd[] = "[/RTFSTR]";

RTFString::RTFString(HMODULE hmod)
{
	g_Formatter = new CFormattedTextDraw(hmod);
	g_Formatter->Create();
	SetText(strTitle = "");
	orientation = 0;
}

RTFString::~RTFString()
{
	delete g_Formatter;
}

const RTFString& RTFString::operator=(const RTFString& rhs)
{
	if (this == &rhs)
		return *this;

	SetText(strTitle = rhs.strTitle);
	orientation = rhs.orientation;

	return *this;
}

void RTFString::SetText(std::string title)
{
	if (title.length() < 4 || strncmp(&title[0], "\\rtf", 4) != 0)
	{
		//convert into RTF
		const char szHead[] = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Segue UI;}}{\\colortbl \\red0\\green0\\blue0;}\\cf0\\f0\\fs20";
		const char szEnd[] = "}";
		title.insert(0, szHead);
		title.insert(title.length(), szEnd);
	}
	strTitle = title;

	USES_CONVERSION;
	BSTR bstrText = SysAllocString(A2W(strTitle.c_str()));
	g_Formatter->put_RTFText(bstrText);
	SysFreeString(bstrText);
}

const std::string& RTFString::GetText() const
{
	return strTitle;
}

void RTFString::SetColor(COLORREF clr)
{
	// Change color
	g_Formatter->SetTextColor(clr);

	// Get back modified string
	USES_CONVERSION;
	BSTR bstrText;
	g_Formatter->get_RTFText(&bstrText);
	strTitle = W2A(bstrText);
	SysFreeString(bstrText);
}

void RTFString::SetFont(const LOGFONT* const plf)
{
	FontManager& fm = FontManager::Instance();
	unsigned font = fm.RegisterFont(*plf);

	// Change font
	g_Formatter->SetTextFont(fm.GetFont(font));

	// Get back modified string
	USES_CONVERSION;
	BSTR bstrText;
	g_Formatter->get_RTFText(&bstrText);
	strTitle = W2A(bstrText);
	SysFreeString(bstrText);
}

void RTFString::GetDimensions(SIZE& size) const
{
	LONG cx, cy;
	g_Formatter->get_NaturalWidth(1, &cx);
	g_Formatter->get_NaturalHeight(1000000000, &cy);
	cy += 3;

	double angle = double(orientation*M_PI/180);

	size.cx = LONG(cos(angle)*cx + sin(angle)*cy); 
	size.cy = LONG(cos(angle)*cy + sin(angle)*cx);
}

void RTFString::Draw(HDC hdc, int x, int y) const
{
	int nOldMode = SetGraphicsMode(hdc, GM_ADVANCED);

	LONG cx, cy;
	g_Formatter->get_NaturalWidth(1, &cx);
	g_Formatter->get_NaturalHeight(1000000000, &cy);
	cy += 3;

	float angle = float(orientation*M_PI/180);

	POINT pt;
	XFORM xform;
	xform.eM11 = cos(angle);
	xform.eM12 = sin(angle);
	xform.eM21 = -sin(angle);
	xform.eM22 = cos(angle);
	xform.eDx = 0;
	xform.eDy = 0;
	::SetWorldTransform(hdc, &xform);
	::SetViewportOrgEx(hdc, x, y, &pt);

	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 0 + cx;
	r.bottom = 0 + cy;
	//Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	g_Formatter->Draw(hdc, &r);

	xform.eM11 = xform.eM22 = 1;
	xform.eM12 = xform.eM21 = 0;
	xform.eDx = xform.eDy = 0;
	::SetWorldTransform(hdc, &xform);
	::SetViewportOrgEx(hdc, pt.x, pt.y, &pt);
	::SetGraphicsMode(hdc, nOldMode);
}

BOOL RTFString::Write(HANDLE hFile) const
{
	string str;

	if (!WriteString(hFile, szBegin))
		return FALSE;

	if (!WriteString(hFile, strTitle))
		return FALSE;

	if (!WriteString(hFile, szEnd))
		return FALSE;

	return TRUE;
}

BOOL RTFString::Read(HANDLE hFile)
{
	if (ReadString(hFile) != szBegin)
		return FALSE;

	strTitle = ReadString(hFile);
	
	if (ReadString(hFile) != szEnd)
		return FALSE;

	return TRUE;
}

std::string RTFString::ReadString(HANDLE file)
{
	static const char szErr1[] = "RTFString::ReadString I/O error";
	static const char szErr2[] = "RTFString::ReadString - corrupted string detected";
	
	const unsigned HEADERSIZE = 6;
	char szSize[HEADERSIZE]; // Example: [0006|string]
	DWORD dwRes;

	if (!ReadFile( file, szSize, HEADERSIZE, &dwRes, NULL ) || dwRes != HEADERSIZE)
		throw exception(szErr1);

	if (szSize[0] != '[' || szSize[HEADERSIZE-1] != '|')
		throw exception(szErr2);

	szSize[HEADERSIZE-1] = 0;
	int nSize = atoi(&szSize[1]);

	if (nSize < 0 || nSize > 9998)
		throw exception(szErr2);

	string str;
	str.assign(nSize, ' ');

	if (!ReadFile( file, &str[0], nSize+1, &dwRes, NULL ) || int(dwRes) != nSize+1)
		throw exception(szErr1);
	
	if (str[nSize] != ']')
		throw exception(szErr2);

	if (!str.empty())
		str.erase(--str.end());
	
	return str;
}

BOOL RTFString::WriteString(HANDLE hFile, std::string theString)
{
	const unsigned HEADERSIZE = 6;
	char szHeader[HEADERSIZE]; // Example: [0006|string]
	DWORD dwRes;

	if (theString.length() > 9998) // max for 4 digits
		return FALSE;

	_snprintf_s(szHeader, HEADERSIZE, _TRUNCATE, "[%04d|", theString.length());

	if (!WriteFile(hFile, szHeader, HEADERSIZE, &dwRes, NULL) || dwRes != HEADERSIZE)
		return FALSE;

	if (!WriteFile(hFile, theString.c_str(), DWORD(theString.length()), &dwRes, NULL) || dwRes != theString.length())
		return FALSE;

	if (!WriteFile(hFile, "]", 1, &dwRes, NULL) || dwRes != 1)
		return FALSE;

	return TRUE;
}