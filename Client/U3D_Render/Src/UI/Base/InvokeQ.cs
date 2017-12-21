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

// ���ã�ĳЩ��ĳ�Ա�������Լ�����myRoot.Invoke()����ʱmyRoot���ܻ�û��ʼ����
// �÷���myRoot���������InvokeQ�����������ԭ������myRoot.Invoke()��ʱ��
// ���ɵ�����������_Invoke��������ԭ��myRoot��ֵ�ĵط����ɵ����������
// ��OnRegister������
// ʵ�֣�_Invoke���õ�ʱ�����myRoot��δ��ʼ����������invoke��������
// �ȵ���ʼ��(OnRegister)��ʱ���Զ�����֮ǰ����������invoke��
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