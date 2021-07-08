/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "qe3.h"
#include "Radiant.h"
#include "GLWidget.h"
#include "GLDrawable.h"
#include "../../renderer/ImmediateMode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(idGLWidget, CWnd)
	//{{AFX_MSG_MAP(idGLWidget)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

idGLWidget::idGLWidget()
{
	initialized = false;
	drawable = NULL;
}

idGLWidget::~idGLWidget()
{
}

/////////////////////////////////////////////////////////////////////////////
// idGLWidget message handlers

BOOL idGLWidget::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreCreateWindow(cs);
}

BOOL idGLWidget::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	if (CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext) == -1) {
		return FALSE;
	}

	CDC *dc = GetDC();
	QEW_SetupPixelFormat(dc->m_hDC, false);
	ReleaseDC(dc);

	return TRUE;
}

void idGLWidget::OnPaint()
{

	if (!initialized) {
		CDC *dc = GetDC();
		QEW_SetupPixelFormat(dc->m_hDC, false);
		ReleaseDC(dc);
		initialized = true;
	}
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(rect);

	if (!wglMakeCurrent(dc.m_hDC, win32.hGLRC)) {
	}

	const int oldWindowHeight = glConfig.windowHeight;
	const int oldWindowWidth = glConfig.windowWidth;
	const int oldVidHeight = glConfig.vidHeight;
	const int oldVidWidth = glConfig.vidWidth;

	glConfig.windowHeight = rect.Height();
	glConfig.windowWidth = rect.Width();

	glViewport(0, 0, rect.Width(), rect.Height());
	glScissor(0, 0, rect.Width(), rect.Height());

	GL_ProjectionMatrix.LoadIdentity();
	GL_ProjectionMatrix.Ortho(0, rect.Width(), 0, rect.Height(), -256, 256);

	glClearColor(0.4f, 0.4f, 0.4f, 0.7f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if (drawable) {
		drawable->draw(1, 1, rect.Width()-1, rect.Height()-1);
	} else {
		glViewport(0, 0, rect.Width(), rect.Height());
		glScissor(0, 0, rect.Width(), rect.Height());

		GL_ProjectionMatrix.LoadIdentity();
		glClearColor (0.4f, 0.4f, 0.4f, 0.7f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	wglSwapBuffers(dc);
	glFlush();
	wglMakeCurrent(win32.hDC, win32.hGLRC);

	glConfig.windowHeight = oldWindowHeight;
	glConfig.windowWidth = oldWindowWidth;
	glConfig.vidHeight = oldVidHeight;
	glConfig.vidWidth = oldVidWidth;
}

void idGLWidget::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (drawable) {
		if ( drawable->ScreenCoords() ) {
			ClientToScreen(&point);
		}
		drawable->buttonDown(MouseButton::Left, point.x, point.y);
	}
}

void idGLWidget::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (drawable) {
		if ( drawable->ScreenCoords() ) {
			ClientToScreen(&point);
		}
		drawable->buttonUp( MouseButton::Left, point.x, point.y );
	}
	ReleaseCapture();
}

void idGLWidget::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (drawable) {
		if ( drawable->ScreenCoords() ) {
			ClientToScreen(&point);
		}
		drawable->buttonDown( MouseButton::Middle, point.x, point.y );
	}
}

void idGLWidget::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (drawable) {
		if ( drawable->ScreenCoords() ) {
			ClientToScreen(&point);
		}
		drawable->buttonUp( MouseButton::Middle, point.x, point.y );
	}
	ReleaseCapture();
}

void idGLWidget::OnMouseMove(UINT nFlags, CPoint point)
{
	if (drawable) {
		if ( drawable->ScreenCoords() ) {
			ClientToScreen(&point);
		}
		drawable->mouseMove(point.x, point.y);
		RedrawWindow();
	}
}

BOOL idGLWidget::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (drawable) {
		float f = drawable->getScale();
		if ( zDelta > 0.0f ) {
			f += 0.1f;
		} else {
			f -= 0.1f;
		}
		if ( f <= 0.0f ) {
			f = 0.1f;
		}
		if ( f > 5.0f ) {
			f = 5.0f;
		}
		drawable->setScale(f);
	}
	return TRUE;
}

void idGLWidget::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	if (drawable) {
		if ( drawable->ScreenCoords() ) {
			ClientToScreen(&point);
		}
		drawable->buttonDown( MouseButton::Right, point.x, point.y );
	}
}

void idGLWidget::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (drawable) {
		if ( drawable->ScreenCoords() ) {
			ClientToScreen(&point);
		}
		drawable->buttonUp( MouseButton::Right, point.x, point.y );
	}
	ReleaseCapture();
}

void idGLWidget::setDrawable(idGLDrawable *d) {
	drawable = d;
	if (d->getRealTime()) {
		SetTimer(1, d->getRealTime(), NULL);
	}
}


void idGLWidget::OnTimer(UINT_PTR nIDEvent) {
	if (drawable && drawable->getRealTime()) {
		Invalidate(FALSE);
	} else {
		KillTimer(1);
	}
}

BOOL idGLWidget::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;

	//return CWnd::OnEraseBkgnd(pDC);
}
