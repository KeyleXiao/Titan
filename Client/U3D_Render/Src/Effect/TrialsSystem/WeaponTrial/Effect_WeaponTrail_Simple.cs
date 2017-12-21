using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    using EffectCore;
    public sealed class Effect_WeaponTrail_Simple : Effect_WeaponTrailBase
    {
        //平滑程度,简单版本平滑程度只有20
        private int Granularity = 25;
        protected override int GetGranularity()
        {
            return Granularity;
        }
    }

}

