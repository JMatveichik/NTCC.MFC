//#pragma warning( disable : 4786 )
#include "stdafx.h"
#include "title.h"
#include "exceptions.h"
#include ".\rtfstring.h"
#include "main.h"

using namespace std;

static const char BEGINMARK[] = "[title]";
static const char ENDMARK[] = "[/title]";

Title::Title(XYPlot* parent)
{
	m_enabled = TRUE;
	m_text = new RTFString(parent->hmod);
}

Title::~Title()
{
	delete m_text;
}

const string& Title::GetText() const
{ 
	return m_text->GetText();
}

void Title::SetColor(COLORREF clr)
{
	m_text->SetColor(clr);
}

void Title::SetFont(const LOGFONT* const plf)
{
	m_text->SetFont(plf);
}

void Title::SetText(string title)
{
	m_text->SetText(title);
}

void Title::SetRect( int left, int top, int right, int bottom )
{
	m_self.left = left;
	m_self.top = top;
	m_self.right = right;
	m_self.bottom = bottom;
}

void Title::PreDraw( HDC hdc ) const
{
	return;
}

void Title::CalcRect(RECT& rect) const
{
	SIZE size;
	m_text->GetDimensions(size);
	rect.left = rect.top = 0;
	rect.right = size.cx;
	rect.bottom = size.cy;
}

void Title::OnDraw( HDC hdc ) const
{
	if( !m_enabled )
		return;

	SIZE size;
	m_text->GetDimensions(size);
	m_text->Draw(hdc, (m_self.left+m_self.right-size.cx)/2, m_self.top-1);
}

BOOL Title::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;

	DWORD dwRes;
	if (!WriteFile(hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL) || dwRes != sizeof(m_enabled))
		return FALSE;

	if (!m_text->Write(hFile))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}

BOOL Title::Read(HANDLE hFile)
{
	try {
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;

		DWORD dwRes;
		if (!ReadFile(hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL) || dwRes != sizeof(m_enabled))
			return FALSE;

		if (!m_text->Read(hFile))
			return FALSE;

		if (ReadString(hFile) != ENDMARK)
			return FALSE;
	}
	catch (runtime_error& e)
	{
		cerr << "Title::Read() : Exception detected: " << e.what() << endl;
		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}


