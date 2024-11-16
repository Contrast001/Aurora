#pragma once

#include "Event.h"


namespace Aurora
{
	class AURORA_API MouseMoveEvent:public Event
	{
	public:
		MouseMoveEvent(float x,float y)
			:m_MouseX(x),m_MouseY(y){}
		inline float GetX()	const { return m_MouseX; }
		inline float GetY()	const { return m_MouseY; }
		
		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MouseMoveEvent: " << m_MouseX<<","<<m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		//事件类型：输入事件  输入设备：鼠标
		EVENT_CLASS_CATEGORY(EventCategoryMouse|EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;

	};


	class AURORA_API MouseScrolledEvent :public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			:m_XOffset(xOffset), m_YOffset(yOffset) {}
		inline float GetXOffset()	const { return m_XOffset; }
		inline float GetYOffset()	const { return m_YOffset; }

		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent: " << m_XOffset << "," << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};

	class AURORA_API MouseButtonEvent :public Event
	{
	public:
		inline int GetMouseButton()const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtonEvent(int button)
			:m_Button(button){}
	
		int m_Button;
	};

	class AURORA_API MouseButtonPressedEvent :public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			:MouseButtonEvent(button){}
		
		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MousePressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class AURORA_API MouseButtonReleaseEvent :public MouseButtonEvent
	{
	public:
		MouseButtonReleaseEvent(int button)
			:MouseButtonEvent(button) {}

		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MouseReleaseEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}
