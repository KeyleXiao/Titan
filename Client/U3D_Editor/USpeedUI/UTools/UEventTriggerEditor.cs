using Data.UISoundConfig;
using System;
using System.Collections.Generic;
using System.Linq;
using UnityEditor;
using UnityEngine;
using UnityEngine.EventSystems;
using USpeedUI;
using UIWidgets;

namespace U3D_Editor.USpeedUI
{
    [CustomEditor(typeof(UEventTrigger), true)]
    class UEventTriggerEditor : Editor
    {
        SerializedProperty m_EventTriggerListProperty;

        GUIContent m_IconToolbarMinus;
        GUIContent m_IconToolbarPlus;
        GUIContent m_EventIDName;
        GUIContent[] m_EventTypes;
        GUIContent[] m_EventHandlerTypes;
        GUIContent m_AddButonContent;

        List<bool> m_EventTriggerToggleGroup;

        UEventTrigger m_CurrentEventTrigger;
        UEventTrigger.EventEntry m_CurrentEventEntry;

        string[] eventHandlerNames;
        string[] eventNames;

        GUIStyle m_EventHandlerLabelStyle;
        GUIStyle m_EventHandlerTextStyle ;
        GUIStyle m_VerticalGroupStyle;

        int toBeAddEntry = -1;

        protected virtual void OnEnable()
        {
            Debug.Log(serializedObject.ToString());
            m_EventTriggerListProperty = serializedObject.FindProperty("m_EventTriggerList");
            m_CurrentEventTrigger = serializedObject.targetObject as UEventTrigger;

            m_AddButonContent = new GUIContent("添加新的事件类型");
            m_EventIDName = new GUIContent("");
            // Have to create a copy since otherwise the tooltip will be overwritten.
            m_IconToolbarMinus = new GUIContent(EditorGUIUtility.IconContent("Toolbar Minus"));
            m_IconToolbarMinus.tooltip = "移除该事件列表";

            m_IconToolbarPlus = new GUIContent(EditorGUIUtility.IconContent("Toolbar Plus"));
            m_IconToolbarPlus.tooltip = "添加子项";

            eventNames = Enum.GetNames(typeof(EventTriggerType));
            
            m_EventTypes = new GUIContent[eventNames.Length];
            for (int i = 0; i < eventNames.Length; ++i)
            {
                m_EventTypes[i] = new GUIContent(eventNames[i]);
            }

            eventHandlerNames = Enum.GetNames(typeof(EEventHandlerType));
            m_EventHandlerTypes = new GUIContent[eventHandlerNames.Length];
            for(int i=0;i<eventHandlerNames.Length;++i)
            {
                m_EventHandlerTypes[i] = new GUIContent(eventHandlerNames[i]);
            }

            m_EventTriggerToggleGroup = new List<bool>();
            m_EventTriggerToggleGroup.AddRange(Enumerable.Repeat(false, m_EventTriggerListProperty.arraySize));




        }

        private void SetGUIStyles()
        {
            m_EventHandlerLabelStyle = new GUIStyle(GUI.skin.label);
            m_EventHandlerTextStyle = new GUIStyle(GUI.skin.textField);
            m_VerticalGroupStyle = new GUIStyle(GUI.skin.box);


            m_EventHandlerLabelStyle.margin = new RectOffset(20, 0, 0, 0);
            m_EventHandlerTextStyle.margin = new RectOffset(20, 0, 0, 0);
            m_VerticalGroupStyle.margin = new RectOffset(20, 0, 0, 0);
        }


