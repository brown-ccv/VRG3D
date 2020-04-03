

#ifndef FSAHELPER_H
#define FSAHELPER_H

#include "CommonInc.H"

namespace VRBase {


// Functors allow for callback functions that are class methods
// in C++.  Google "C++ functors" for more information.
class EventCallbackFunctor
{
public:
  EventCallbackFunctor() {}
  virtual ~EventCallbackFunctor() {}
  virtual void exec(VRG3D::EventRef event) = 0;
};

template <class T>
class EventCallbackSpecificFunctor : public EventCallbackFunctor
{
public:
  typedef void (T::*MethodType)(VRG3D::EventRef event);

  EventCallbackSpecificFunctor(T *obj, MethodType meth) {
    _obj = obj;
    _method = meth;
  }

  virtual ~EventCallbackSpecificFunctor() {}

  void exec(VRG3D::EventRef event) {
    (_obj->*_method)(event);
  }
  
protected:
  T          *_obj;
  MethodType  _method;
};
// End functor bullshit



/// FsaArc class is only for use internally within the Fsa class
typedef G3D::ReferenceCountedPointer<class FsaArc> FsaArcRef;
class FsaArc : public G3D::ReferenceCountedObject
{
public:
  FsaArc(const std::string &arcName, 
	 const int &toState, 
	 const G3D::Array<std::string> &triggerEvents);
  FsaArc();  
  virtual ~FsaArc();

  void addCallback(EventCallbackFunctor *f);
  void callCallbacks(VRG3D::EventRef event);

  std::string getName()         const { return _name; }
  int         getTo()           const { return _to; }
  int         getNumTriggers()  const { return _triggers.size(); }
  std::string getTrigger(int i) const { return _triggers[i]; }

protected:
  G3D::Array<EventCallbackFunctor*> _callbacks;
  G3D::Array<std::string>           _triggers;
  std::string                  _name;
  int                          _to;
};


/// FsaState class is only for use internally within the Fsa class
class FsaState
{
public:
  FsaState(const std::string &stateName);  
  FsaState();  
  virtual ~FsaState();
  
  void addArcRef(FsaArcRef a);

  void addEnterCallback(EventCallbackFunctor *f);
  void addExitCallback(EventCallbackFunctor *f);
  
  void callEnterCallbacks();
  void callExitCallbacks();

  FsaArcRef   getArcRef(int i)       { return _arcs[i]; }
  int         getNumArcs()     const { return _arcs.size(); }
  std::string getName()        const { return _name; }

protected:
  std::string      _name;
  G3D::Array<FsaArcRef> _arcs;
  G3D::Array<EventCallbackFunctor*>  _enterCallbacks;
  G3D::Array<EventCallbackFunctor*>  _exitCallbacks;
};


} // end namespace

#endif

