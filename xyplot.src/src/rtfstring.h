#pragma once

#include <string>

class CFormattedTextDraw;

class RTFString
{
public:
	RTFString(HMODULE hmod);
	RTFString(const RTFString& rhs) { *this = rhs; };
	~RTFString();

	const RTFString& operator=(const RTFString& rhs);

	void SetText(std::string title);
	const std::string& GetText() const;

	void SetColor(COLORREF clr);
	void SetFont(const LOGFONT* const plf);

	BOOL Write(HANDLE hFile) const;
	BOOL Read(HANDLE hFile);

	void GetDimensions(SIZE& size) const;

	void SetOrientation( int degrees ) { orientation = degrees; };
	int GetOrientation() const { return orientation; };

	void Draw(HDC hdc, int x, int y) const;

	static std::string ReadString(HANDLE hFile);
	static BOOL WriteString(HANDLE hFile, std::string theString);

protected:

	std::string strTitle;
	int orientation;

	CFormattedTextDraw* g_Formatter;
};