// FSDoc.h : interface of the CFSDoc class
//
#pragma once

#include "ah_images.h"
#include "FSLayer.h"

class FSLayer;
class CDrawObj;
class DataSrc;
class AnalogOutputDataSrc;
class DiscreteDataSrc;
class BaseTimer;

#define HINT_UPDATE_WINDOW					0
#define HINT_UPDATE_DRAWOBJ					1
#define HINT_UPDATE_SELECTION				2
#define HINT_DELETE_SELECTION				3
#define HINT_ADD_CHECKPOINT					4

typedef struct tagControlBarItemInfo{
    std::string dsid;
    double precition;
    int colorid;
    bool autoapply;
}CONTROLBAR_ITEM_INFO, *LPCONTROLBAR_ITEM_INFO;

typedef struct tagPresetInfo {
    std::string name;

    double val;
} PRESET_ITEM, *LP_PREST_ITEM;

typedef struct tagControlBarPresetInfo
{
    std::string title;
    std::string icon;
    std::vector<PRESET_ITEM> presetItems;
}CONTROLBAR_PRESET_INFO, *LPCONTROLBAR_PRESET_INFO;

typedef struct tagControlBarInfo
{
    std::string type;
    std::string title;
    std::string iconPath;
    std::string disabledDS;
    std::vector<CONTROLBAR_ITEM_INFO> cbitems;
    std::vector<CONTROLBAR_PRESET_INFO>	  cbpresets;

} CONTROLBAR_INFO, *LPCONTROLBAR_INFO;

typedef struct tagGraphicGroup {

    std::string title;
    std::string xAxisTitle;

    std::string yLeftAxisTitle;
    std::string yRightAxisTitle;

    std::string yLeftAxisLblFormat;
    std::string yRightAxisLblFormat;

    std::pair<double, double> yLeftAxisRange;
    std::pair<double, double> yRightAxisRange;

    std::vector<std::string> dslya;
    std::vector<std::string> dsrya;

    std::vector<int> lt;
    std::vector<int> lw;
    std::vector<std::string> lp;

}GRAPHIC_GROUP, *LPGRAPHIC_GROUP;

typedef struct tagCheckPoint {
    std::string title;
    std::string ds;

    int lt;
    int lw;
    std::string lp;
    int titlepos;
}CHECKPOINTINFO, *LPCHECKPOINTINFO;

typedef struct tagRegionInfo
{
    tagRegionInfo() : pDS(NULL), pTimer(NULL) {};

    struct tagColor
    {
        tagColor() : from(0x000000), to (0x000000) {};

        COLORREF from;
        COLORREF to;
    }color;

    struct tagOpacity{
        tagOpacity () : from(0.0), to(0.0) {};
        double from;
        double to;
    }opacity;

    struct tagHatch{
        tagHatch() : type(-1), color(0x000000){};
        int type;
        COLORREF color;
    } hatch;

    struct tagBorder{
        tagBorder() : type (1), width(1), color(0x000000) {};
        int type;
        int width;
        std::string pat;
        COLORREF color;
    } border;

    struct tagTitle{
        tagTitle() : text(""), color(0xFFFFFF), fontname("Segue UI"), fontsize(28) {};
        std::string text;
        COLORREF color;
        std::string fontname;
        int fontsize;
    } title;

    bool vertical;
    DataSrc* pDS;
    BaseTimer* pTimer;

}REGIONINFO, *LPREGIONINFO;
typedef const REGIONINFO* LPCREGIONINFO;

class DrawItemStyle;
class DrawItemFont;

class CFSDoc : public CDocument
{
protected: // create from serialization only

    CFSDoc();
    DECLARE_DYNCREATE(CFSDoc)

    // Attributes
public:

    // Operations
public:

    // Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

    BOOL Rebuild();
    BOOL FromXML(const char* pszFileName);

    // Implementation
public:
    virtual ~CFSDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    void BuildDefaultsAttributes(IXMLDOMNode* pMainNode);

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

public:

    std::string GetDefault(const char* pszName) const;

    void EnumDescribedControlBars(std::vector<CONTROLBAR_INFO>& bi)
    {
        bi.assign( bars.begin(), bars.end() );

    }

    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

    // [1/5/2012 Johnny A. Matveichik]

    //�������� ����� �����
    int GetLayerCount() const { return (int)layers.size(); };

    const std::list<FSLayer*>& GetLayersList() const { return layers; };
    //�������� ���� �� ������ z-�������
    //FSLayer* GetLayer(unsigned short zIndex) { return layers.at(zIndex); }

    //��������/������ ����
    void ShowLayer(unsigned short zIndex, BOOL bShow = FALSE);

    //����������/����������� ����
    //(��������� / ��������� �������������� ��������� ����)
    void FreezeLayer(unsigned short zIndex, BOOL bFreeze = TRUE);

    // [1/5/2012 Johnny A. Matveichik]

    //Gdiplus::Image* GetBackgroundImage() const { return m_imgBG; }

    //int ItemsCount() const { return (int)items.size(); }
    //FSDocItem* GetItem(int ind) const { return items[ind]; }

    CDrawObj* HitTest(CPoint& pt) const;

    bool IsDebugMode() const { return bDebugMode; };

    CRect GetMessageListPlace() const { return rcMsgList; };

    void UpdateFSDocTitle();

    HICON GetDocumentIcon() const { return m_hDocIcon; };

    ULONG GetLMBHoldInterval() const { return m_lLBMouseHoldInterval;};
    ULONG GetUpdateInterval() const { return m_lUpdateInterval;};

