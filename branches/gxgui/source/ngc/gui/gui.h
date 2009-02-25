/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * Tantric February 2009
 *
 * gui.h
 *
 * GUI class definitions
 ***************************************************************************/

#ifndef GUI_H
#define GUI_H

#include <gccore.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <wiiuse/wpad.h>
#include <mp3player.h>
#include "pngu/pngu.h"
#include "GRRLIB.h"
#include "FreeTypeGX.h"
#include "snes9xGX.h"
#include "video.h"
#include "filelist.h"
#include "fileop.h"
#include "menu.h"

#define PI 3.14159265f
#define PADCAL 50
#define SCROLL_INITIAL_DELAY 20
#define SCROLL_LOOP_DELAY 3
#define PAGESIZE 8
#define SAVELISTSIZE 6

typedef void (*UpdateCallback)(void * e);

typedef struct _paddata {
	u16 btns_d;
	u16 btns_u;
	u16 btns_h;
	s8 stickX;
	s8 stickY;
	s8 substickX;
	s8 substickY;
	u8 triggerL;
	u8 triggerR;
} PADData;

extern int rumbleRequest[4];

enum
{
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTRE,
	ALIGN_TOP,
	ALIGN_BOTTOM,
	ALIGN_MIDDLE
};

enum
{
	STATE_DEFAULT,
	STATE_SELECTED,
	STATE_CLICKED,
	STATE_DISABLED
};

enum
{
	TRIGGER_SIMPLE,
	TRIGGER_BUTTON_ONLY,
	TRIGGER_BUTTON_ONLY_IN_FOCUS
};

class GuiSound
{
	public:
		GuiSound(const u8 * s, int l);
		~GuiSound();
		void Play();
	protected:
		const u8 * sound;
		s32 length;
};

class GuiTrigger
{
	public:
		GuiTrigger();
		~GuiTrigger();
		void SetSimpleTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
		void SetButtonOnlyTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
		void SetButtonOnlyInFocusTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
		s8 WPAD_Stick(u8 right, int axis);
		bool Left();
		bool Right();
		bool Up();
		bool Down();

		u8 type;
		s32 chan;
		WPADData wpad;
		PADData pad;
};

class GuiElement
{
	public:
		GuiElement();
		~GuiElement();
		void SetParent(GuiElement * e);
		int GetLeft();
		int GetTop();
		int GetWidth();
		int GetHeight();
		void SetSize(int w, int h);
		bool IsVisible();
		bool IsSelectable();
		bool IsClickable();
		void SetSelectable(bool s);
		void SetClickable(bool c);
		int GetState();
		void SetVisible(bool v);
		void SetTrigger(GuiTrigger * t);
		void SetTrigger(u8 i, GuiTrigger * t);
		bool IsInside(int x, int y);
		void SetPosition(int x, int y);
		void SetUpdateCallback(UpdateCallback u);
		int IsFocused();
		virtual void SetFocus(int f);
		virtual void SetState(int s);
		virtual void ResetState();
		virtual int GetSelected();
		virtual void SetAlignment(int hor, int vert);
		virtual void Update(GuiTrigger * t);
		virtual void Draw();
	protected:
		bool visible;
		int focus; // -1 = cannot focus, 0 = not focused, 1 = focused
		int width;
		int height;
		int xoffset;
		int yoffset;
		int alignmentHor; // LEFT, RIGHT, CENTRE
		int alignmentVert; // TOP, BOTTOM, MIDDLE
		int state; // DEFAULT, SELECTED, CLICKED, DISABLED
		bool selectable; // is SELECTED a valid state?
		bool clickable; // is CLICKED a valid state?
		GuiTrigger * trigger[2];
		GuiElement * parentElement;
		UpdateCallback updateCB;
};

//!Groups elements into one window in which they can be managed.
class GuiWindow : public GuiElement
{
	public:
		//!Constructor
		GuiWindow();
		GuiWindow(int w, int h);
		//!Destructor.
		~GuiWindow();

		//!Appends a element at the end, thus drawing it at last.
		//!\param element The element to append. If it is already in the list, it gets removed first.
		void Append(GuiElement* e);
		//!Inserts a element into the manager.
		//!\param element The element to insert. If it is already in the list, it gets removed first.
		//!\param index The new index of the element.
		void Insert(GuiElement* e, u32 index);
		//!Removes a element from the list.
		//!\param element A element that is in the list.
		void Remove(GuiElement* e);
		//!Clears the whole GuiWindow from all GuiElement.
		void RemoveAll();

