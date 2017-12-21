// MyRichEdit.cpp : 实现文件
//
#include "stdafx.h"
#include "AutoClient.h"
#include "MyRichEdit.h"

// CMyRichEdit

IMPLEMENT_DYNAMIC(CMyRichEdit, CRichEditCtrl)

CMyRichEdit::CMyRichEdit()
{

	EnableAutomation();
}

CMyRichEdit::~CMyRichEdit()
{
}

void CMyRichEdit::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CRichEditCtrl::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CMyRichEdit, CRichEditCtrl)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CMyRichEdit, CRichEditCtrl)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_IMyRichEdit 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {B2F9EFB0-E606-47FB-8BD4-0629056EF926}
static const IID IID_IMyRichEdit =
{ 0xB2F9EFB0, 0xE606, 0x47FB, { 0x8B, 0xD4, 0x6, 0x29, 0x5, 0x6E, 0xF9, 0x26 } };

BEGIN_INTERFACE_MAP(CMyRichEdit, CRichEditCtrl)
	INTERFACE_PART(CMyRichEdit, IID_IMyRichEdit, Dispatch)
END_INTERFACE_MAP()


// CMyRichEdit 消息处理程序
void CMyRichEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRichEditCtrl::OnRButtonDown(nFlags, point);

	CMenu mn;
	mn.CreateMenu();

	mn.AppendMenu(MF_STRING, 1, "Trace");	// 第一项菜单项
	mn.AppendMenu(MF_STRING, 2, "Warning");	// 第二项菜单项
	mn.AppendMenu(MF_STRING, 3, "Error");	// 第三项菜单项

	mn.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);
	mn.DestroyMenu();
}