        public override void OnInspectorGUI()
        {
            SetGUIStyles();

            serializedObject.Update();
            int toBeRemovedEntry = -1;

            EditorGUILayout.Space();

            Vector2 removeButtonSize = GUIStyle.none.CalcSize(m_IconToolbarMinus);
            Vector2 plusButtonSize = GUIStyle.none.CalcSize(m_IconToolbarPlus);

            for (int i = 0; i < m_EventTriggerListProperty.arraySize; ++i)
            {
                m_CurrentEventEntry = m_CurrentEventTrigger.Triggers[i];
                EventTriggerType entryID = m_CurrentEventEntry.eventID;                 

                EditorGUILayout.BeginVertical();

                float viewWidth = EditorGUIUtility.currentViewWidth - removeButtonSize.x - plusButtonSize.x - 32;

                m_EventTriggerToggleGroup[i] = GUILayout.Toggle(m_EventTriggerToggleGroup[i], eventNames[(int)entryID], "Foldout", GUILayout.Width(viewWidth));

                #region 移除Event 和 增加Event.Handler
                Rect callbackRect = GUILayoutUtility.GetLastRect();
                Rect removeButtonPos = new Rect(EditorGUIUtility.currentViewWidth - removeButtonSize.x - 8, callbackRect.y + 1, removeButtonSize.x, removeButtonSize.y);
                if (GUI.Button(removeButtonPos, m_IconToolbarMinus, GUIStyle.none))
                {
                    toBeRemovedEntry = i;
                }

                Rect plusButtonPos = new Rect(EditorGUIUtility.currentViewWidth - removeButtonPos.width - plusButtonSize.x - 8, callbackRect.y + 1, plusButtonSize.x, plusButtonSize.y);
                if (GUI.Button(plusButtonPos, m_IconToolbarPlus, GUIStyle.none))
                {
                    ShowAddHandlerMenu();
                    toBeAddEntry = i;
                }
                #endregion

                if (m_EventTriggerToggleGroup[i])
                {

                    SerializedProperty eventEntryProperty = m_EventTriggerListProperty.GetArrayElementAtIndex(i);
                    if (eventEntryProperty != null)
                    {
                        SerializedProperty eventHandlerList = eventEntryProperty.FindPropertyRelative("m_EventHandlerList");
                        if (eventHandlerList != null)
                        {
                            int toBeRemoveHandler = -1;

                            for (int j = 0; j < eventHandlerList.arraySize; ++j)
                            {
                                SerializedProperty handler = eventHandlerList.GetArrayElementAtIndex(j);
                                if(handler != null)
                                {
                                    m_EventIDName.text = eventHandlerNames[handler.FindPropertyRelative("m_handlerType").enumValueIndex];

                                    EditorGUILayout.BeginVertical(m_VerticalGroupStyle);

                                    EditorGUILayout.LabelField(m_EventIDName, m_EventHandlerLabelStyle);

                                    Rect handlerRemoveRect = GUILayoutUtility.GetLastRect();
                                    Rect handlerremoveButtonPos = new Rect(EditorGUIUtility.currentViewWidth - removeButtonSize.x - 8, handlerRemoveRect.y + 1, removeButtonSize.x, removeButtonSize.y);
                                    if (GUI.Button(handlerremoveButtonPos, m_IconToolbarMinus, GUIStyle.none))
                                    {
                                        toBeRemoveHandler = j;
                                    }
                                    DrawHandlerInfoProperty(entryID,handler);

                                    EditorGUILayout.EndVertical();
                                    EditorGUILayout.Space();
                                }
                            }

                            if (toBeRemoveHandler > -1)
                            {
                                eventHandlerList.DeleteArrayElementAtIndex(toBeRemoveHandler);
                            }

                        }
                    }







                    //if (m_CurrentEventEntry.EventHandlerList != null)
                    //{
                    //    int toBeRemoveHandler = -1;
                    //    for (int j = 0; j < m_CurrentEventEntry.EventHandlerList.Count; ++j)
                    //    {
                    //        EventHandlerBase handler = m_CurrentEventEntry.EventHandlerList[j];

                    //        m_EventIDName.text = eventHandlerNames[(int)handler.HandlerType];

                    //        EditorGUILayout.BeginVertical(m_VerticalGroupStyle);

                    //        EditorGUILayout.LabelField(m_EventIDName, m_EventHandlerLabelStyle);

                    //        Rect handlerRemoveRect = GUILayoutUtility.GetLastRect();
                    //        Rect handlerremoveButtonPos = new Rect(EditorGUIUtility.currentViewWidth - removeButtonSize.x - 8, handlerRemoveRect.y + 1, removeButtonSize.x, removeButtonSize.y);
                    //        if (GUI.Button(handlerremoveButtonPos, m_IconToolbarMinus, GUIStyle.none))
                    //        {
                    //            toBeRemoveHandler = j;
                    //        }

                    //        //EditorGUI.LabelField(handlerRect,m_EventIDName);
                    //        DrawHandlerInfo(handler);
                    //        EditorGUILayout.EndVertical();
                    //        EditorGUILayout.Space();
                    //    }

                    //    if (toBeRemoveHandler > -1)
                    //    {
                    //        m_CurrentEventEntry.EventHandlerList.RemoveAt(toBeRemoveHandler);
                    //    }
                    //}

                    EditorGUILayout.Space();
                }

                EditorGUILayout.EndVertical();

            }

            if (toBeRemovedEntry > -1)
            {
                RemoveEntry(toBeRemovedEntry);
            }

            Rect btPosition = GUILayoutUtility.GetRect(m_AddButonContent, GUI.skin.button);
            const float addButonWidth = 200f;
            btPosition.x = btPosition.x + (btPosition.width - addButonWidth) / 2;
            btPosition.width = addButonWidth;
            if (GUI.Button(btPosition, m_AddButonContent))
            {
                ShowAddEventMenu();
            }

            serializedObject.ApplyModifiedProperties();
        }

