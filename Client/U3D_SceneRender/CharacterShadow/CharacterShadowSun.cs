using UnityEngine;
using System.Collections;

public class CharacterShadowSun : MonoBehaviour
{
	public static Light instance;

	Light m_lightSource;

	public void Awake() {
		m_lightSource = GetComponent<Light>();
		if(!m_lightSource)
            Debug.LogWarning("CharacterShadowSun-找不到灯光 :" + name);
	}

    public void OnEnable()
    {
		if(instance) {
            Debug.LogWarning("CharacterShadowSun-有多个实例，只支持一个实例" + instance.name);
			return;
		}

		instance = m_lightSource;
	}

    public void OnDisable()
    {


		instance = null;
	}
}
