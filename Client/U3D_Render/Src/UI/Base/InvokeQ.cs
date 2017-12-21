using System;
using System.Collections;
using Scaleform;

public class InvokeElement
{
    public String methodName;
    public object[] objs;

    public InvokeElement(String _method, object[] _objs)
    {
        methodName = _method;

        objs = new object[_objs.Length];
        _objs.CopyTo(objs, 0);
    }
}

// 适用：某些类的成员函数里自己调用myRoot.Invoke()，此时myRoot可能还没初始化。
// 用法：myRoot对象可以用InvokeQ对象替代，在原来调用myRoot.Invoke()的时候，
// 换成调用这个对象的_Invoke函数。在原来myRoot赋值的地方换成调用这个对象
// 的OnRegister函数。
// 实现：_Invoke调用的时候如果myRoot还未初始化，会把这次invoke缓存起来
// 等到初始化(OnRegister)的时候，自动调用之前缓存起来的invoke。
public class InvokeQ
{
    protected Value myRoot = null;
    protected Queue pendQ = null; 

    public InvokeQ()
    {
        myRoot = null;
        pendQ = new Queue();
    }

    public void Executes()
    {
        foreach (InvokeElement ele in pendQ)
        {
            myRoot.Invoke(ele.methodName, ele.objs);
        }

        pendQ.Clear();
    }

    public void OnRegister(Value root)
    {
        myRoot = root;
        Executes();
    }

    public void _Invoke(String methodName, params object[] objs)
    {
        InvokeElement pElement = new InvokeElement(methodName, objs);
        pendQ.Enqueue(pElement);

        if(myRoot == null)
        {
            Trace.LogWarning("" + methodName + "will delay execute,myRoot == null");
            return;
        }

        Executes();
    }
}