using System;
using UnityEngine;

public class MouseOrbit : MonoBehaviour
{
    public Transform target;
    public float distance = 5.0f;

    public float Height = 0.0f;

    public float xSpeed = 250.0f;
    public float ySpeed = 120.0f;

    public float ScrollWheelSpeed = 2.0f;

    public float yMinLimit = -20;
    public float yMaxLimit = 80;

    public float x = 0.0f;
    public float y = 0.0f;
    public bool bLocked = false;
    public Camera cam;
    void Start()
    {
        //var angles = transform.eulerAngles;
        //x = angles.y;
        //y = angles.x;
        //ySpeed = 0;
        bLocked = false;
        if(!cam)
        {
            cam = Camera.main;
        }
    }

    void LateUpdate()
    {
        if (!target)
        {
            return;
        }
        distance -= Input.GetAxis("Mouse ScrollWheel") * ScrollWheelSpeed;
        if (!bLocked && target && (Input.GetMouseButton(0) || Input.GetMouseButton(1)))
        {
            var pos = Input.mousePosition;
            if (pos.x < 250 && Screen.height - pos.y < 250) return;
            Cursor.visible = false;
            x += Input.GetAxis("Mouse X") * xSpeed * 0.02f;
            y -= Input.GetAxis("Mouse Y") * ySpeed * 0.02f;

            y = ClampAngle(y, yMinLimit, yMaxLimit);
        }
        else
        {
            Cursor.visible = true;
        }

        Quaternion rotation = Quaternion.Euler(y, x, 0);
        Vector3 position = rotation * new Vector3(0.0f, Height, -distance) + target.position;

        cam.transform.rotation = rotation;
        cam.transform.position = position;
    }

    static float ClampAngle(float angle, float min, float max)
    {
        if (angle < -360)
            angle += 360;
        if (angle > 360)
            angle -= 360;
        return Mathf.Clamp(angle, min, max);
    }
}
