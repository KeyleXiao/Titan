using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    using EffectCore;
    public sealed class Effect_WeaponTrail_Simplest : Effect_WeaponTrailBase
    {
        //平滑程度,简单版本的平滑度只有20
        private int Granularity = 18;
        protected override int GetGranularity()
        {
            return Granularity;
        }
    }

}

