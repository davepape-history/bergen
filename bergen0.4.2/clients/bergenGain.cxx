#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <getopt.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/DialogS.h>
#include <Xm/FileSB.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <bergenServer.h>


#define GAIN_SLIDER 1

#define GAIN_MAX 2.0


static float currentGain=1.0;
bergenServer *soundServer;

struct _guiData
	{
	Widget main_w;
	Widget gain_textf;
	} guiData;

#define MOTIF_TYPED_LABEL(s) XtVaTypedArg,XmNlabelString,XmRString,s,strlen(s)+1
#define MOTIF_TYPED_TITLE(s) XtVaTypedArg,XmNtitleString,XmRString,s,strlen(s)+1


static void create_work_area(Widget main_w);
static void create_file_menu(Widget menubar,int buttonnum);
static void create_menus(Widget mainw);
void close_dialog(Widget w,Widget dialog);
static Widget create_a_slider(Widget parent_w,char *label,float min,float max,float,void *data);
static void slider_change_cb(Widget w,void *data,XmScaleCallbackStruct *cbs);
static void slider_drag_cb(Widget w,void *data,XmScaleCallbackStruct *cbs);
static void redraw_slider_text(Widget w,float val);
static void cleanup_and_quit(void);
static void setGain(float g);


main(int argc,char **argv)
{
 XtAppContext app;
 Widget toplevel;
/* String fallbacks[] = { "*scale*fontList: -*-helvetica-medium-r-*--10-*",
			"*layerlabel*fontList: -*-times-bold-i-*--12-*",
			NULL }; */
#define fallbacks NULL
 soundServer = new bergenServer;
 toplevel = XtVaAppInitialize(&app,argv[0],NULL,0,&argc,argv,fallbacks,NULL);
 guiData.main_w = XtVaCreateManagedWidget("main",xmMainWindowWidgetClass,toplevel,
		NULL);
// create_menus(guiData.main_w);
 create_work_area(guiData.main_w);
 XtRealizeWidget(toplevel);
 XtAppMainLoop(app);
}


static void cleanup_and_quit(void)
{
 exit(0);
}


static void create_work_area(Widget main_w)
{
 Widget box_w;
 box_w = XtVaCreateManagedWidget("rowcol",xmRowColumnWidgetClass,main_w,
		XmNwidth, 300,
		XmNheight, 80,
		NULL);
 guiData.gain_textf = create_a_slider(box_w,"Gain", 0, GAIN_MAX, currentGain,
				(void*)GAIN_SLIDER);
 XtVaSetValues(main_w,XmNworkWindow,box_w,NULL);
}

static Widget create_a_slider(Widget parent_w,char *label,float min,float max,float initval,void *data)
{
 Widget scale_w,form_w,textf_w;
 char str[32];
 form_w = XtVaCreateManagedWidget("form",xmFormWidgetClass,parent_w,
		NULL);
 scale_w = XtVaCreateManagedWidget("scale",xmScaleWidgetClass,form_w,
		MOTIF_TYPED_TITLE(label),
		XmNminimum, (int)(min*100),
		XmNmaximum, (int)(max*100),
		XmNvalue, (int)(initval*100),
		XmNshowValue, False,
		XmNorientation, XmHORIZONTAL,
		XmNscaleWidth, 300,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 2,
		NULL);
 XtAddCallback(scale_w,XmNvalueChangedCallback,(XtCallbackProc)slider_change_cb,
			data);
 XtAddCallback(scale_w,XmNdragCallback,(XtCallbackProc)slider_drag_cb,
			data);
 sprintf(str,"%.2f",initval);
 textf_w = XtVaCreateManagedWidget("textf",xmTextFieldWidgetClass,form_w,
		XmNvalue, str,
		XmNcolumns, 4,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, scale_w,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 5,
		NULL);
 return textf_w;
}

static void slider_change_cb(Widget w,void *data,XmScaleCallbackStruct *cbs)
{
 int slider = (int)data;
 if (slider == GAIN_SLIDER)
	setGain(cbs->value / 100.0f);
}

static void slider_drag_cb(Widget w,void *data,XmScaleCallbackStruct *cbs)
{
 int slider = (int)data;
 if (slider == GAIN_SLIDER)
	setGain(cbs->value / 100.0f);
}


static void setGain(float g)
{
 char message[256];
 currentGain = g;
 sprintf(message,"-1 gain %.3f",currentGain);
 soundServer->sendMessage(message);
 redraw_slider_text(guiData.gain_textf,currentGain);
}


static void redraw_slider_text(Widget w,float val)
{
 char str[32];
 sprintf(str,"%.2f",val);
 XmTextFieldSetString(w,str);
}

/* file_menu_cb
	Purpose: Callback function called when an item is selected from the
		"File" menu. The function corresponding to the selected item
		is called.
	Example call: file_menu_cb(menu_item,itemnum);
	Arguments: Widget menu_item: the menu item's widget
		   int itemnum: the index of the item selected
	Called by: callback for menu created in create_file_menu
	Author: Dave Pape
*/
static void file_menu_cb(Widget menu_item,int itemnum)
{
 switch (itemnum) {
        case 0: cleanup_and_quit();
        }
}

/* create_file_menu
	Purpose: Creates the "File" menu on the main window's menu bar. This
		menu contains entries for loading and saving specifications,
		and for quitting.
	Example call: create_file_menu(menubar,buttonnum);
	Arguments: Widget menubar: the parent menu bar of this menu
		   int buttonnum: the index of this menu on the menu bar
	Called by: create_menus
	Author: Dave Pape
*/
static void create_file_menu(Widget menubar,int buttonnum)
{
 XmString quit;
 quit = XmStringCreateSimple("Quit");
 XmVaCreateSimplePulldownMenu(menubar,"file_menu",buttonnum,
		(XtCallbackProc)file_menu_cb,
                XmVaPUSHBUTTON, quit, 'Q', NULL, NULL,
                NULL);
 XmStringFree(quit);
}


/* create_menus
	Purpose: Creates the main window's menu bar & menus.
	Example call: create_menus(main_w);
	Arguments: Widget main_w: the main window widget
	Called by: create_gui
	Author: Dave Pape
*/
static void create_menus(Widget main_w)
{
 XmString file;
 Widget menubar;
 file = XmStringCreateSimple("File");
 menubar = XmVaCreateSimpleMenuBar(main_w,"menubar",
                XmVaCASCADEBUTTON, file, 'F',
                NULL);
 XmStringFree(file);
 create_file_menu(menubar,0);
 XtManageChild(menubar);
 XtVaSetValues(main_w,XmNmenuBar,menubar,NULL);
}


void close_dialog(Widget w,Widget dialog)
{
 XtDestroyWidget(dialog);
}