        //private void DrawHandlerInfo(EventHandlerBase _handler)
        //{
        //    switch(_handler.HandlerType)
        //    {
        //        case EEventHandlerType.Sound:
        //            DrawSoundHandlerInfo(_handler);
        //            break;
        //        default:
        //            break;
        //    }
        //}

        private void DrawHandlerInfoProperty(EventTriggerType _eTriggerType, SerializedProperty _handler)
        {
            int handlerType = _handler.FindPropertyRelative("m_handlerType").enumValueIndex;
            switch(handlerType)
            {
                case (int)EEventHandlerType.EventCallback:
                    DrawEventCallback(_eTriggerType, _handler);
                    break;
                case (int)EEventHandlerType.Sound:
                    DrawSoundHandlerInfo(_eTriggerType,_handler);
                    break;

            }
        }

        private void DrawSoundHandlerInfo(EventTriggerType _eTriggerType,SerializedProperty _handler)
        {
            if (_handler == null)
                return;

            UISoundConfig.Instance.Load_Editor();

            //取音效ID拆分作用域和关键字
            List<string> USoundIDList = UISoundConfig.Instance.GetSoundNameList();

            if (USoundIDList == null)
            {
                EditorGUILayout.LabelField("获取UI音效ID列表失败。");
                return;
            }

            {
                //取筛选器
                List<string> filters = new List<string>();
                

                UISoundConfig.Instance.GetSoundFilterList(ref filters);

                if (filters.Count == 0)
                    return;

                SSchemeUSoundData ssUoundData = UISoundConfig.Instance.GetData(_handler.FindPropertyRelative("m_strSoundName").stringValue);

                string curFilter = ssUoundData == null ? string.Empty:ssUoundData.USoundFilter;
                string curKey = ssUoundData == null ? string.Empty:ssUoundData.USoundKey;

                int filterIndex = filters.IndexOf(curFilter);
                int selectFilterIndex = EditorGUILayout.Popup("Filter:",filterIndex,filters.ToArray());

                if (filterIndex == -1)
                    curFilter = filters[0];
                if(selectFilterIndex != filterIndex)
                {
                    curFilter = filters[selectFilterIndex];
                    EditorUtility.SetDirty(target);
                }
                //取关键字
                List<string> keys = new List<string>();
                UISoundConfig.Instance.GetSoundFilterKeyList(curFilter, ref keys);

                if (keys.Count == 0)
                    return;

                int keyIndex = keys.IndexOf(curKey);
                int selectKeyIndex = EditorGUILayout.Popup("Key:", keyIndex, keys.ToArray());
                if (keyIndex == -1)
                    curKey = keys[0];
                if (selectKeyIndex != keyIndex)
                {
                    curKey = keys[selectKeyIndex];
                    EditorUtility.SetDirty(target);
                }

                string tmpSoundName = string.Empty ;
                UISoundConfig.Instance.GetSoundName(curFilter, curKey, ref tmpSoundName);
                _handler.FindPropertyRelative("m_strSoundName").stringValue = tmpSoundName;

                if (!string.IsNullOrEmpty(tmpSoundName))
                {
                    SSchemeUSoundData data = UISoundConfig.Instance.GetData(tmpSoundName);
                    if (data != null)
                    {
                        EditorGUILayout.LabelField("SoundID:", data.USoundID.ToString());
                    }
                }
            }

            ////下面只是另一种方法而已，不容易理解
            return;

            /////////////////////////////////////////////////////////////

            //string[] FilterList = new string[USoundIDList.Count];
            //string[] KeyList = new string[USoundIDList.Count];
            //string[] IDSplit;

            //for (int i=0;i< USoundIDList.Count;++i)
            //{
            //    IDSplit = USoundIDList[i].Split(new char[] { '_'},2);

            //    FilterList[i] = IDSplit[0];
            //    KeyList[i] = IDSplit[1];
            //}


            //int USoundFilter = 0;
            //int NewUSoundFilter = 0;
            //int USoundKey = 0;


            //if (_handler.SoundName != null)
            //{
            //    IDSplit = _handler.SoundName.Split(new char[] { '_' }, 2);
            //    if(IDSplit.Length == 2)
            //    {
            //        USoundFilter = Array.IndexOf(FilterList, IDSplit[0]);
            //        USoundKey = Array.IndexOf(KeyList, IDSplit[1]);
            //    }
            //}

            //NewUSoundFilter = EditorGUILayout.Popup("Filter:", USoundFilter, FilterList);


            //List<string> SoundKeyList = new List<string>();
            //USoundIDList.ForEach((v, i) => 
            //{
            //    if(v.IndexOf(FilterList[NewUSoundFilter]) == 0)
            //    {
            //        SoundKeyList.Add(KeyList[i]);
            //    }
            //});

            
            //USoundKey = USoundFilter != NewUSoundFilter ? 0:SoundKeyList.IndexOf(KeyList[USoundKey]);
            //USoundKey = EditorGUILayout.Popup("Key:", USoundKey, SoundKeyList.ToArray());

            //_handler.SoundName = FilterList[NewUSoundFilter] +'_'+ SoundKeyList[USoundKey];

            
            //EditorGUILayout.LabelField("SoundID:", _handler.GetSoundID().ToString());
        }

