#pragma once
#include "Event.hpp"

namespace ke
{
    namespace Events
    {
        class WindowResizedEvent : public Event
        {
        public:
            WindowResizedEvent(int width, int height)
                : Event(), mWidth(width), mHeight(height) {}
            
            EVENT_TYPE(WindowFramebufferResizedEvent)
            EVENT_CATEGORY(WindowEvent)

            int getWidth() const {return mWidth;}
            int getHeight() const {return mHeight;}

        private:
            int mHeight;
            int mWidth;
        };
    }
}