//in header file
class CPlotView : public CView
{
	//class definitions

	protected:
		HPLOT m_hPlot;
		HPROFILE m_hProf1;

	//other class difinitions
};

//in cpp file
void CPlotView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	XYPlotManager& pm = XYPlotManager::Instance();

	m_hPlot = pm.CreatePlot(m_hWnd);	//create plot for CPlotView

	m_hProf1 = pm.CreateProfile(m_hPlot, "Profile 0", 0x00ff00, 1); //Create profile for plot

	pm.SetData(m_hPlot, m_hProf1, pdx, pdy, 100);
	//Other functions
}