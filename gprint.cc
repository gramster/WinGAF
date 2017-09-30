/*
 * File:	gprint.cc
 * Purpose:	basic print class for printing the canvas
 * Author:	Graham Wheeler
 * Created:	1998
 * Updated:	
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include <ctype.h>
#include "wx.h"

#if !USE_PRINTING_ARCHITECTURE
#error You must set USE_PRINTING_ARCHITECTURE to 1 in wx_setup.h to compile this demo.
#endif

#include "wx_mf.h"
#include "wx_print.h"

#include "gdialog.h"
#include "gcanvas.h"
#include "gcanvas.h"
#include "gprint.h"

GamePrintout::GamePrintout(class GameCanvas *canvas_in, char *title)
    : wxPrintout(title), 
      canvas(canvas_in)
{
    SetExtentToVisible();
}

Bool GamePrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
      	DrawPage(dc, page);
	dc->SetDeviceOrigin(0, 0);
    	return TRUE;
    }
    return FALSE;
}

Bool GamePrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
    	return FALSE;
    return TRUE;
}

void GamePrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = *maxPage = *selPageFrom = *selPageTo = 1;
}

Bool GamePrintout::HasPage(int pageNum)
{
    return (pageNum == 1);
}

void GamePrintout::SetExtent(int w, int h)
{
    if (w >= 0) width = w;
    if (h >= 0) height = h;
}

void GamePrintout::SetExtentToVisible()
{
    canvas->GetSize(&width, &height);
}

void GamePrintout::SetExtentToVirtual()
{
    canvas->GetVirtualSize(&width, &height);
}

void GamePrintout::DrawPage(wxDC *dc, int page)
{
    if (width == 0 || height == 0) return;
    float maxX = width;
    float maxY = height;
  
    // Get the size of the DC in pixels
    float w, h;
    dc->GetSize(&w, &h);

    // Calculate a suitable scaling factor
    float scaleX=(float)(w/maxX);
    float scaleY=(float)(h/maxY);

    // Use x or y scaling factor, whichever fits on the DC

    float actualScale = wxMin(scaleX,scaleY);
    dc->SetUserScale(actualScale, actualScale);

#if 0
    // Calculate the position on the DC for centring the graphic
    float posX = (float)((w - (width*actualScale))/2.0);
    float posY = (float)((h - (height*actualScale))/2.0);
    // Set the origin
    dc->SetDeviceOrigin(posX, posY);
#endif
    canvas->Print(dc);
}

GamePrintout::~GamePrintout()
{}

