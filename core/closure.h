#ifndef CLOSURE_H
#define CLOSURE_H

struct ClosureBase
{
    virtual ~ClosureBase()
    {
    }
    virtual void execute() = 0;
};

template <class Object>
class Closure : public ClosureBase
{
public:
    Closure(Object* object, void (Object::*method)())
        : _object(object)
        , _method(method)
    {
    }
    void execute()
    {
        (_object->*_method)();
    }

private:
    Object* _object;
    void (Object::*_method)();
};

#endif  // CLOSURE_H
