#pragma once
#include "Event.hpp"
#include <iostream>

typedef u_int32_t uint32_t;

namespace ke
{
    namespace Events
    {

        class MouseEvent : public Event
        {
        public:
            double getMouseX() const {return mMouseX;}
            double getMouseY() const {return mMouseY;}

            EVENT_CATEGORY(MouseEvent)
        protected:
            MouseEvent(double xpos, double ypos)
                : Event(), mMouseX(xpos), mMouseY(ypos) {};
        private:
            double mMouseX;
            double mMouseY;
        };

        class MouseButtonPressedEvent : public MouseEvent
        {
        public:
            MouseButtonPressedEvent(int button, double xpos, double ypos)
                : MouseEvent(xpos, ypos), mMouseButton(button){}

            EVENT_CATEGORY(MouseEvent)
            EVENT_TYPE(MousePressedEvent)

        private:
            int mMouseButton;
        };

        class MouseButtonReleasedEvent : public MouseEvent
        {
        public:
            MouseButtonReleasedEvent(int button, double xpos, double ypos)
                : MouseEvent(xpos, ypos), mMouseButton(button) {}
            
            EVENT_TYPE(MouseReleasedEvent)
            EVENT_CATEGORY(MouseEvent)

        private:
            int mMouseButton;
        };

        class MouseMovedEvent : public MouseEvent
        {
        public:
            MouseMovedEvent(double xpos, double ypos)
                : MouseEvent(xpos, ypos) {}
        
            EVENT_TYPE(MouseMovedEvent)
            EVENT_CATEGORY(MouseEvent)
        };
    }
}