/**
 * \author Bret Jackson
 * Summer 2011
 * \file  TouchButton.H
 *
 */

#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include "CommonInc.H"
#include "TouchWidget.H"
#include "TouchButtonCallback.H"

namespace VRBase {

typedef G3D::ReferenceCountedPointer<class TouchButton> TouchButtonRef;

class TouchButton : public TouchWidget
{
public:

	/* Create a multitouch button that calls a callback for touchup and touchdown events
	 * For example in a VRBaseApp:
	 * 		string themeFile = VRApp::findVRG3DDataFile("osx.gtm");
	 *		GuiThemeRef theme;
	 *		if (FileSystem::exists(themeFile)) {
	 *			theme = GuiTheme::fromFile(themeFile, debugFont);
	 *		}
	 * 		GuiThemeRef theme = GuiTheme::fromFile(themeFile, debugFont);
	 * 		TouchButton button = new TouchButton("ResetButton", "Reset Mesh", Rect2D::xywh(200, 300, 100, 50), theme, getEventMgr(), getGfxMgr(), ..... 
	 */
	TouchButton(string name, string caption, G3D::Rect2D position, G3D::GuiThemeRef theme, GfxMgrRef gfxMgr, G3D::Vector3 tableTL, G3D::Vector3 tableTR, G3D::Vector3 tableBL, G3D::Vector3 tableBR);
	~TouchButton();
	bool offerTouchDown(int id, G3D::Vector2 pos);
	bool offerTouchUp(int id);
	void draw(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace);
	void setEnabled(bool isEnabled);
	void setCaption(string caption);
	void setPosition(G3D::Rect2D position);
	bool isEnabled();
	
	
	/** Register a callback method (must be a method of a class) to be called once
    	whenever the button is pressed down.

    	void MyClass::buttonDown(string buttonName);
    	should be registered with a call from within MyClass like this
    	int id = button->addButtonDownCallback(this, &MyClass::buttonDown);
	*/
	template <class T>
	int addButtonDownCallback(T *thisPtr, void (T::*method)(string buttonName)) {
		TouchButtonFunctor *f = new SpecificTouchButtonFunctor<T>(thisPtr, method);
		int id = _nextDownCallbackID;
		_buttonDownCallbacks.set(id, f);
		_nextDownCallbackID++;
		return id;
	}

	/// TODO: Make this safe to call within a button callback.
	void removeButtonDownCallback(int id) {
		delete _buttonDownCallbacks[id];
		_buttonDownCallbacks.remove(id);
	}
	
	/** Register a callback method (must be a method of a class) to be called once
    	whenever the pressed button is released.

    	void MyClass::buttonUp(string buttonName);
    	should be registered with a call from within MyClass like this
    	int id = button->addButtonUpCallback(this, &MyClass::buttonUp);
	*/
	template <class T>
	int addButtonUpCallback(T *thisPtr, void (T::*method)(string buttonName)) {
		TouchButtonFunctor *f = new SpecificTouchButtonFunctor<T>(thisPtr, method);
		int id = _nextUpCallbackID;
		_buttonUpCallbacks.set(id, f);
		_nextUpCallbackID++;
		return id;
	}

	/// TODO: Make this safe to call within a button callback.
	void removeButtonUpCallback(int id) {
		delete _buttonUpCallbacks[id];
		_buttonUpCallbacks.remove(id);
	}
	
protected:
	int _touchId;
	bool _isPressed;
	bool _isEnabled;
	string _caption;
	G3D::Rect2D _position;
	G3D::GuiThemeRef _theme;
	string _name;
	G3D::Table<int, TouchButtonFunctor*> _buttonDownCallbacks;
	G3D::Table<int, TouchButtonFunctor*> _buttonUpCallbacks;
	
	static int _nextDownCallbackID;
	static int _nextUpCallbackID;	
};

} // end namespace
#endif