    void AddItem(CDrawObj* pDO);

    CDrawObjList* GetItems() { return &m_docItems; }

    void GetDSGroup(std::string name, LPGRAPHIC_GROUP grGroup) const;

    std::vector<REGIONINFO> GetRegions() const
    {
        return regions;
    }

    LPCREGIONINFO GetRegion(const BaseTimer* pTimer) const
    {
        for (std::vector<REGIONINFO>::const_iterator it = regions.begin(); it != regions.end(); ++it)
        {
            if ( it->pTimer == pTimer )
            return &(*it);
        }
        return NULL;
    }

    std::string DSPrefix() const
    {
        return dspref;
    }

    Gdiplus::RectF GetFirstViewRect() const;

private:

    HICON m_hDocIcon ;

    CString m_strEmmebedDocTitle;

    ImagesStrip	ti;				//������ ����������� ��� ����������� ���������
    //��������� ���������� �������

    bool		bDebugMode;		//����������� ��������� � ������ �������

    ULONG		m_lLBMouseHoldInterval;	//����� ��������� ������ ����
    ULONG		m_lUpdateInterval;		//�������� ���������� ����

    CRect	rcMsgList;

    //������� ��� ���������� ����������
    std::string dspref;

    //std::vector<FSDocItem*> items;
    std::vector<CONTROLBAR_INFO> bars;
    std::vector<GRAPHIC_GROUP> grGroups;
    std::vector<REGIONINFO>    regions;

    //������ ����� �����
    std::list<FSLayer*> layers;

    //�������� ����������� ��� ���������
    std::map<std::string, std::string> defaults;

    //������ ������ ���������
    std::map<std::string, DrawItemStyle*> styles;

    //������ ������
    std::map<std::string, Gdiplus::Brush*> brushes;

    //������ ������
    std::map<std::string, Gdiplus::Pen*> pens;

    //������ �������
    std::map<std::string, DrawItemFont*> fonts;

    //������ ��������� ���������
    CDrawObjList m_docItems;

public:

    //���������� ������������ �����
    bool AddStyle(std::string name, const DrawItemStyle& dis);
    //�������� ���������� ����� � ���������
    bool AddBrush(std::string name, const Gdiplus::Brush& brush);
    //�������� ����������� ���� � ���������
    bool AddPen(std::string name, const Gdiplus::Pen& brush);
    //���������� �������������� ������
    bool AddFont(std::string name, const DrawItemFont& font);

    //��������� ����� �� �����
    DrawItemStyle* GetStyle(std::string name) const;
    //��������� ����� �� �����
    Gdiplus::Brush* GetBrush(std::string name) const;
    //��������� ���� �� �����
    Gdiplus::Pen* GetPen(std::string name) const;
    //��������� ������ �� �����
    DrawItemFont* GetFont(std::string name) const;

public:

    virtual void DeleteContents();
    virtual void OnCloseDocument();

protected:

    void OnAnalogDSChanged(const AnalogOutputDataSrc* pADS);
    void OnDiscreteDSChanged(const DiscreteDataSrc* pDDS);

    void OnTimerStart(const BaseTimer* pTimer);
    void OnTimerStop(const BaseTimer* pTimer);
    void OnTimerOverflowed(const BaseTimer* pTimer);

};

class DrawItemFont
{
public:

    DrawItemFont(const CFSDoc* doc);
    virtual ~DrawItemFont();

    //�������� ������ �� XML ����
    virtual BOOL CreateFromXML(IXMLDOMNode* pNode);

    //�������� �����
    virtual DrawItemFont* Clone() const;

public:

    //�����
    const Gdiplus::Font* GetFont() const;

    // ������������ ������
    const Gdiplus::StringFormat& GetStringFormat() const;

    // ����������
    const Gdiplus::Brush*	GetFontBrush() const;

    //����� ������
    const Gdiplus::FontStyle& GetFontStyle() const;

protected:

    //�����
    Gdiplus::Font* font;

    // ������������ ������
    Gdiplus::StringFormat sf;

    // ����� ����
    Gdiplus::Brush* fontbrush;

    // ����� ������
    Gdiplus::FontStyle fontstyle;

    // ���� �������
    Gdiplus::REAL ang;

private:

    const CFSDoc* parent;
};

class DrawItemStyle
{
public:

    DrawItemStyle(const CFSDoc* parent);
    virtual ~DrawItemStyle(void);

public:
    //�������� ����� �� XML ����
    virtual BOOL CreateFromXML(IXMLDOMNode* pNode);

    //�������
    std::vector<Gdiplus::Brush*> GetFills() const;

    //�������
    std::vector<Gdiplus::Pen*> GetStrokes() const;

    //�����
    const DrawItemFont* DrawItemStyle::GetFont() const;

    DrawItemStyle* Clone() const;

protected:

    //�������� ����� �� XML ����
    static Gdiplus::Brush* BrushFromNode(IXMLDOMNode* pNode);

    //�������� ���� �� XML ����
    static Gdiplus::Pen* PenFromNode(IXMLDOMNode* pNode);

    //�������� ������ �� XML ����
    static Gdiplus::Pen* FontFromNode(IXMLDOMNode* pNode);

    //���c���� �����
    void Clear();

protected:

    //�������
    std::vector<Gdiplus::Brush*> brushes;

    //�������
    std::vector<Gdiplus::Pen*> pens;

    //�����
    DrawItemFont* font;

private:

    //������������ ��������
    const CFSDoc* parent;
};
