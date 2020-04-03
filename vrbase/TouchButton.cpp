#include "TouchButton.H"

using namespace G3D;

int TouchButton::_nextDownCallbackID = 0;
int TouchButton::_nextUpCallbackID = 0;

TouchButton::TouchButton(string name, string caption, Rect2D position, GuiThemeRef theme, GfxMgrRef gfxMgr, Vector3 tableTL, Vector3 tableTR, Vector3 tableBL, Vector3 tableBR) : TouchWidget(gfxMgr, tableTL, tableTR, tableBL, tableBR)
{
	assert(theme != NULL);
	
	_name = name;
	_position = position;
	_theme = theme;
	_isPressed = false;
	_isEnabled = true;
	_caption = caption;	
}

TouchButton::~TouchButton()
{
}

bool TouchButton::offerTouchDown(int id, Vector2 pos)
{
	if (_isEnabled && !_isPressed) {
		if (_position.contains(pos)) {
			_touchId = id;
			Array<int> ids = _buttonDownCallbacks.getKeys();
			// Call button down callbacks
			for (int i=0;i<ids.size();i++) {
			  _buttonDownCallbacks[ids[i]]->exec(_name);
			}
			_isPressed = true;
			return true;
		}
	}
	return false;	
}

bool TouchButton::offerTouchUp(int id)
{
	if (id == _touchId && _isPressed) {
		_isPressed = false;
		Array<int> ids = _buttonUpCallbacks.getKeys();
		// Call button down callbacks
		for (int i=0;i<ids.size();i++) {
		  _buttonUpCallbacks[ids[i]]->exec(_name);
		}
		return true;
	}
	return false;
}

void TouchButton::draw(RenderDevice *rd, const CoordinateFrame &virtualToRoomSpace)
{
	_theme->beginRendering(rd);
	_theme->renderButton(_position, _isEnabled, false, _isPressed, _caption, GuiTheme::NORMAL_BUTTON_STYLE);
	_theme->endRendering();
}

void TouchButton::setEnabled(bool isEnabled)
{
	_isEnabled = isEnabled;
}

bool TouchButton::isEnabled() {
	return _isEnabled;
}

void TouchButton::setCaption(string caption)
{
	_caption = caption;
}
	
void TouchButton::setPosition(Rect2D position)
{
	_position = position;
}
