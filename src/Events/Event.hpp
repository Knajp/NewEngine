#pragma once
#include <functional>

/*
    Big credit to @TheCherno on YouTube for inspiring me to create an event system
    Event system is based on his
    Check out his channel, it's a lifesaver.
*/

#define BIT(x) (1 << x)

namespace ke
{
    namespace Events
    {
        enum class EventType
        {
            None = 0,
            KeyPressedEvent, KeyReleasedEvent,
            WindowResizedEvent, WindowClosedEvent,
            MousePressedEvent, MouseReleasedEvent, MouseMovedEvent
        };

        enum class EventCategory
        {
            None = 0,
            KeyboardEvent = BIT(1),
            InputEvent = BIT(2),
            WindowEvent = BIT(3),
            MouseEvent = BIT(4)
        };
        inline EventCategory operator&(EventCategory a, EventCategory b)
        {
            return static_cast<EventCategory>(static_cast<int>(a) & static_cast<int>(b));
        }
        inline EventCategory operator|(EventCategory a, EventCategory b)
        {
            return static_cast<EventCategory>(static_cast<int>(a) | static_cast<int>(b));
        }

        //Base Event class, to be implemented in event-specific classes.
        class Event
        {
            friend class EventDispatcher;
        public:
            virtual EventType getType() const = 0;
            virtual EventCategory getCategoryFlags() const = 0;
        protected:
            Event() = default;
        private:
            bool mHandled = false;
        };

        // Event implementation macros to make life easier
        #define EVENT_TYPE(type) static EventType getStaticType() {return EventType::type;} \
            EventType getType() const override {return getStaticType();}
        #define EVENT_CATEGORY(cat) EventCategory getCategoryFlags() const override {return EventCategory::cat;}

        class EventDispatcher
        {
            friend class Event;
            template <typename T>
            using EventFn = std::function<bool(T&)>;
        public:
            EventDispatcher(Event& event)
                : mEvent(event){}

            template<typename T>
            bool Dispatch(EventFn<T> func)
            {
                if(mEvent.getType() != T::getStaticType()) return false;
                
                mEvent.mHandled = func(static_cast<T&>(mEvent));
                return true;
                
            }

        private:
            Event& mEvent;
        };
    }
}