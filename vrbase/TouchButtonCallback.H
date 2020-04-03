/*
 * \Author Bret Jackson
 * \brief callback template for TouchButton widgets
 */

#ifndef TOUCHBUTTONCALLBACK_H
#define TOUCHBUTTONCALLBACK_H

#include "CommonInc.H"

namespace VRBase {

class TouchButtonFunctor
{
public:
	TouchButtonFunctor() {}
	virtual ~TouchButtonFunctor() {}
	virtual void exec(string buttonName) = 0;
};

template<class T>
class SpecificTouchButtonFunctor : public TouchButtonFunctor
{
public:
	typedef void (T::*MethodType)(string buttonName);
	SpecificTouchButtonFunctor (T *obj, MethodType meth) {
		_obj = obj;
		_method = meth;
	}
	virtual ~SpecificTouchButtonFunctor() {}
	
	void exec(string buttonName) {
		(_obj->*_method)(buttonName);
	}
protected:
	T *_obj;
	MethodType _method;
};

} // end namespace
#endif
