using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    using EffectCore;
    public sealed class Effect_WeaponTrail_Simple : Effect_WeaponTrailBase
    {
        //ƽ���̶�,�򵥰汾ƽ���̶�ֻ��20
        private int Granularity = 25;
        protected override int GetGranularity()
        {
            return Granularity;
        }
    }

}

