#pragma once
#include "arpch.h"
#include "Aurora/Core/Base.h"


namespace Aurora {
	//我们的事件在Aurora中是阻塞式的
	//当事件发生时,它会立即被调度,并必须立即处理
	//我们需要将事件缓存在事件总线中，这样我们可以在事件处理阶段期间进行处理

	//enum
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled

	};
	//事件类别
	//enum class 使用时有作用域限制,需要EventType:: 引用
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};
	//宏定义 自动化函数重写
#define EVENT_CLASS_TYPE(type)\
						static EventType GetStaticType(){return EventType::type;}\
						virtual EventType GetEventType()const override {return GetStaticType();}\
						virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category)\
						virtual int GetCategoryFlags() const override {return category;}

	//虚基类定义
	class Event
	{
		friend class EventDispatcher;
	public:
		virtual ~Event() = default;
		bool Handled = false;

		virtual EventType GetEventType()const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags()const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
		

		
	};

	//事件分发器
	class EventDispatcher
	{
		//此处为私有EventFn,为一个函数指针或函数对象,返回的是bool
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			:m_Event(event) {}

		//派发事件
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			//检查当前事件类型是否与T类型匹配
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				//将事件的m_Handled标记为处理结果
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}

