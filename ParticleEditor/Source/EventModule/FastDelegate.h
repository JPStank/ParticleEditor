#pragma once
//--------------------------------------------------------------------------------
namespace Events {
	//Used when a fast call is needed, does not work with inheridence since the class type is selected from the function prototype
	//
	template <typename returnType, typename ...arg>
	class FastDelegate {
		using functionType = returnType(*)(void*, arg...);
	public:
		using globalFunction = returnType(*)(arg...);
		template<typename T> using memberFunction = returnType(T::*)(arg...);
		template<typename T> using constmemberFunction = returnType(T::*)(arg...) const;

		FastDelegate();
		FastDelegate(const FastDelegate & _in);
		FastDelegate(const FastDelegate && _in);
		FastDelegate & operator = (const FastDelegate & _in);
		FastDelegate & operator = (const FastDelegate && _in);
		returnType operator()(arg... _in) const;
		bool operator == (const FastDelegate & _in) {
			if (_in.instance == instance) {
				if (_in.type == type)
					return true;
			}
			return false;
		}
		bool operator!() const;
		operator bool() const;
	private:
		void* instance;
		functionType type;

		FastDelegate(void* _obj, functionType _function);

		//Can't move the implemtnation of the following functions to the inline file.  The const function pointer cast makes the compiler choke
	public:
		template <globalFunction _function> static FastDelegate init() {
			return FastDelegate(0, &functionCall<_function>);
		}
		template <class T, memberFunction<T> _function> static FastDelegate init(T* _obj) {
			return FastDelegate(_obj, &functionCall<T, _function>);
		}
		template <class T, constmemberFunction<T> _function> static  FastDelegate init(T const* _obj) {
			return FastDelegate(const_cast<T*>(_obj), &functionCall<T, _function>);
		}
	private:
		template <globalFunction _function> static returnType functionCall(void*, arg... _in) {
			return (_function)(_in...);
		}
		template <class T, memberFunction<T> _function> static returnType functionCall(void* _obj, arg... _in) {
			return (static_cast<T*>(_obj)->*_function)(_in...);
		}
		template <class T, constmemberFunction<T> _function> static returnType functionCall(void* _obj, arg... _in) {
			return (static_cast<T const*>(_obj)->*_function)(_in...);
		}
	};
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg> FastDelegate<R, arg...>::FastDelegate() : instance(nullptr), type(nullptr) {}
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg> FastDelegate<R, arg...>::FastDelegate(const FastDelegate & _in) : instance(_in.instance), type(_in.type) {}
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg> FastDelegate<R, arg...>::FastDelegate(const FastDelegate && _in) : instance(_in.instance), type(_in.type) {}
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg> FastDelegate<R, arg...>::FastDelegate(void* _obj, functionType _function) : instance(_obj), type(_function) {}
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg> inline FastDelegate<R, arg...>::operator bool() const { return nullptr != type; }
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg> inline bool FastDelegate<R, arg...>::operator!() const { return !(operator bool()); }
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg> inline R FastDelegate<R, arg...>::operator()(arg... _in) const{ return (*type)(instance, _in...); }
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg>
	inline FastDelegate<R, arg...> & FastDelegate<R, arg...>::operator = (const FastDelegate & _in) { instance = _in.instance; type = _in.type; return *this; }
	//--------------------------------------------------------------------------------
	template <typename R, typename ...arg>
	inline FastDelegate<R, arg...> & FastDelegate<R, arg...>::operator = (const FastDelegate && _in) { instance = _in.instance; type = _in.type; return *this; }
	//--------------------------------------------------------------------------------
};



