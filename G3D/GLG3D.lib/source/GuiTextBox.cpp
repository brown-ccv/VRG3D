/**
 @file GuiTextBox.cpp
 
 @maintainer Morgan McGuire, http://graphics.cs.williams.edu

 @created 2007-06-02
 @edited  2007-06-10
 */
#include "G3D/platform.h"
#include "GLG3D/GuiTextBox.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/GuiPane.h"

/** Cursor flashes per second. */
static const float               blinkRate = 3.0f;

/** Keypresses per second. */
static const float               keyRepeatRate = 18.0f;

/** Delay before repeat begins. */
static const float               keyRepeatDelay = 0.25f;

namespace G3D {

GuiTextBox::GuiTextBox(GuiContainer* parent, const GuiText& caption, 
                       const Pointer<std::string>& value, Update update) 
    : GuiControl(parent, caption), m_value(value), 
      m_cursorPos(0), m_editing(false), m_update(update), m_cursor("|") {

    unsetRepeatKeysym();
    m_keyDownTime = System::time();
}


void GuiTextBox::render(RenderDevice* rd, const GuiThemeRef& skin) const {
    (void)rd;
    (void)skin;
    GuiTextBox* me = const_cast<GuiTextBox*>(this);

    if (m_visible) {
        if (m_editing) {
            if (! focused()) {
                // Just lost focus
                if ((m_update == DELAYED_UPDATE) && (m_oldValue != m_userValue)) {
                    me->m_oldValue = m_userValue;
                    me->commit();
                    GEvent response;
                    response.gui.type = GEventType::GUI_CHANGE;
                    response.gui.control = me->m_eventSource;
                    m_gui->fireEvent(response);
                }
                me->m_editing = false;
            } else if (*m_value != m_oldValue) {
                // The value has been changed by the program while we
                // were editing; override our copy with the
                // programmatic value.
                me->m_userValue = *m_value;
                me->m_cursorPos = iMin(m_cursorPos, m_userValue.size());
            }
        } else if (focused()) {
            // Just gained focus
            me->m_userValue = *m_value;
            me->m_oldValue  = m_userValue;
            me->m_editing   = true;
        }

        static RealTime then = System::time();
        RealTime now = System::time();

        bool hasKeyDown = (m_repeatKeysym.sym != GKey::UNKNOWN);

        // Amount of time that the last simulation step took.  
        // This is used to limit the key repeat rate
        // so that it is not faster than the frame rate.
        RealTime frameTime = then - now;

        // If a key is being pressed, process it on a steady repeat schedule.
        if (hasKeyDown && (now > m_keyRepeatTime)) {
            me->processRepeatKeysym();
            me->m_keyRepeatTime = max(now + frameTime * 1.1, now + 1.0 / keyRepeatRate);
        }
        then = now;

        // Only blink the cursor when keys are not being pressed or
        // have not recently been pressed.
        bool solidCursor = hasKeyDown || (now - m_keyRepeatTime < 1.0 / blinkRate);
        if (! solidCursor) {
            static const RealTime zero = System::time();
            solidCursor = isOdd((int)((now - zero) * blinkRate));
        }

        // Note that textbox does not have a mouseover state
        skin->renderTextBox
           (m_rect,
            m_enabled,
            focused(), 
            m_caption,
            m_captionSize,
            m_editing ? m_userValue : *m_value, 
            solidCursor ? m_cursor : GuiText(std::string()),
            m_cursorPos);
    }
}


void GuiTextBox::setRepeatKeysym(GKeySym key) {
    m_keyDownTime = System::time();
    m_keyRepeatTime = m_keyDownTime + keyRepeatDelay;
    m_repeatKeysym = key;
}


void GuiTextBox::unsetRepeatKeysym() {
    m_repeatKeysym.sym = GKey::UNKNOWN;
}


void GuiTextBox::processRepeatKeysym() {
    debugAssert(m_cursorPos >= 0);
    debugAssert(m_cursorPos < 100000);

    switch (m_repeatKeysym.sym) {
    case GKey::UNKNOWN:
        // No key
        break;

    case GKey::RIGHT:
        if (m_cursorPos < (int)m_userValue.size()) {
            ++m_cursorPos;
        }
        break;

    case GKey::LEFT:
        if (m_cursorPos > 0) {
            --m_cursorPos;
        }
        break;

    case GKey::HOME:
        m_cursorPos = 0;
        break;

    case GKey::END:
        m_cursorPos = m_userValue.size();
        break;

    case GKey::DELETE:
        if (m_cursorPos < (int)m_userValue.size()) {
            m_userValue = 
                m_userValue.substr(0, m_cursorPos) + 
                m_userValue.substr(m_cursorPos + 1, std::string::npos);
        }
        break;

    case GKey::BACKSPACE:
        if (m_cursorPos > 0) {
            m_userValue = 
                m_userValue.substr(0, m_cursorPos - 1) + 
                ((m_cursorPos < (int)m_userValue.size()) ? 
                  m_userValue.substr(m_cursorPos, std::string::npos) :
                 std::string());
           --m_cursorPos;
        }
        break;

    default:
        if ((m_repeatKeysym.sym >= GKey::SPACE) &&
            (m_repeatKeysym.sym <= 'z')) {

            // Insert character
            char c = m_repeatKeysym.unicode & 0xFF;
            m_userValue = 
                m_userValue.substr(0, m_cursorPos) + 
                c +
                ((m_cursorPos < (int)m_userValue.size()) ? 
                  m_userValue.substr(m_cursorPos, std::string::npos) :
                 std::string());
            ++m_cursorPos;

        } else {
            // This key wasn't processed by the console
            debugAssertM(false, "Unexpected repeat key");
        }
    }

    if (m_editing && (m_update == IMMEDIATE_UPDATE)) {
        m_oldValue = m_userValue;
        commit();
        GEvent response;
        response.gui.type = GEventType::GUI_CHANGE;
        response.gui.control = m_eventSource;
        m_gui->fireEvent(response);
    }

}


void GuiTextBox::commit() {
    *m_value = m_userValue;
}


bool GuiTextBox::onEvent(const GEvent& event) {
    if (! m_visible) {
        return false;
    }

    switch (event.type) {
    case GEventType::KEY_DOWN:
        switch (event.key.keysym.sym) {
        case GKey::ESCAPE:
            // Stop editing and revert
            m_editing = false;
            {
                GEvent response;
                response.gui.type = GEventType::GUI_CANCEL;
                response.gui.control = m_eventSource;
                m_gui->fireEvent(response);
            }            
            setFocused(false);
            return true;

        case GKey::RIGHT:
        case GKey::LEFT:
        case GKey::DELETE:
        case GKey::BACKSPACE:
        case GKey::HOME:
        case GKey::END:
            setRepeatKeysym(event.key.keysym);
            processRepeatKeysym();
            return true;

        case GKey::RETURN:
        case GKey::TAB:
            // Edit done
            commit();
            m_editing = false;
            if (m_update == DELAYED_UPDATE) {
                GEvent response;
                response.gui.type = GEventType::GUI_CHANGE;
                response.gui.control = m_eventSource;
                m_gui->fireEvent(response);
            }
            if (event.key.keysym.sym == GKey::RETURN) {
                GEvent response;
                response.gui.type = GEventType::GUI_ACTION;
                response.gui.control = m_eventSource;
                m_gui->fireEvent(response);
            }

            setFocused(false);
            return true;

        default:

            if ((((event.key.keysym.mod & GKeyMod::CTRL) != 0) &&
                 ((event.key.keysym.sym == 'v') || (event.key.keysym.sym == 'y'))) ||

                (((event.key.keysym.mod & GKeyMod::SHIFT) != 0) &&
                (event.key.keysym.sym == GKey::INSERT))) {

                // Paste (not autorepeatable)
                m_userValue = System::getClipboardText();
                return true;

            } else if (((event.key.keysym.mod & GKeyMod::CTRL) != 0) &&
                (event.key.keysym.sym == 'k')) {

                debugAssert(m_cursorPos < (int)m_userValue.size());

                // Cut (not autorepeatable)
                std::string cut = m_userValue.substr(m_cursorPos);
                m_userValue = m_userValue.substr(0, m_cursorPos);

                System::setClipboardText(cut);

                return true;

            } else if ((event.key.keysym.sym >= GKey::SPACE) &&
                (event.key.keysym.sym <= 'z')) {

                // A normal character
                setRepeatKeysym(event.key.keysym);
                processRepeatKeysym();
                return true;

            } else {
                // This key wasn't processed by the console
                return false;
            }
        }
        break;

    case GEventType::KEY_UP:
        if (event.key.keysym.sym == m_repeatKeysym.sym) {
            unsetRepeatKeysym();
            return true;
        }
        break;
    }

    return false;
}


void GuiTextBox::setRect(const Rect2D& rect) {
     m_rect = rect;
     m_clickRect = theme()->textBoxToClickBounds(rect, m_captionSize);
}


}
