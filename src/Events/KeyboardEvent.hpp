#pragma once
#include "Event.hpp"


namespace ke
{
    namespace Events
    {

        class KeyEvent : public Event
        {
        public:
            EVENT_CATEGORY(KeyboardEvent);

            int getKeyCode() const {return mKeyCode;}
        protected:
            KeyEvent(int _keycode)
                :Event(), mKeyCode(_keycode){}
        private:
            int mKeyCode;
        };

        class KeyPressedEvent : public KeyEvent
        {
        public:
            KeyPressedEvent(int _keycode, bool isRepeated)
                : KeyEvent(_keycode), mIsRepeated(isRepeated) {}
            
            EVENT_TYPE(KeyPressedEvent);
            EVENT_CATEGORY(KeyboardEvent);

        private:
            bool mIsRepeated = false;
        };

        class KeyReleasedEvent : public KeyEvent
        {
        public:
            KeyReleasedEvent(int _keyCode)
                : KeyEvent(_keyCode) {}
        
            EVENT_TYPE(KeyReleasedEvent)
            EVENT_CATEGORY(KeyboardEvent)
        };
    }
}