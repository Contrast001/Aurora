#pragma once

#include "Event.h"



namespace Aurora{
	

	//CLASS
	class AURORA_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int keycode)
			:m_KeyCode(keycode) {}
		int m_KeyCode;
	};

	class AURORA_API KeyPressedEvent:public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode,int repeatCount)
			:KeyEvent(keycode),m_RepeatCount(repeatCount){}

		inline  int GetRepeatCount()const { return m_RepeatCount; }

		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode;
			return ss.str();
		}
		
		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class AURORA_API KeyReleaseEvent :public KeyEvent
	{
	public:
		KeyReleaseEvent(int keycode)
			:KeyEvent(keycode) {}

	

		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "KeyReleaseEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};
}
