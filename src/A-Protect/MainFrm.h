// MainFrm.h : CMainFrame ��Ľӿ�
#pragma once
class CMainFrame : public CFrameWndEx
{	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
// ����
public:
// ����
public:
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:  // �ؼ���Ƕ���Ա
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnExit();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);
	afx_msg void OnWindowsoverhead();
	afx_msg void OnCanceltheoverhead();
};

