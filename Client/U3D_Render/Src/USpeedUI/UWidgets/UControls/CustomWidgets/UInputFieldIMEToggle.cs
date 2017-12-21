using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class UInputFieldIMEToggle : UIBehaviour
{
    [SerializeField]
    public bool ForbidIME = false;

    private InputField m_InputField;
    private bool bIsFocused;
    protected override void Awake()
    {
        base.Awake();
        bIsFocused = false;
        m_InputField = this.GetComponent<InputField>();
    }

    public void Update()
    {
        if (null != m_InputField && bIsFocused != m_InputField.isFocused)
        {
            bIsFocused = m_InputField.isFocused;
            //GameLogicConfigApi.ActiveIME(bIsFocused);
            InputManager.Available = !bIsFocused;

            if(m_InputField.inputType != InputField.InputType.Password
                && !ForbidIME)
                ImeManager.ActiveIme(bIsFocused);
        }
    }
}