        private void DrawEventCallback(EventTriggerType _eTriggerType, SerializedProperty _handler)
        {
            SerializedProperty eventCallback = _handler.FindPropertyRelative("m_eventCallback");
            if(eventCallback != null)
            {
                m_EventIDName.text = eventNames[(int)_eTriggerType];
                EditorGUILayout.PropertyField(eventCallback, m_EventIDName);
            }
        }

        private void RemoveEntry(int toBeRemovedEntry)
        {
            m_CurrentEventTrigger.Triggers.RemoveAt(toBeRemovedEntry);
            m_EventTriggerToggleGroup.RemoveAt(toBeRemovedEntry);
        }

        void ShowAddEventMenu()
        {
            // Now create the menu, add items and show it
            GenericMenu menu = new GenericMenu();
            for (int i = 0; i < m_EventTypes.Length; ++i)
            {
                bool active = true;

                // Check if we already have a Entry for the current eventType, if so, disable it
                for (int p = 0; p < m_EventTriggerListProperty.arraySize; ++p)
                {
                    UEventTrigger.EventEntry delegateEntry = m_CurrentEventTrigger.Triggers[p];
                    
                    if ((int)delegateEntry.eventID == i)
                    {
                        active = false;
                    }
                }
                if (active)
                    menu.AddItem(m_EventTypes[i], false, OnAddNewSelected, i);
                else
                    menu.AddDisabledItem(m_EventTypes[i]);
            }
            menu.ShowAsContext();
            Event.current.Use();
        }

        void ShowAddHandlerMenu()
        {
            // Now create the menu, add items and show it
            GenericMenu menu = new GenericMenu();
            for (int i = 0; i < m_EventHandlerTypes.Length; ++i)
            {
                menu.AddItem(m_EventHandlerTypes[i], false, OnAddNewHandlerSelected, i);
            }
            menu.ShowAsContext();
            Event.current.Use();
        }

        private void OnAddNewSelected(object index)
        {
            int selected = (int)index;
            if (m_CurrentEventTrigger == null)
                return;


            m_CurrentEventTrigger.AddEventEntry((EventTriggerType)index);

            serializedObject.ApplyModifiedProperties();

            m_EventTriggerToggleGroup.Add(false);
        }

        private void OnAddNewHandlerSelected(object index)
        {
            EEventHandlerType handlerType = (EEventHandlerType)index;
            EventHandlerBase newHandler = null;

            newHandler = m_CurrentEventTrigger.CreateEventEntryHandler(handlerType);

            if (newHandler == null)
                return;

            m_CurrentEventTrigger.Triggers[toBeAddEntry].AddEventHandler(newHandler);

            serializedObject.ApplyModifiedProperties();
        }
    }
}