		//!Returns a element at a specified index.
		//!\param index The index from where to poll the element.
		//!\return A pointer to the element at the index. NULL if index is out of bounds.
		GuiElement* GetGuiElementAt(u32 index) const;
		//!Returns the size of the list of elements.
		//!\return The size of the current elementlist.
		u32 GetSize();
		void ResetState();
		void SetState(int s);
		int GetSelected();
		void SetFocus(int f);
		void ChangeFocus(GuiElement * e);
		void ToggleFocus(GuiTrigger * t);
		void MoveSelectionHor(int d);
		void MoveSelectionVert(int d);

		//!Draws all the elements in this GuiWindow.
		void Draw();
		void Update(GuiTrigger * t);

	protected:
		std::vector<GuiElement*> _elements;
};

class GuiImageData
{
	public:
		GuiImageData(const u8 * i);
		~GuiImageData();
		u8 * GetImage();
		int GetWidth();
		int GetHeight();
	protected:
		u8 * data;
		int height;
		int width;
};

class GuiImage : public GuiElement
{
	public:
		GuiImage(GuiImageData * img);
		GuiImage(u8 * img, int w, int h);
		~GuiImage();
		void SetAngle(float a);
		void SetAlpha(int a);
		void SetScale(float x, float y);
		void SetTile(int t);
		void Draw();
		u8 * GetImage();
		void SetImage(GuiImageData * img);
		void SetImage(u8 * img, int w, int h);
	protected:
		u8 * image;
		int alpha;
		f32 scaleX;
		f32 scaleY;
		f32 imageangle;
		int tile;
};

class GuiText : public GuiElement
{
	public:
		GuiText(const char * t, int s, GXColor c);
		~GuiText();
		void SetText(const char * t);
		void SetSize(int s);
		void SetColor(GXColor c);
		void SetStyle(u16 s);
		void SetAlignment(int hor, int vert);
		void Draw();
	protected:
		wchar_t* text;
		int size;
		u16 style;
		GXColor color;
};

class GuiButton : public GuiElement
{
	public:
		GuiButton(int w, int h);
		~GuiButton();
		void SetImage(GuiImage* i);
		void SetImageOver(GuiImage* i);
		void SetIcon(GuiImage* i);
		void SetIconOver(GuiImage* i);
		void SetLabel(GuiText* t);
		void SetLabelOver(GuiText* t);
		void SetLabel(GuiText* t, int n);
		void SetLabelOver(GuiText* t, int n);
		void SetSoundOver(GuiSound * s);
		void SetSoundClick(GuiSound * s);
		void Draw();
		void Update(GuiTrigger * t);
	protected:
		GuiImage * image;
		GuiImage * imageOver;
		GuiImage * icon;
		GuiImage * iconOver;
		GuiText * label[3];
		GuiText * labelOver[3];
		GuiSound * soundOver;
		GuiSound * soundClick;
};

class GuiFileBrowser : public GuiElement
{
	public:
		GuiFileBrowser(int w, int h);
		~GuiFileBrowser();
		void ResetState();
		void SetFocus(int f);
		void Draw();
		void TriggerUpdate();
		void Update(GuiTrigger * t);
		GuiButton * gameList[PAGESIZE];
	protected:
		int selectedItem;
		bool listChanged;

		GuiText * gameListText[PAGESIZE];
		GuiImage * gameListBg[PAGESIZE];
		GuiImage * gameListFolder[PAGESIZE];

		GuiButton * arrowUpBtn;
		GuiButton * arrowDownBtn;
		GuiButton * scrollbarBoxBtn;

		GuiImage * bgGameSelectionImg;
		GuiImage * scrollbarImg;
		GuiImage * arrowDownImg;
		GuiImage * arrowDownOverImg;
		GuiImage * arrowUpImg;
		GuiImage * arrowUpOverImg;
		GuiImage * scrollbarBoxImg;
		GuiImage * scrollbarBoxOverImg;

		GuiImageData * bgGameSelection;
		GuiImageData * bgGameSelectionEntry;
		GuiImageData * gameFolder;
		GuiImageData * scrollbar;
		GuiImageData * arrowDown;
		GuiImageData * arrowDownOver;
		GuiImageData * arrowUp;
		GuiImageData * arrowUpOver;
		GuiImageData * scrollbarBox;
		GuiImageData * scrollbarBoxOver;

		GuiTrigger * trigA;
};

typedef struct _optionlist {
	int length;
	char name[150][50];
	char value[150][50];
} OptionList;

class GuiOptionBrowser : public GuiElement
{
	public:
		GuiOptionBrowser(int w, int h, OptionList * l);
		~GuiOptionBrowser();
		void SetCol2Position(int x);
		int FindMenuItem(int c, int d);
		int GetClickedOption();
		void ResetState();
		void SetFocus(int f);
		void Draw();
		void Update(GuiTrigger * t);
		GuiText * optionVal[PAGESIZE];
	protected:
		int selectedItem;
		int listOffset;

