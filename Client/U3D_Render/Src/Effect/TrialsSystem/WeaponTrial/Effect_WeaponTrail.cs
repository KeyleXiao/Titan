using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    using EffectCore;
    public sealed class Effect_WeaponTrail : Effect_WeaponTrailBase
    {
        //ƽ���̶�,�򵥰汾ƽ���̶�ֻ��40
        private int Granularity = 40;
        protected override int GetGranularity()
        {
            return Granularity;
        }
    }

}

