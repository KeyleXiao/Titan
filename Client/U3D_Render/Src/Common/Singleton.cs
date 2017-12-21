using UnityEngine;
using System.Collections;

public class Singleton<T> where T: new(){

    // Instead of compile time check, we provide a run time check
    // to make sure there is only one instance.
    //protected Singleton() { Debug.Assert(null == instance); }

    protected static T instance = new T();
    public static T Instance
    {
        get { return instance; }
    }
}

//public class SingletonEx<T> : MonoBehaviour where T : new()
//{
//    // Instead of compile time check, we provide a run time check
//    // to make sure there is only one instance.
//    //protected Singleton() { Debug.Assert(null == instance); }

//    protected static T instance = new T();
//    public static T Instance
//    {
//        get { return instance; }
//    }
//}
