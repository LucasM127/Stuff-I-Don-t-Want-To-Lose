#ifndef OBSERVER_HPP
#define OBSERVER_HPP

//public domain...
class Observer
{
public:
    virtual ~Observer(){}
    virtual void onNotify() = 0;
};

class Subject
{
public:
    Subject():m_observer(nullptr){}
    virtual ~Subject(){}
    inline void setObserver(Observer *observer){m_observer = observer;}
    inline void notify(){if(m_observer) m_observer->onNotify();}
private:
    Observer *m_observer;
};

#endif