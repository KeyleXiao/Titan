using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    using EffectCore;
    public sealed class Effect_WeaponTrail_Simplest : Effect_WeaponTrailBase
    {
        //ƽ���̶�,�򵥰汾��ƽ����ֻ��20
        private int Granularity = 18;
        protected override int GetGranularity()
        {
            return Granularity;
        }
    }

}