		OptionList * options;
		int optionIndex[PAGESIZE];
		GuiButton * optionBtn[PAGESIZE];
		GuiText * optionTxt[PAGESIZE];
		GuiImage * optionBg[PAGESIZE];

		GuiButton * arrowUpBtn;
		GuiButton * arrowDownBtn;
		GuiButton * scrollbarBoxBtn;

		GuiImage * bgOptionsImg;
		GuiImage * scrollbarImg;
		GuiImage * arrowDownImg;
		GuiImage * arrowDownOverImg;
		GuiImage * arrowUpImg;
		GuiImage * arrowUpOverImg;
		GuiImage * scrollbarBoxImg;
		GuiImage * scrollbarBoxOverImg;

		GuiImageData * bgOptions;
		GuiImageData * bgOptionsEntry;
		GuiImageData * scrollbar;
		GuiImageData * arrowDown;
		GuiImageData * arrowDownOver;
		GuiImageData * arrowUp;
		GuiImageData * arrowUpOver;
		GuiImageData * scrollbarBox;
		GuiImageData * scrollbarBoxOver;

		GuiTrigger * trigA;
};

typedef struct _savelist {
	int length;
	char filename[20][255];
	GuiImageData * previewImg[20];
	char date[20][50];
	char time[10][50];
	int type[20];
	int files[2][100];
} SaveList;

class GuiSaveBrowser : public GuiElement
{
	public:
		GuiSaveBrowser(int w, int h, SaveList * l, int a);
		~GuiSaveBrowser();
		int GetClickedSave();
		void ResetState();
		void SetFocus(int f);
		void Draw();
		void Update(GuiTrigger * t);
	protected:
		int selectedItem;
		int listOffset;
		int action;

		SaveList * saves;
		GuiButton * saveBtn[SAVELISTSIZE];
		GuiText * saveDate[SAVELISTSIZE];
		GuiText * saveTime[SAVELISTSIZE];
		GuiText * saveType[SAVELISTSIZE];

		GuiImage * saveBgImg[SAVELISTSIZE];
		GuiImage * saveBgOverImg[SAVELISTSIZE];
		GuiImage * savePreviewImg[SAVELISTSIZE];

		GuiButton * arrowUpBtn;
		GuiButton * arrowDownBtn;
		GuiButton * scrollbarBoxBtn;

		GuiImage * scrollbarImg;
		GuiImage * arrowDownImg;
		GuiImage * arrowDownOverImg;
		GuiImage * arrowUpImg;
		GuiImage * arrowUpOverImg;
		GuiImage * scrollbarBoxImg;
		GuiImage * scrollbarBoxOverImg;

		GuiImageData * gameSave;
		GuiImageData * gameSaveOver;
		GuiImageData * gameSaveBlank;
		GuiImageData * scrollbar;
		GuiImageData * arrowDown;
		GuiImageData * arrowDownOver;
		GuiImageData * arrowUp;
		GuiImageData * arrowUpOver;
		GuiImageData * scrollbarBox;
		GuiImageData * scrollbarBoxOver;

		GuiTrigger * trigA;
};

typedef struct _keytype {
	char ch, chShift;
} Key;

class GuiKeyboard : public GuiWindow
{
	public:
		GuiKeyboard(char * t);
		~GuiKeyboard();
		void Update(GuiTrigger * t);
		char kbtextstr[100];
	protected:
		Key keys[4][10];
		int shift;
		int caps;
		GuiText * kbText;
		GuiImage * keyTextboxImg;
		GuiText * keyCapsText;
		GuiImage * keyCapsImg;
		GuiImage * keyCapsOverImg;
		GuiButton * keyCaps;
		GuiText * keyShiftText;
		GuiImage * keyShiftImg;
		GuiImage * keyShiftOverImg;
		GuiButton * keyShift;
		GuiText * keyBackText;
		GuiImage * keyBackImg;
		GuiImage * keyBackOverImg;
		GuiButton * keyBack;
		GuiImage * keySpaceImg;
		GuiImage * keySpaceOverImg;
		GuiButton * keySpace;
		GuiButton * keyBtn[4][10];
		GuiImage * keyImg[4][10];
		GuiImage * keyImgOver[4][10];
		GuiText * keyTxt[4][10];
		GuiImageData * keyTextbox;
		GuiImageData * key;
		GuiImageData * keyOver;
		GuiImageData * keyMedium;
		GuiImageData * keyMediumOver;
		GuiImageData * keyLarge;
		GuiImageData * keyLargeOver;
		GuiSound * keySoundOver;
		GuiTrigger * trigA;
};

#endif
