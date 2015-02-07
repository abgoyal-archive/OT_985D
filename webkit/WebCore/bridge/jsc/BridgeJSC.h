

#ifndef BridgeJSC_h
#define BridgeJSC_h

#if USE(JSC)

#include <runtime/JSString.h>
#include <wtf/HashMap.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>

namespace JSC  {

class ArgList;
class Identifier;
class JSGlobalObject;
class PropertyNameArray;
class RuntimeObjectImp;

namespace Bindings {

class Instance;
class Method;
class RootObject;

typedef Vector<Method*> MethodList;

class Field {
public:
    virtual JSValue valueFromInstance(ExecState*, const Instance*) const = 0;
    virtual void setValueToInstance(ExecState*, const Instance*, JSValue) const = 0;

    virtual ~Field() { }
};

class Class : public Noncopyable {
public:
    virtual MethodList methodsNamed(const Identifier&, Instance*) const = 0;
    virtual Field* fieldNamed(const Identifier&, Instance*) const = 0;
    virtual JSValue fallbackObject(ExecState*, Instance*, const Identifier&) { return jsUndefined(); }

    virtual ~Class() { }
};

typedef void (*KJSDidExecuteFunctionPtr)(ExecState*, JSObject* rootObject);

class Instance : public RefCounted<Instance> {
public:
    Instance(PassRefPtr<RootObject>);

    static void setDidExecuteFunction(KJSDidExecuteFunctionPtr func);
    static KJSDidExecuteFunctionPtr didExecuteFunction();

    // These functions are called before and after the main entry points into
    // the native implementations.  They can be used to establish and cleanup
    // any needed state.
    void begin();
    void end();

    virtual Class* getClass() const = 0;
    RuntimeObjectImp* createRuntimeObject(ExecState*);
    void willInvalidateRuntimeObject();
    void willDestroyRuntimeObject();

    // Returns false if the value was not set successfully.
    virtual bool setValueOfUndefinedField(ExecState*, const Identifier&, JSValue) { return false; }

    virtual JSValue invokeMethod(ExecState*, const MethodList&, const ArgList& args) = 0;

    virtual bool supportsInvokeDefaultMethod() const { return false; }
    virtual JSValue invokeDefaultMethod(ExecState*, const ArgList&) { return jsUndefined(); }

    virtual bool supportsConstruct() const { return false; }
    virtual JSValue invokeConstruct(ExecState*, const ArgList&) { return JSValue(); }

    virtual void getPropertyNames(ExecState*, PropertyNameArray&) { }

    virtual JSValue defaultValue(ExecState*, PreferredPrimitiveType) const = 0;

    virtual JSValue valueOf(ExecState* exec) const = 0;

    RootObject* rootObject() const;

    virtual ~Instance();

    virtual bool getOwnPropertySlot(JSObject*, ExecState*, const Identifier&, PropertySlot&) { return false; }
    virtual bool getOwnPropertyDescriptor(JSObject*, ExecState*, const Identifier&, PropertyDescriptor&) { return false; }
    virtual void put(JSObject*, ExecState*, const Identifier&, JSValue, PutPropertySlot&) { }

protected:
    virtual void virtualBegin() { }
    virtual void virtualEnd() { }
    virtual RuntimeObjectImp* newRuntimeObject(ExecState*);

    RefPtr<RootObject> m_rootObject;

private:
    RuntimeObjectImp* m_runtimeObject;
};

class Array : public Noncopyable {
public:
    Array(PassRefPtr<RootObject>);
    virtual ~Array();

    virtual void setValueAt(ExecState*, unsigned index, JSValue) const = 0;
    virtual JSValue valueAt(ExecState*, unsigned index) const = 0;
    virtual unsigned int getLength() const = 0;

protected:
    RefPtr<RootObject> m_rootObject;
};

const char* signatureForParameters(const ArgList&);

typedef HashMap<RefPtr<UString::Rep>, MethodList*> MethodListMap;
typedef HashMap<RefPtr<UString::Rep>, Method*> MethodMap;
typedef HashMap<RefPtr<UString::Rep>, Field*> FieldMap;

} // namespace Bindings

} // namespace JSC

#endif // USE(JSC)

#endif