/**
* Created by PeakGao 2008.01.07
*/

#ifndef RKT_APP_H
#define RKT_APP_H

#include "Common.h"

#ifdef SUPPORT_APP

#include <string>

#define SUPPORT_KEYBOARD_HOOK
#define WM_USER_NETMESSAGE (WM_USER + 10)
#define WM_RENDER_TIMERID		1
#define WM_TIMEAXIS_TIMERID		2

namespace rkt {

	/** 窗口消息分发器 */
	class RKT_EXPORT MessageHandler
	{
		// 构造（析构）
	public:
		MessageHandler()			{ }
		virtual ~MessageHandler()	{ }

		// 方法
	public:
		/** 处理（分发）感兴趣的消息(回调机制)，WM_DESTROY 消息被系统内置，外界没法获得
		@param hWnd 消息源的窗口句柄
		@param message 消息码
		@param wParam 参数1
		@param lParam 参数2
		@return 如果处理了消息，不想让windows调用默认处理，请返回true，否则返回false
		*/
		virtual bool onMessage(void* hWnd, uint message, uint wParam, uint lParam)	{ return false; }
	};

	typedef bool (*WinMessageFunc)(void* msg);

	/** 带窗口的应用程序（仅仅适合游戏应用，只有单个窗口） */
	class RKT_EXPORT App
	{
	protected:
		MessageHandler* mMsgHandler;				/// 消息处理器
		void*	mWnd;								/// 窗口句柄
		void*   mParentWnd;                         /// 父窗口句柄，如果嵌入模式
		bool    mIsRunning;                         /// 实现一个好的退出方法,不要依赖WM_QUIT,容易被跟踪

#ifdef SUPPORT_KEYBOARD_HOOK
		bool	mSupportHook;						/// 通过配置文件，确定是否支持hook
		bool	mNeedActivateHook;					/// 是否需要在激活时重新Hook
		HHOOK	mHook;								/// 键盘钩子句柄
#endif
		WinMessageFunc mPreMessageHandler;

		// 构造（析构）
	public:
		App();
		//App(MessageHandler* msgDispatcher);
		virtual ~App();

		// 方法
	public:
// _GAMEMARKET_VER_BEGIN
		bool createWindow(ulong w, ulong h, ulong xPos = 0, ulong yPos = 0, bool center = true, ulong style = WS_OVERLAPPEDWINDOW,HICON hIcon=NULL);
// _GAMEMARKET_VER_END_
		void setWindowTitle(const char* title);
		std::string getWindowTitle() const;

		// 属性
	public:
		void setMessageHandler(MessageHandler* msgHandler);
		MessageHandler* getMessageHandler() const;

		void setPreMsgHandler(WinMessageFunc func) {mPreMessageHandler = func;}

		void setParentHandle(HWND parenthwnd){mParentWnd=parenthwnd;};
		void* getParentHandle() const {return mParentWnd;};
		void setHandle(HWND hwnd) {mWnd=hwnd;};
		void* getHandle() const									{ return mWnd; }
		bool isTopmost() const;
		void setTopmost(bool setting);
		void flashWindow(bool hideIgnore = true);
		bool isIconic() const;
		bool isActive() const;
		void active();
		bool modifyStyle(ulong remove, ulong add, uint flags = 0);
		bool modifyStyleEx(ulong remove, ulong add, uint flags = 0);
		void exit() { mIsRunning=false; }
		void hookKeyboard(bool hook);
		void enableKBHook(bool setting);
		bool isKBHookEnabled() const;
		bool isHookKeyboardStatus() const;

		virtual int run();

		virtual bool onCreate()									{ return false; }
		virtual void onClose()									{ }
		virtual void onTimer(DWORD timerID)						{ }
		virtual void onNetMessage()								{ }

		/// 当用于游戏时，可以将返回值改为1，这样当没有windows消息时，将循环调用onIdle
		virtual int onIdle(int idleCount)						{ return 0; }

		virtual void onSize(int dx, int dy,ulong resizeType)	{ }

	private:
		bool innerModifyStyle(int styleOffset, ulong remove, ulong add, uint flags);
		static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef SUPPORT_KEYBOARD_HOOK
	protected:
		static LRESULT CALLBACK KeyboardProc(int nCode,WPARAM wParam, LPARAM lParam);
		static bool FilterKeyboard(WPARAM wParam, BOOL keyUp);  //过虑键盘输入
		void innerHookKeyboard(bool hook);
#endif
	public:
		//取得游戏的Fps
		virtual float GetFps() =0;
	};

	/// 获取全局应用程序实例
	RKT_API App* getApp();

	/// 判断某个按键是否按下
	RKT_API bool isKeyPressed(uint virtualKeyCode);

} // namespace rkt

#endif // #ifdef SUPPORT_APP

#endif // RKT_APP_H