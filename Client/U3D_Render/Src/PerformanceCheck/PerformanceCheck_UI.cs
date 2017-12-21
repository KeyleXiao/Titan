using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

public class PerformanceCheck_UI : MonoBehaviour
{
    public Image ImgEffect;
    public Text CheckTimer;
    public Button SkipBtn;
    public GameObject SkipConfirmFrame;
    public Button ConfirmBtn;
    public Button CancelBtn;

    public float rotateSpeed = 20f;

    private bool bStart = false;

    public void Init()
    {
        bStart = false;

        this.gameObject.SetActive(false);

        if (SkipBtn)
        {
            SkipBtn.interactable = false;
            SkipBtn.onClick.AddListener(SkipBtnClick);
        }

        if (ConfirmBtn)
            ConfirmBtn.onClick.AddListener(ConfirmBtnClick);

        if (CancelBtn)
            CancelBtn.onClick.AddListener(CancelBtnClick);

        if (SkipConfirmFrame)
            SkipConfirmFrame.gameObject.SetActive(false);
    }

    public void OnDestroy()
    {
        if (SkipBtn)
            SkipBtn.onClick.RemoveListener(SkipBtnClick);

        if (ConfirmBtn)
            ConfirmBtn.onClick.RemoveListener(ConfirmBtnClick);

        if (CancelBtn)
            CancelBtn.onClick.RemoveListener(CancelBtnClick);

        if (SkipConfirmFrame)
            SkipConfirmFrame.gameObject.SetActive(false);
    }

    public void FixedUpdate()
    {
        if (!bStart)
            return;

        if (ImgEffect)
        {
            Vector3 rotate = ImgEffect.rectTransform.localEulerAngles;
            if (rotate.z <= -360.0f)
            {
                rotate.z = 0;
            }

            rotate.z -= Time.deltaTime * rotateSpeed;

            ImgEffect.rectTransform.localEulerAngles = rotate;
        }
    }

    public void OnUpdate()
    {
        if (!bStart)
            return;

        if(CheckTimer)
            CheckTimer.text = PerformanceCheckManager.Instance.flowShareData.currentCheckTime.ToString();

        if (PerformanceCheckManager.Instance.flowShareData.currentCheckTime <= PerformanceCheckManager.HoldDuringCheckTime - 10)
            AllowSkip();

        //超时，直接去到完成，然后根据结果来设置
        if (PerformanceCheckManager.Instance.flowShareData.currentCheckTime <= 0)
        {
            PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
        }
    }

    public void StartCheck()
    {
        this.gameObject.SetActive(true);
        bStart = true;
    }

    public void AllowSkip()
    {
        if (SkipBtn)
        {
            SkipBtn.interactable = true;
        }
    }


    public void SkipBtnClick()
    {
        if (SkipConfirmFrame)
            SkipConfirmFrame.gameObject.SetActive(true);
    }

    public void ConfirmBtnClick()
    {
        if (SkipConfirmFrame)
            SkipConfirmFrame.gameObject.SetActive(false);        
        
        PerformanceCheckManager.Instance.ReslutData.NotDoPerformanceCheck = true;
        PerformanceCheckManager.Instance.ChangeFlow(CheckFlow.Finish);
    }

    public void CancelBtnClick()
    {
        if (SkipConfirmFrame)
            SkipConfirmFrame.gameObject.SetActive(false);
    }
}

