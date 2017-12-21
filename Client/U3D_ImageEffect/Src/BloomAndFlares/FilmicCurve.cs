using UnityEngine;
using System.Collections;
using System;

[Serializable]
public class FilmicCurve 
{
    [SerializeField]
    public float m_BlackPoint = 0.0f;

    [SerializeField]
    public float m_WhitePoint = 4.0f;

    [SerializeField]
    public float m_CrossOverPoint = 0.6f;

    [SerializeField]
    public float m_ToeStrength = 0.98f;

    [SerializeField]
    public float m_ShoulderStrength = 0.0f;

    public float m_k;
    public Vector4 m_ToeCoef;
    public Vector4 m_ShoulderCoef;

    public float ComputeK(float t, float c, float b, float s, float w)
    {
        float num = (1 - t) * (c - b);
        float denom = (1 - s) * (w - c) + (1 - t) * (c - b);

        return num / denom;
    }

    public float Toe(float x, float t, float c, float b, float s, float w, float k)
    {
        float xnum = m_ToeCoef.x * x;
        float xdenom = m_ToeCoef.y * x;

        return (xnum + m_ToeCoef.z) / (xdenom + m_ToeCoef.w);

    }

    public float Shoulder(float x, float t, float c, float b, float s, float w, float k)
    {
        float xnum = m_ShoulderCoef.x * x;
        float xdenom = m_ShoulderCoef.y * x;

        return (xnum + m_ShoulderCoef.z) / (xdenom + m_ShoulderCoef.w) + k;

    }

    public float Graph(float x, float t, float c, float b, float s, float w, float k)
    {
        if (x <= m_CrossOverPoint)
            return Toe(x, t, c, b, s, w, k);

        return Shoulder(x, t, c, b, s, w, k);
    }

    public void StoreK()
    {
        m_k = ComputeK(m_ToeStrength, m_CrossOverPoint, m_BlackPoint, m_ShoulderStrength, m_WhitePoint);
    }

    public void ComputeShaderCoefficients(float t, float c, float b, float s, float w, float k)
    {
        {
            float xNumMul = k * (1 - t);
            float numAdd = k * (1 - t) * -b;
            float xDenomMul = -t;
            float denomAdd = c - (1 - t) * b;
            m_ToeCoef = new Vector4(xNumMul, xDenomMul, numAdd, denomAdd);
        }

        {
            float xNumMul = (1 - k);
            float numAdd = (1 - k) * -c;
            float xDenomMul = s;
            float denomAdd = (1 - s) * w - c;
            m_ShoulderCoef = new Vector4(xNumMul, xDenomMul, numAdd, denomAdd);
        }
    }

    public void UpdateCoefficients()
    {
        StoreK();
        ComputeShaderCoefficients(m_ToeStrength, m_CrossOverPoint, m_BlackPoint, m_ShoulderStrength, m_WhitePoint, m_k);
    }
}
