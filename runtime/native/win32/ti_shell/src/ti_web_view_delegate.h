/*
* Copyright 2006-2008 Appcelerator, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#pragma once

#include <tchar.h>
#include <windows.h>
#include <atlbase.h>
#include <commdlg.h>
#include <objbase.h>
#include <shlwapi.h>
#include <wininet.h>
#include <commctrl.h>

#include "base/scoped_ptr.h"
#include "webkit/glue/webpreferences.h"
#include "webkit/glue/weburlrequest.h"
#include "webkit/glue/webframe.h"
#include "webkit/glue/webview.h"
#include "webkit/glue/webview_delegate.h"
#include "webkit/glue/webwidget_delegate.h"
#include "webkit/glue/plugins/webplugin_delegate_impl.h"
#include "webkit/glue/plugins/plugin_list.h"
#include "webkit/glue/webkit_glue.h"
#include "base/gfx/point.h"
#include "base/file_util.h"
#include "base/basictypes.h"
#include "base/resource_util.h"
#include "base/ref_counted.h"
#include "base/path_service.h"
#include "base/at_exit.h"
#include "base/process_util.h"
#include "base/message_loop.h"
#include "base/icu_util.h"
#include "net/base/net_module.h"
#include "webview_host.h"
#include "webwidget_host.h"
#include "simple_resource_loader_bridge.h"
#include "test_shell_request_context.h"

class TIWebShell;

#include "ti_native.h"

class TIWebViewDelegate: 
	public base::RefCounted<TIWebViewDelegate>,
	public WebViewDelegate
{
private:
	WebViewHost *host;
	HWND mainWnd;
	TIWebShell *ti_web_shell;
	TiNative *ti_native;
	HCURSOR customCursor;

public:
	bool bootstrapTitanium;
	TIWebViewDelegate(TIWebShell *ti_web_shell);
	~TIWebViewDelegate(void);

	void setHost(WebViewHost* host);
	void setMainWnd(HWND hWnd);

	// following are the functions defined in the super classes
	virtual gfx::ViewHandle GetContainingWindow(WebWidget* webwidget);

	// Called when a region of the WebWidget needs to be re-painted.
	virtual void DidInvalidateRect(WebWidget* webwidget, const gfx::Rect& rect);

	// Called when a region of the WebWidget, given by clip_rect, should be
	// scrolled by the specified dx and dy amounts.
	virtual void DidScrollRect(WebWidget* webwidget, int dx, int dy, const gfx::Rect& clip_rect);

	// This method is called to instruct the window containing the WebWidget to
	// show itself as the topmost window.  This method is only used after a
	// successful call to CreateWebWidget.  |disposition| indicates how this new
	// window should be displayed, but generally only means something for WebViews.
	virtual void Show(WebWidget* webwidget, WindowOpenDisposition disposition);

	// This method is called to instruct the window containing the WebWidget to
	// close.  Note: This method should just be the trigger that causes the
	// WebWidget to eventually close.  It should not actually be destroyed until
	// after this call returns.
	virtual void CloseWidgetSoon(WebWidget* webwidget);

	// This method is called to focus the window containing the WebWidget so
	// that it receives keyboard events.
	virtual void Focus(WebWidget* webwidget);

	// This method is called to unfocus the window containing the WebWidget so that
	// it no longer receives keyboard events.
	virtual void Blur(WebWidget* webwidget);

	virtual void SetCursor(WebWidget* webwidget, const WebCursor& cursor);

	// Returns the rectangle of the WebWidget in screen coordinates.
	virtual void GetWindowRect(WebWidget* webwidget, gfx::Rect* out_rect);


	// This method is called to re-position the WebWidget on the screen.  The given
	// rect is in screen coordinates.  The implementation may choose to ignore
	// this call or modify the given rect.  This method may be called before Show
	// has been called.
	// TODO(darin): this is more of a request; does this need to take effect
	// synchronously?
	virtual void SetWindowRect(WebWidget* webwidget, const gfx::Rect& rect);

	// Returns the rectangle of the window in which this WebWidget is embeded in.
	virtual void GetRootWindowRect(WebWidget* webwidget, gfx::Rect* out_rect);

	// Keeps track of the necessary window move for a plugin window that resulted
	// from a scroll operation.  That way, all plugin windows can be moved at the
	// same time as each other and the page.
	virtual void DidMove(WebWidget* webwidget, const WebPluginGeometry& move);

	// Suppress input events to other windows, and do not return until the widget
	// is closed.  This is used to support |window.showModalDialog|.
	virtual void RunModal(WebWidget* webwidget);

	// Owners depend on the delegates living as long as they do, so we ref them.
	virtual void AddRef();

	virtual void Release();

	// Returns true if the widget is in a background tab.
	virtual bool IsHidden();

	virtual void WindowObjectCleared(WebFrame *webFrame);

	virtual WebPluginDelegate* CreatePluginDelegate(
		WebView *webview, const GURL &url,
		const std::string &mime_type, const std::string &clsid,
		std::string *actual_mime_type);

	virtual void AddMessageToConsole(WebView* webview,
		const std::wstring& message,
		unsigned int line_no,
		const std::wstring& source_id);

	virtual void DidFinishLoadForFrame(WebView* webview,
                                     WebFrame* frame);

	virtual void RunJavaScriptAlert(WebView* webview, const std::wstring& message);

	// Displays a JavaScript confirm panel associated with the given view.
	// Clients should visually indicate that this panel comes
	// from JavaScript. The panel should have two buttons, e.g. "OK" and
	// "Cancel". Returns true if the user hit OK, or false if the user hit Cancel.
	virtual bool RunJavaScriptConfirm(WebView* webview, const std::wstring& message);

	// Displays a JavaScript text input panel associated with the given view.
	// Clients should visually indicate that this panel comes from JavaScript.
	// The panel should have two buttons, e.g. "OK" and "Cancel", and an area to
	// type text. The default_value should appear as the initial text in the
	// panel when it is shown. If the user hit OK, returns true and fills result
	// with the text in the box.  The value of result is undefined if the user
	// hit Cancel.
	virtual bool RunJavaScriptPrompt(WebView* webview,
								   const std::wstring& message,
								   const std::wstring& default_value,
								   std::wstring* result);

	virtual bool RunBeforeUnloadConfirm(WebView* webview,
									  const std::wstring& message);

	virtual bool ShouldBeginEditing(WebView* webview, 
                                             std::wstring range);
	virtual bool ShouldEndEditing(WebView* webview, 
                                           std::wstring range);
	virtual bool ShouldInsertNode(WebView* webview, 
                                           std::wstring node, 
                                           std::wstring range,
                                           std::wstring action);
	virtual bool ShouldInsertText(WebView* webview, 
                                           std::wstring text, 
                                           std::wstring range,
                                           std::wstring action);
	virtual bool ShouldChangeSelectedRange(WebView* webview, 
                                                    std::wstring fromRange, 
                                                    std::wstring toRange, 
                                                    std::wstring affinity, 
                                                    bool stillSelecting);
	virtual bool ShouldDeleteRange(WebView* webview, 
                                            std::wstring range);
	virtual bool ShouldApplyStyle(WebView* webview, 
                                           std::wstring style,
                                           std::wstring range);
	virtual bool SmartInsertDeleteEnabled();

	WebWidget* CreatePopupWidget(WebView* webview, bool focus_on_show);
};
